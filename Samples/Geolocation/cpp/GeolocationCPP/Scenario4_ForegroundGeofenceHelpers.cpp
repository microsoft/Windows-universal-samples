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

//
// Scenario4_ForegroundGeofenceHelpers.cpp
// Implementation of the Scenario4 class
//

#include "pch.h"
#include "Scenario4_ForegroundGeofence.xaml.h"

using namespace SDKTemplate;
using namespace SDKTemplate::GeolocationCPP;

using namespace concurrency;
using namespace Platform;
using namespace Windows::Data::Json;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Devices::Geolocation::Geofencing;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

const unsigned int secondsPerMinute = 60;
const unsigned int secondsPerHour = 60 * secondsPerMinute;
const unsigned int secondsPerDay = 24 * secondsPerHour;
const unsigned int oneHundredNanosecondsPerSecond = 10000000;
const unsigned int timeComponentBufferSize = 10;

const unsigned int maxEventDescriptors = 42;    // Value determined by how many max length event descriptors (91 chars) 
// stored as a JSON string can fit in 8K (max allowed for local settings)

void Scenario4::OnSetStartTimeToNow(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    SetStartTimeToNowFunction();
}

void Scenario4::SetStartTimeToNowFunction()
{
    try
    {
        calendar->SetToNow();

        DateTime dt = calendar->GetDateTime();

        StartTime->Text = formatterShortDateLongTime->Format(dt);
    }
    catch (Platform::Exception^ ex)
    {
        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
    }
}

void Scenario4::OnSetPositionToHere(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    SetPositionProgressBar->Visibility = Windows::UI::Xaml::Visibility::Visible;
    SetPositionToHereButton->IsEnabled = false;
    Latitude->IsEnabled = false;
    Longitude->IsEnabled = false;

    // Request a high accuracy position for better accuracy locating the geofence
    geolocator->DesiredAccuracy = PositionAccuracy::High;

    task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());
    geopositionTask.then([this](task<Geoposition^> getPosTask)
    {
        try
        {
            // Get will throw an exception if the task was canceled or failed with an error
            Geoposition^ pos = getPosTask.get();

            Latitude->Text = pos->Coordinate->Point->Position.Latitude.ToString();
            Longitude->Text = pos->Coordinate->Point->Position.Longitude.ToString();

            // clear status
            rootPage->NotifyUser("", NotifyType::StatusMessage);
        }
        catch (task_canceled&)
        {
            rootPage->NotifyUser("Operation canceled", NotifyType::StatusMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
        }
    });

    // restore cursor and re-enable button
    SetPositionProgressBar->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    SetPositionToHereButton->IsEnabled = true;
    Latitude->IsEnabled = true;
    Longitude->IsEnabled = true;
}

Platform::String^ Scenario4::GetTimeStampedMessage(Platform::String^ EventCalled)
{
    Platform::String^ message;

    calendar->SetToNow();

    message = EventCalled + " " + formatterLongTime->Format(calendar->GetDateTime());

    return message;
}

void Scenario4::OnLatitudeTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
    latitudeSet = TextChangedHandlerDouble(false, "Latitude", Latitude);

    CreateGeofenceButton->IsEnabled = SettingsAvailable();
}

void Scenario4::OnLongitudeTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
    longitudeSet = TextChangedHandlerDouble(false, "Longitude", Longitude);

    CreateGeofenceButton->IsEnabled = SettingsAvailable();
}

void Scenario4::OnRadiusTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
    radiusSet = TextChangedHandlerDouble(false, "Radius", Radius);

    CreateGeofenceButton->IsEnabled = SettingsAvailable();
}

void Scenario4::OnIdTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
    // get number of characters
    unsigned int charCount = Id->Text->Length();

    nameSet = (0 != charCount);

    CharCount->Text = charCount.ToString() + " characters";

    CreateGeofenceButton->IsEnabled = SettingsAvailable();
}

