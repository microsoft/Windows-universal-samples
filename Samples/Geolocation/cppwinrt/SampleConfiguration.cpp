//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include <winrt/SDKTemplate.h>
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Devices::Geolocation::Geofencing;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Globalization::DateTimeFormatting;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::SDKTemplate;

hstring implementation::MainPage::FEATURE_NAME()
{
    return L"Geolocation C++/WinRT Sample";
}

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
{
    Scenario{ L"Track position", xaml_typename<SDKTemplate::Scenario1_TrackPosition>() },
    Scenario{ L"Get position", xaml_typename<SDKTemplate::Scenario2_GetPosition>() },
    Scenario{ L"Background position", xaml_typename<SDKTemplate::Scenario3_BackgroundTask>() },
    Scenario{ L"Foreground geofencing", xaml_typename<SDKTemplate::Scenario4_ForegroundGeofence>() },
    Scenario{ L"Background geofencing", xaml_typename<SDKTemplate::Scenario5_GeofenceBackgroundTask>() },
    Scenario{ L"Get last visit", xaml_typename<SDKTemplate::Scenario6_GetLastVisit>() },
    Scenario{ L"Foreground visit monitoring", xaml_typename<SDKTemplate::Scenario7_ForegroundVisits>() },
    Scenario{ L"Background visit monitoring", xaml_typename<SDKTemplate::Scenario8_VisitsBackgroundTask>() },
});

// Utility function to convert as much of a string as possible to a uint32_t and detect bad input
wchar_t* SDKTemplate::TryParseUIntPrefix(const wchar_t* str, uint32_t& result)
{
    wchar_t* end;
    errno = 0;
    result = std::wcstoul(str, &end, 0);

    if (str == end)
    {
        // Not parseable.
        return nullptr;
    }

    if (errno == ERANGE)
    {
        // Out of range.
        return nullptr;
    }

    return end;
}

// Utility function to convert a string to a uint32_t and detect bad input
bool SDKTemplate::TryParseUInt(const wchar_t* str, uint32_t& result)
{
    wchar_t* end = TryParseUIntPrefix(str, result);
    if (!end || *end != L'\0')
    {
        result = 0;
        return false;
    }

    return true;
}
// Utility function to convert a string to a double and detect bad input
bool SDKTemplate::TryParseDouble(const wchar_t* str, double& result)
{
    wchar_t* end;
    errno = 0;
    result = std::wcstod(str, &end);

    if (str == end)
    {
        // Not parseable.
        return false;
    }

    if (errno == ERANGE)
    {
        // Out of range.
        result = 0;
        return false;
    }

    if (*end != L'\0')
    {
        // Extra unparseable characters at the end.
        result = 0;
        return false;
    }

    // Disallow infinity and NaN.
    if (!std::isfinite(result))
    {
        return false;
    }

    return true;
}

// Look up a background task by name.
// If found, then also report whether the user has granted background execution access.
IBackgroundTaskRegistration SDKTemplate::LookupBackgroundTask(hstring const& name)
{
    IBackgroundTaskRegistration found;
    // Loop through all background tasks to find one with the matching name
    for (auto [taskGuid, task] : BackgroundTaskRegistration::AllTasks())
    {
        if (task.Name() == name)
        {
            found = task;
            break;
        }
    }

    if (found != nullptr)
    {
        // Check whether the user has already granted background execution access.
        BackgroundAccessStatus backgroundAccessStatus = BackgroundExecutionManager::GetAccessStatus();

        SDKTemplate::MainPage rootPage = SDKTemplate::MainPage::Current();

        switch (backgroundAccessStatus)
        {
        case BackgroundAccessStatus::AlwaysAllowed:
        case BackgroundAccessStatus::AllowedSubjectToSystemPolicy:
            rootPage.NotifyUser(L"Background task is already registered. Waiting for next update...", NotifyType::StatusMessage);
            break;

        default:
            rootPage.NotifyUser(L"Background tasks may be disabled for this app", NotifyType::ErrorMessage);
            break;
        }
    }

    return found;
}

// Check and report whether the user has granted background execution access.
// If so, then also request location access so that the background task can do its work.
fire_and_forget SDKTemplate::CheckBackgroundAndRequestLocationAccess()
{
    // Get permission for a background task from the user. If the user has already answered once,
    // this does nothing and the user must manually update their preference via Settings.
    BackgroundAccessStatus backgroundAccessStatus = co_await BackgroundExecutionManager::RequestAccessAsync();

    SDKTemplate::MainPage rootPage = SDKTemplate::MainPage::Current();

    switch (backgroundAccessStatus)
    {
    case BackgroundAccessStatus::AlwaysAllowed:
    case BackgroundAccessStatus::AllowedSubjectToSystemPolicy:
        // BackgroundTask is allowed
        rootPage.NotifyUser(L"Background task registered.", NotifyType::StatusMessage);

        // Need to request access to location.
        // This must be done when we create the background task registration
        // because the background task cannot display UI.
        // If the user has already answered once,
        // this does nothing and the user must manually update their preference via Settings.
        switch (co_await Geolocator::RequestAccessAsync())
        {
        case GeolocationAccessStatus::Allowed:
            break;

        case GeolocationAccessStatus::Denied:
            rootPage.NotifyUser(L"Access to location is denied.", NotifyType::ErrorMessage);
            break;

        case GeolocationAccessStatus::Unspecified:
            rootPage.NotifyUser(L"Unspecified error!", NotifyType::ErrorMessage);
            break;
        }
        break;

    default:
        rootPage.NotifyUser(L"Background tasks may be disabled for this app", NotifyType::ErrorMessage);
        break;
    }
}

