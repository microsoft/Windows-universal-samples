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
#include "Scenario4_ForegroundGeofence.h"
#include "Scenario4_ForegroundGeofence.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Devices::Geolocation::Geofencing;
using namespace winrt::Windows::Globalization;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace
{
    template<typename T, typename U>
    void RemoveByValue(T&& collection, U&& value)
    {
        uint32_t index = 0;
        if (collection.IndexOf(value, index))
        {
            collection.RemoveAt(index);
        }
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario4_ForegroundGeofence::Scenario4_ForegroundGeofence()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario4_ForegroundGeofence::OnNavigatedTo(NavigationEventArgs const&)
    {
        auto lifetime = get_strong();

        m_visibilityChangedToken = Window::Current().VisibilityChanged({ get_weak(), &Scenario4_ForegroundGeofence::OnVisibilityChanged });

        SetStartDateTimeToUI(clock::now() + std::chrono::hours(1));

        // Get permission to use location
        GeolocationAccessStatus accessStatus = co_await Geolocator::RequestAccessAsync();
        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            // Wrap in try/catch in case user revokes access suddenly.
            try
            {
                m_geofenceMonitor = GeofenceMonitor::Current();
            }
            catch (hresult_access_denied const&)
            {
                m_rootPage.NotifyUser(L"Access denied.", NotifyType::ErrorMessage);
            }
            if (m_geofenceMonitor != nullptr)
            {
                for (Geofence geofence : m_geofenceMonitor.Geofences())
                {
                    AddGeofenceToRegisteredGeofenceListBox(geofence);
                }

                // register for state change events
                m_geofenceStateChangedToken = m_geofenceMonitor.GeofenceStateChanged({ get_weak(), &Scenario4_ForegroundGeofence::OnGeofenceStateChanged });
                m_geofenceStatusChangedToken = m_geofenceMonitor.StatusChanged({ get_weak(), &Scenario4_ForegroundGeofence::OnGeofenceStatusChanged });
            }
            break;

        case GeolocationAccessStatus::Denied:
            m_rootPage.NotifyUser(L"Access denied.", NotifyType::ErrorMessage);
            break;

        case GeolocationAccessStatus::Unspecified:
            m_rootPage.NotifyUser(L"Unspecified error.", NotifyType::ErrorMessage);
            break;
        }
    }

    void Scenario4_ForegroundGeofence::OnNavigatedFrom(NavigationEventArgs const&)
    {
        Window::Current().VisibilityChanged(m_visibilityChangedToken);

        if (m_pendingOperation != nullptr)
        {
            m_pendingOperation.Cancel();
            m_pendingOperation = nullptr;
        }
        if (m_geofenceStateChangedToken)
        {
            m_geofenceMonitor.GeofenceStateChanged(std::exchange(m_geofenceStateChangedToken, {}));
            m_geofenceMonitor.StatusChanged(std::exchange(m_geofenceStatusChangedToken, {}));
        }
    }

    TimeSpan LocalTimeOfDay(DateTime dateTime)
    {
        // Convert from UTC to local, then calculate remainder.
        // This takes advantage of the fact that FILETIME begins at midnight.
        FILETIME utc = clock::to_FILETIME(dateTime);
        FILETIME local;
        FileTimeToLocalFileTime(&utc, &local);
        return TimeSpan(file_time(local).value % TimeSpan(std::chrono::hours(24)).count());
    }

    void Scenario4_ForegroundGeofence::SetStartDateTimeToUI(DateTime dateTime)
    {
        TimeSpan timeOfDay = LocalTimeOfDay(dateTime);
        DateTime startOfDay = dateTime - timeOfDay;
        StartDate().Date(startOfDay);
        StartTime().Time(timeOfDay);
    }

    void Scenario4_ForegroundGeofence::OnVisibilityChanged(IInspectable const&, VisibilityChangedEventArgs const& e)
    {
        // NOTE: After the app is no longer visible on the screen and before the app is suspended
        // you might want your app to use toast notification for any geofence activity.
        // By registering for VisibiltyChanged the app is notified when the app is no longer visible in the foreground.

        if (m_geofenceMonitor != nullptr)
        {
            if (e.Visible())
            {
                // register for foreground events
                m_geofenceStateChangedToken = m_geofenceMonitor.GeofenceStateChanged({ get_weak(), &Scenario4_ForegroundGeofence::OnGeofenceStateChanged });
                m_geofenceStatusChangedToken = m_geofenceMonitor.StatusChanged({ get_weak(), &Scenario4_ForegroundGeofence::OnGeofenceStatusChanged });
            }
            else
            {
                // unregister foreground events (see Scenario 5 to capture events in the background)
                m_geofenceMonitor.GeofenceStateChanged(std::exchange(m_geofenceStateChangedToken, {}));
                m_geofenceMonitor.StatusChanged(std::exchange(m_geofenceStatusChangedToken, {}));
            }
        }
    }

    fire_and_forget Scenario4_ForegroundGeofence::OnGeofenceStatusChanged(GeofenceMonitor const& sender, IInspectable const&)
    {
        auto lifetime = get_strong();

        GeofenceMonitorStatus status = sender.Status();

        hstring eventDescription = L"Geofence Status Changed " + FormatDateTime(clock::now(), L"shorttime") + L" (" + to_hstring(status) + L")";

        co_await resume_foreground(Dispatcher());
        AddEventDescription(eventDescription);
    }

    fire_and_forget Scenario4_ForegroundGeofence::OnGeofenceStateChanged(GeofenceMonitor const& sender, IInspectable const&)
    {
        auto lifetime = get_strong();

        auto reports = sender.ReadReports();

        co_await resume_foreground(Dispatcher());

        for (GeofenceStateChangeReport report : reports)
        {
            GeofenceState state = report.NewState();
            Geofence geofence = report.Geofence();
            hstring eventDescription = geofence.Id() + L" " + FormatDateTime(clock::now(), L"shortime") + L" (" + to_hstring(state);

            if (state == GeofenceState::Removed)
            {
                eventDescription = eventDescription + L"/" + to_hstring(report.RemovalReason()) + L")";
                AddEventDescription(eventDescription);

                // Remove the geofence from the monitor.
                RemoveByValue(m_geofenceMonitor.Geofences(), geofence);

                // Remove the geofence from the list box.
                auto items = RegisteredGeofenceListBox().Items();
                for (IInspectable item : items)
                {
                    if (item.as<ListBoxItem>().Tag() == geofence)
                    {
                        RemoveByValue(items, item);
                        break;
                    }
                }
            }
            else if (state == GeofenceState::Entered || state == GeofenceState::Exited)
            {
                // NOTE: You might want to write your app to take particular
                // action based on whether the app has internet connectivity.

                eventDescription = eventDescription + L")";
                AddEventDescription(eventDescription);
            }
        }
    }

    void Scenario4_ForegroundGeofence::OnRemoveGeofenceItem(IInspectable const&, RoutedEventArgs const&)
    {
        auto item = RegisteredGeofenceListBox().SelectedItem().try_as<ListBoxItem>();
        if (item != nullptr)
        {
            Geofence geofence = item.Tag().as<Geofence>();

            // remove the geofence from the monitor
            RemoveByValue(m_geofenceMonitor.Geofences(), geofence);

            // and remove from the UI
            RemoveByValue(RegisteredGeofenceListBox().Items(), item);
        }
    }

    Geofence Scenario4_ForegroundGeofence::GenerateGeofence()
    {
        m_rootPage.NotifyUser(L"", NotifyType::StatusMessage);

        hstring fenceKey = Id().Text();
        if (fenceKey.empty())
        {
            m_rootPage.NotifyUser(L"Name must be between 1 and 64 characters.", NotifyType::ErrorMessage);
            return nullptr;
        }

        BasicGeoposition position{};
        double radius = 0.0;
        if (!ParseDoubleFromTextBox(Longitude(), L"Longitude", -180, 180, position.Longitude) ||
            !ParseDoubleFromTextBox(Latitude(), L"Latitude", -90, 90, position.Latitude) ||
            !ParseDoubleFromTextBox(Radius(), L"Radius", .1, 10018754.3, radius))
        {
            return nullptr;
        }

        position.Altitude = 0.0;

        // the geofence is a circular region
        Geocircle geocircle(position, radius);

        bool singleUse = SingleUse().IsChecked().Value();

        // want to listen for enter geofence, exit geofence and remove geofence events
        // you can select a subset of these event states
        MonitoredGeofenceStates mask = MonitoredGeofenceStates::Entered | MonitoredGeofenceStates::Exited | MonitoredGeofenceStates::Removed;

        // Dwell time is how long you need to be in geofence to have been considered to have Entered it.
        TimeSpan dwellTime;
        if (!ParseTimeSpan(DwellTime().Text(), defaultDwellTime, dwellTime))
        {
            m_rootPage.NotifyUser(L"Invalid Dwell Time.", NotifyType::ErrorMessage);
            return nullptr;
        }

        // The Duration is the length of time the geofence is active.
        // Zero means "infinite duration".
        TimeSpan duration;
        if (!ParseTimeSpan(Duration().Text(), TimeSpan::zero(), duration))
        {
            m_rootPage.NotifyUser(L"Invalid Duration.", NotifyType::ErrorMessage);
            return nullptr;
        }

        // setting up the start time of the geofence
        DateTime startTime;
        if (StartImmediately().IsChecked().Value())
        {
            // The special "start immediately" value can be used if the duration is zero.
            // Otherwise, we get the current time as the start time.
            startTime = (duration == TimeSpan::zero()) ? startImmediately : clock::now();
        }
        else
        {
            startTime = StartDate().Date() + StartTime().Time();
        }

        // Let the platform detect other invalid parameter combinations.
        try
        {
            return Geofence(fenceKey, geocircle, mask, singleUse, dwellTime, startTime, duration);
        }
        catch (...)
        {
            m_rootPage.NotifyUser(to_message(), NotifyType::ErrorMessage);
            return nullptr;
        }
    }

    void Scenario4_ForegroundGeofence::OnRegisteredGeofenceListBoxSelectionChanged(IInspectable const&, SelectionChangedEventArgs const& e)
    {
        if (0 == e.AddedItems().Size())
        {
            // disable the remove button
            RemoveGeofenceItem().IsEnabled(false);
        }
        else
        {
            // enable the remove button
            RemoveGeofenceItem().IsEnabled(true);

            // update controls with the values from the newly-selected geofence
            Geofence geofence = RegisteredGeofenceListBox().SelectedItem().as<ListBoxItem>().Tag().as<Geofence>();
            RefreshControlsFromGeofence(geofence);
        }
    }

    void Scenario4_ForegroundGeofence::RefreshControlsFromGeofence(Geofence const& geofence)
    {
        Id().Text(geofence.Id());

        Geocircle circle = geofence.Geoshape().as<Geocircle>();

        BasicGeoposition center = circle.Center();
        Latitude().Text(to_hstring(center.Latitude));
        Longitude().Text(to_hstring(center.Longitude));
        Radius().Text(to_hstring(circle.Radius()));
        SingleUse().IsChecked(geofence.SingleUse());;

        TimeSpan dwellTime = geofence.DwellTime();
        if (dwellTime != TimeSpan{ 0 })
        {
            DwellTime().Text(to_hstring(dwellTime));
        }
        else
        {
            DwellTime().Text(L"");
        }

        TimeSpan duration = geofence.Duration();
        if (duration != TimeSpan{ 0 })
        {
            Duration().Text(to_hstring(duration));
        }
        else
        {
            Duration().Text(L"");
        }

        DateTime startTime = geofence.StartTime();
        if (startTime == startImmediately)
        {
            StartImmediately().IsChecked(true);
        }
        else
        {
            StartAtSpecificTime().IsChecked(true);
            SetStartDateTimeToUI(startTime);
        }
    }

    bool Scenario4_ForegroundGeofence::ParseDoubleFromTextBox(TextBox const& textBox, hstring const& name, double minValue, double maxValue, double& value)
    {
        if (!SDKTemplate::TryParseDouble(textBox.Text().c_str(), value) || (value < minValue) || (value > maxValue))
        {
            m_rootPage.NotifyUser(name + L" must be between " + to_hstring(minValue) + L" and " + to_hstring(maxValue), NotifyType::StatusMessage);
            return false;
        }
        return true;
    }

    // add geofence to listbox
    void Scenario4_ForegroundGeofence::AddGeofenceToRegisteredGeofenceListBox(Geofence const& geofence)
    {
        Geocircle circle = geofence.Geoshape().as<Geocircle>();

        BasicGeoposition center = circle.Center();
        std::wostringstream stream;
        stream << std::wstring_view{ geofence.Id() } << L" (" << center.Latitude << L", " << center.Longitude << L", " << circle.Radius() << L")";
        ListBoxItem item;
        item.Content(box_value(stream.str()));
        item.Tag(geofence);
        RegisteredGeofenceListBox().Items().InsertAt(0, item);
    }

    fire_and_forget Scenario4_ForegroundGeofence::OnSetPositionToHere(IInspectable const&, RoutedEventArgs const&)
    {
        SetPositionProgressBar().Visibility(Visibility::Visible);
        SetPositionToHereButton().IsEnabled(false);
        Latitude().IsEnabled(false);
        Longitude().IsEnabled(false);

        try
        {
            // Carry out the operation
            // geolocator and location permissions are initialized and checked on page creation.
            Geolocator geolocator;

            // Request a high accuracy position for better accuracy locating the geofence
            geolocator.DesiredAccuracy(PositionAccuracy::High);

            m_pendingOperation = geolocator.GetGeopositionAsync();
            Geoposition pos = co_await m_pendingOperation;

            BasicGeoposition basicPosition = pos.Coordinate().Point().Position();
            Latitude().Text(to_hstring(basicPosition.Latitude));
            Longitude().Text(to_hstring(basicPosition.Longitude));

            // clear status
            m_rootPage.NotifyUser(L"", NotifyType::StatusMessage);
        }
        catch (hresult_canceled const&)
        {
            m_rootPage.NotifyUser(L"Task canceled", NotifyType::StatusMessage);
        }
        catch (...)
        {
            m_rootPage.NotifyUser(to_message(), NotifyType::ErrorMessage);
        }
        m_pendingOperation = nullptr;

        SetPositionProgressBar().Visibility(Visibility::Collapsed);
        SetPositionToHereButton().IsEnabled(true);
        Latitude().IsEnabled(true);
        Longitude().IsEnabled(true);
    }

    void Scenario4_ForegroundGeofence::OnCreateGeofence(IInspectable const&, RoutedEventArgs const&)
    {
        Geofence geofence = GenerateGeofence();
        if (geofence != nullptr)
        {
            try
            {
                // add geofence to GeofenceMonitor
                m_geofenceMonitor.Geofences().Append(geofence);
            }
            catch (...)
            {
                // GeofenceMonitor failed to add the geofence. Possible reasons include
                // duplicate or invalid name, invalid position, having a limited-duration
                // geofence that doesn't leave enough time for the DwellTime to trigger the geofence.
                m_rootPage.NotifyUser(to_message(), NotifyType::ErrorMessage);
                geofence = nullptr;
            }
        }

        if (geofence != nullptr)
        {
            // Update UI after geofence successfully added to GeofenceMonitor.
            AddGeofenceToRegisteredGeofenceListBox(geofence);
        }
    }

    void Scenario4_ForegroundGeofence::AddEventDescription(hstring const& eventDescription)
    {
        ListBoxItem item;
        item.Content(box_value(eventDescription));
        GeofenceEventsListBox().Items().InsertAt(0, item);
    }

    // TimeSpan format is days:hours:minutes:seconds.
    // Leading zeroes may be omitted, so you can also write "minutes:seconds", for example.
    // As an extreme case, "" is the same as "0:0:0:0".
    bool Scenario4_ForegroundGeofence::ParseTimeSpan(hstring const& field, TimeSpan defaultValue, TimeSpan& value)
    {
        value = TimeSpan(0);
        uint32_t fields[4]{};

        wchar_t const* str = field.c_str();
        uint32_t count;
        for (count = 0; *str && count < 4; count++)
        {
            str = SDKTemplate::TryParseUIntPrefix(str, fields[count]);
            if (!str)
            {
                // Not parseable.
                return false;
            }

            // Must be end of string or colon
            if (*str == L':')
            {
                str++;
            }
            else if (*str != L'\0')
            {
                return false;
            }
        }
        // Should be at end of string now.
        if (*str != L'\0')
        {
            return false;
        }

        std::reverse(&fields[0], &fields[count]);

        value = std::chrono::hours(fields[3] * 24) +
            std::chrono::hours(fields[2]) +
            std::chrono::minutes(fields[1]) +
            std::chrono::seconds(fields[0]);

        if (value.count() == 0)
        {
            value = defaultValue;
        }

        return true;
    }
}