void Scenario4::OnRegisteredGeofenceListBoxSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
    IVector<Object^>^ list = e->AddedItems;

    if (0 == list->Size)
    {
        // disable the remove button
        RemoveGeofenceItem->IsEnabled = false;
    }
    else
    {
        // enable the remove button
        RemoveGeofenceItem->IsEnabled = true;

        // update controls with the values from this geofence item
        // get selected item
        GeofenceItem^ item = safe_cast<GeofenceItem^>(RegisteredGeofenceListBox->SelectedItem);

        RefreshControlsFromGeofenceItem(item);

        CreateGeofenceButton->IsEnabled = SettingsAvailable();

    }
}

void Scenario4::GetTimeComponentAsString(Platform::String^& str, int timecomponent, bool appenddelimiter)
{
    if (0 != timecomponent)
    {
        if (timecomponent < 10 && str->Length())
        {
            str += L"0";
        }

        str += timecomponent.ToString();
    }
    else if (str->Length())
    {
        str += L"00";
    }

    if (appenddelimiter && str->Length())
    {
        str += L":";
    }
}

Platform::String^ Scenario4::GetDurationString(long long duration)
{
    long long totalSeconds = duration / oneHundredNanosecondsPerSecond;

    int days = safe_cast<int>(totalSeconds / secondsPerDay);

    totalSeconds -= days * secondsPerDay;

    int hours = safe_cast<int>(totalSeconds / secondsPerHour);

    totalSeconds -= hours * secondsPerHour;

    int minutes = safe_cast<int>(totalSeconds / secondsPerMinute);

    totalSeconds -= minutes * secondsPerMinute;

    int seconds = safe_cast<int>(totalSeconds);

    Platform::String^ str;

    GetTimeComponentAsString(str, days, true);
    GetTimeComponentAsString(str, hours, true);
    GetTimeComponentAsString(str, minutes, true);
    GetTimeComponentAsString(str, seconds, false);

    return str;
}

void Scenario4::RefreshControlsFromGeofenceItem(GeofenceItem^ item)
{
    if (nullptr != item)
    {
        Id->Text = item->Id;
        Latitude->Text = item->Latitude.ToString();
        Longitude->Text = item->Longitude.ToString();
        Radius->Text = item->Radius.ToString();
        SingleUse->IsChecked = item->SingleUse;

        if (0 != item->StartTime)
        {
            DateTime dt;
            dt.UniversalTime = item->StartTime;

            StartTime->Text = formatterShortDateLongTime->Format(dt);
        }
        else
        {
            StartTime->Text = "";
        }

        if (0 != item->DwellTime)
        {
            DwellTime->Text = GetDurationString(item->DwellTime);
        }
        else
        {
            DwellTime->Text = "";
        }

        if (0 != item->Duration)
        {
            Duration->Text = GetDurationString(item->Duration);
        }
        else
        {
            Duration->Text = "";
        }

        // Update flags used to enable Create Geofence button
        OnIdTextChanged(nullptr, nullptr);
        OnLongitudeTextChanged(nullptr, nullptr);
        OnLatitudeTextChanged(nullptr, nullptr);
        OnRadiusTextChanged(nullptr, nullptr);
    }
}

bool Scenario4::TextChangedHandlerDouble(bool nullAllowed, Platform::String^ name, Windows::UI::Xaml::Controls::TextBox^ e)
{
    bool valueSet = false;

    if (e->Text->IsEmpty())
    {
        if (false == nullAllowed)
        {
            if (nullptr != name)
            {
                auto strMsg = name + " needs a value";

                rootPage->NotifyUser(strMsg, NotifyType::StatusMessage);
            }
        }
        else
        {
            valueSet = true;
        }
    }
    else
    {
        valueSet = true;

        double value = FromStringTo<double>(e->Text);

        if (0 == value)
        {
            // make sure string was '0'
            if ("0" != e->Text)
            {
                auto strMsg = name + " must be a number";

                rootPage->NotifyUser(strMsg, NotifyType::StatusMessage);

                valueSet = false;
            }
        }

        if (true == valueSet)
        {
            // clear out status message
            rootPage->NotifyUser("", NotifyType::StatusMessage);
        }
    }

    return valueSet;
}