void SDKTemplate::FillItemsFromSavedJson(ItemsControl const& control, hstring const& key)
{
    auto values = Windows::Storage::ApplicationData::Current().LocalSettings().Values();
    hstring json = values.TryLookup(key).try_as<hstring>().value_or(L"");
    if (!json.empty())
    {
        // remove all entries and repopulate
        IVector<IInspectable> items = control.Items();
        items.Clear();

        for (IJsonValue element : JsonValue::Parse(json).GetArray())
        {
            items.Append(box_value(element.GetString()));
        }
    }
}

void SDKTemplate::ReportSavedStatus()
{
    auto values = Windows::Storage::ApplicationData::Current().LocalSettings().Values();
    hstring status = values.TryLookup(L"Status").try_as<hstring>().value_or(L"");
    if (!status.empty())
    {
        SDKTemplate::MainPage::Current().NotifyUser(status, NotifyType::StatusMessage);
    }
}

// Extract a message for the current exception.
hstring SDKTemplate::to_message()
{
    try
    {
        throw;
    }
    catch (hresult_error const& e)
    {
        return e.message();
    }
    catch (std::exception const& ex)
    {
        return winrt::to_hstring(ex.what());
    }
    catch (...)
    {
        std::terminate();
    }
}

hstring SDKTemplate::FormatDateTime(DateTime value, hstring const& format)
{
    return DateTimeFormatter(format).Format(value);
}

hstring winrt::to_hstring(IReference<double> const& value)
{
    return value ? to_hstring(value.Value()) : L"N/A";
}

hstring winrt::to_hstring(TimeSpan value)
{
    uint32_t days = static_cast<uint32_t>(value / std::chrono::hours(24));
    value = value % std::chrono::hours(24);
    uint32_t hours = static_cast<uint32_t>(value / std::chrono::hours(1));
    value = value % std::chrono::hours(1);
    uint32_t minutes = static_cast<uint32_t>(value / std::chrono::minutes(1));
    value = value % std::chrono::minutes(1);
    uint32_t seconds = static_cast<uint32_t>(value / std::chrono::seconds(1));

    return to_hstring(days) + L":" + to_hstring(hours) + L":" + to_hstring(minutes) + L":" + to_hstring(seconds);
}

hstring winrt::to_hstring(PositionSource value)
{
    switch (value)
    {
    case PositionSource::Cellular:
        return L"Cellular";
    case PositionSource::Satellite:
        return L"Satellite";
    case PositionSource::WiFi:
        return L"WiFi";
    case PositionSource::IPAddress:
        return L"IPAddress";
    case PositionSource::Unknown:
        return L"Unknown";
    case PositionSource::Default:
        return L"Default";
    case PositionSource::Obfuscated:
        return L"Obfuscated";
    }
    return to_hstring(static_cast<uint32_t>(value));
}

hstring winrt::to_hstring(VisitStateChange value)
{
    switch (value)
    {
    case VisitStateChange::TrackingLost:
        return L"TrackingLost";
    case VisitStateChange::Arrived:
        return L"Arrived";
    case VisitStateChange::Departed:
        return L"Departed";
    case VisitStateChange::OtherMovement:
        return L"OtherMovement";
    }
    return to_hstring(static_cast<uint32_t>(value));
}

hstring winrt::to_hstring(GeofenceState value)
{
    if (value == GeofenceState::None)
    {
        return L"None";
    }

    std::wstring result;

    if ((value & GeofenceState::Entered) == GeofenceState::Entered)
    {
        value &= ~GeofenceState::Entered;
        result.append(L" | Entered");
    }
    if ((value & GeofenceState::Exited) == GeofenceState::Exited)
    {
        value &= ~GeofenceState::Exited;
        result.append(L" | Exited");
    }
    if ((value & GeofenceState::Removed) == GeofenceState::Removed)
    {
        value &= ~GeofenceState::Removed;
        result.append(L" | Removed");
    }
    if (value != GeofenceState::None)
    {
        result.append(L" | ");
        result.append(std::to_wstring(static_cast<uint32_t>(value)));
    }

    return hstring{ result.substr(3) };
}

hstring winrt::to_hstring(GeofenceMonitorStatus value)
{
    switch (value)
    {
    case GeofenceMonitorStatus::Ready:
        return L"Ready";
    case GeofenceMonitorStatus::Initializing:
        return L"Initializing";
    case GeofenceMonitorStatus::NoData:
        return L"NoData";
    case GeofenceMonitorStatus::Disabled:
        return L"Disabled";
    case GeofenceMonitorStatus::NotInitialized:
        return L"NotInitialized";
    case GeofenceMonitorStatus::NotAvailable:
        return L"NotAvailable";
    }
    return to_hstring(static_cast<uint32_t>(value));
}

hstring winrt::to_hstring(GeofenceRemovalReason value)
{
    switch (value)
    {
    case GeofenceRemovalReason::Used:
        return L"Used";
    case GeofenceRemovalReason::Expired:
        return L"Expired";
    }
    return to_hstring(static_cast<uint32_t>(value));
}