bool Scenario4::TextChangedHandlerInt(bool nullAllowed, Platform::String^ name, Windows::UI::Xaml::Controls::TextBox^ e)
{
    bool valueSet = false;

    if (e->Text->IsEmpty())
    {
        if (false == nullAllowed)
        {
            if (nullptr != name)
            {
                auto strMsg = name + " needs a value";

                rootPage->NotifyUser(strMsg, NotifyType::StatusMessage);
            }
        }
        else
        {
            valueSet = true;
        }
    }
    else
    {
        valueSet = true;

        int value = FromStringTo<int>(e->Text);

        if (0 == value)
        {
            // make sure string was '0'
            if ("0" != e->Text)
            {
                auto strMsg = name + " must be a number";

                rootPage->NotifyUser(strMsg, NotifyType::StatusMessage);

                valueSet = false;
            }
        }

        if (true == valueSet)
        {
            // clear out status message
            rootPage->NotifyUser("", NotifyType::StatusMessage);
        }
    }

    return valueSet;
}

// are settings available so a geofence can be created?
bool Scenario4::SettingsAvailable()
{
    bool fSettingsAvailable = false;

    if (true == nameSet &&
        true == latitudeSet &&
        true == longitudeSet &&
        true == radiusSet)
    {
        // also need to test if data is good
        fSettingsAvailable = true;
    }

    return fSettingsAvailable;
}

// add geofence to listbox
void Scenario4::AddGeofenceToRegisteredGeofenceListBox(Windows::Devices::Geolocation::Geofencing::Geofence^ geofence)
{
    GeofenceItem^ item = ref new GeofenceItem(geofence);

    // The registered geofence listbox is data bound
    // to a collection
    // so once the GeofenceItem is added to the 
    // collection it will appear in the 
    // registered geofence listbox.
    geofenceCollection->InsertAt(0, item);
}

long long Scenario4::GetDurationFromString(Platform::String^ str)
{
    long long result = 0;
    long tally = 0;

    std::wistringstream ss(str->Data());

    // create an array of strings to hold time information
    // (days/hours/minutes/seconds)
    std::vector<std::array<wchar_t, timeComponentBufferSize>> v;

    // parse string using the : delimiter
    for (std::array<wchar_t, timeComponentBufferSize> a; ss.getline(&a[0], timeComponentBufferSize, ':');)
    {
        v.push_back(a);
    }

    auto start = 4 - v.size();

    if (start >= 0)
    {
        // convert string to a number
        enum { day = 0, hour, minute, second };
        auto component = start;
        bool stayinloop = true;
        for (auto& a : v)
        {
            int timeComponentResult = _wtoi(&a[0]);

            switch (component)
            {
            case day:
                timeComponentResult *= secondsPerDay;
                break;

            case hour:
                timeComponentResult *= secondsPerHour;
                break;

            case minute:
                timeComponentResult *= secondsPerMinute;
                break;

            case second:
                break;

            default:
                stayinloop = false;
                break;
            }

            if (!stayinloop)
            {
                break;
            }

            tally += timeComponentResult;
            ++component;
        }
    }

    result = tally;
    result *= oneHundredNanosecondsPerSecond;

    return result;
}

long long Scenario4::GetUniversalTimeFromString(Platform::String^ str)
{
    long long result = 0;

    std::tm t;
    std::wistringstream ss(str->Data());
    ss >> std::get_time(&t, L"%m/%d/%Y %H:%M:%S");

    SYSTEMTIME st = { 0 };
    st.wYear = t.tm_year + 1900;
    st.wMonth = t.tm_mon + 1;
    st.wDay = t.tm_mday;
    st.wHour = t.tm_hour;
    st.wMinute = t.tm_min;
    st.wSecond = t.tm_sec;

    // now convert to UTC
    SYSTEMTIME stUTC = { 0 };
    TIME_ZONE_INFORMATION tzi = { 0 };
    if (0 != ::GetTimeZoneInformation(&tzi))
    {
        if (TRUE == ::TzSpecificLocalTimeToSystemTime(&tzi, &st, &stUTC))
        {
            FILETIME ft;

            if (TRUE == ::SystemTimeToFileTime(&stUTC, &ft))
            {
                _ULARGE_INTEGER ulint = { ft.dwLowDateTime, ft.dwHighDateTime };

                result = ulint.QuadPart;
            }
        }
    }

    return result;
}

/// <summary>
/// This is the click handler for the 'Create Geofence' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4::OnCreateGeofence(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        // This must be done here because there is no guarantee of 
        // getting the location consent from a geofence call.
        if (false == permissionsChecked)
        {
            GetGeopositionAsync();
            permissionsChecked = true;
        }

        // get lat/long/radius, the fence name (fenceKey), 
        // and other properties from controls,
        // depending on data in controls for activation time
        // and duration the appropriate
        // constructor will be used.
        Geofence^ geofence = GenerateGeofence();

        // Add the geofence to the GeofenceMonitor's
        // collection of fences
        geofences->InsertAt(0, geofence);

        // add geofence to listbox
        AddGeofenceToRegisteredGeofenceListBox(geofence);
    }
    catch (task_canceled&)
    {
        rootPage->NotifyUser("Canceled", NotifyType::StatusMessage);
    }
    catch (Platform::Exception^ ex)
    {
        // GeofenceMonitor failed in adding a geofence
        // exceptions could be from out of memory, lat/long out of range,
        // too long a name, not a unique name, specifying an activation
        // time + duration that is still in the past
        rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
    }
}

void Scenario4::GetGeopositionAsync()
{
    rootPage->NotifyUser("Checking permissions...", NotifyType::StatusMessage);

    inGetPositionAsync = true;

    task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());
    geopositionTask.then([this](task<Geoposition^> getPosTask)
    {
        try
        {
            // Get will throw an exception if the task was canceled or failed with an error
            Geoposition^ pos = getPosTask.get();

            // clear status
            rootPage->NotifyUser("", NotifyType::StatusMessage);
        }
        catch (task_canceled&)
        {
            rootPage->NotifyUser("Operation canceled", NotifyType::StatusMessage);
        }
        catch (Exception^ ex)
        {
            rootPage->NotifyUser(ex->ToString(), NotifyType::ErrorMessage);
        }
    });

    inGetPositionAsync = false;
}

void Scenario4::FillRegisteredGeofenceListBoxWithExistingGeofences()
{
    for (auto geofence : geofences)
    {
        AddGeofenceToRegisteredGeofenceListBox(geofence);
    }
}

void Scenario4::FillEventListBoxWithExistingEvents()
{
    auto settings = ApplicationData::Current->LocalSettings->Values;
    if (settings->HasKey("ForegroundGeofenceEventCollection"))
    {
        String^ geofenceEvent = safe_cast<String^>(settings->Lookup("ForegroundGeofenceEventCollection"));

        if (0 != geofenceEvent->Length())
        {
            auto events = JsonValue::Parse(geofenceEvent)->GetArray();

            // NOTE: the events are accessed in reverse order
            // because the events were added to JSON from
            // newer to older.  AddEventDescription() adds
            // each new entry to the beginning of the collection.
            for (int pos = events->Size - 1; pos >= 0; pos--)
            {
                auto element = events->GetAt(pos);
                AddEventDescription(element->GetString());
            }
        }

        settings->Remove("ForegroundGeofenceEventCollection");
    }
}

void Scenario4::SaveExistingEvents()
{
    auto jsonArray = ref new JsonArray();

    for (auto eventDescriptor : eventCollection)
    {
        jsonArray->Append(JsonValue::CreateStringValue(eventDescriptor->ToString()));
    }

    String^ jsonString = jsonArray->Stringify();

    auto settings = ApplicationData::Current->LocalSettings->Values;
    settings->Insert("ForegroundGeofenceEventCollection", jsonString);
}

void Scenario4::AddEventDescription(Platform::String^ eventDescription)
{
    if (eventCollection->Size == maxEventDescriptors)
    {
        eventCollection->RemoveAtEnd();
    }

    eventCollection->InsertAt(0, eventDescription);
}
