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
#include "Scenario1_TrackPosition.h"
#include "Scenario1_TrackPosition.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_TrackPosition::Scenario1_TrackPosition()
    {
        InitializeComponent();
    }

    void Scenario1_TrackPosition::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (m_geolocator != nullptr)
        {
            m_geolocator.PositionChanged(m_positionChangedToken);
            m_geolocator.StatusChanged(m_statusChangedToken);
            m_geolocator = nullptr;
        }
    }

    fire_and_forget Scenario1_TrackPosition::StartTracking(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Request permission to access location
        auto accessStatus = co_await Geolocator::RequestAccessAsync();

        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            // You should set MovementThreshold for distance-based tracking
            // or ReportInterval for periodic-based tracking before adding event
            // handlers. If none is set, a ReportInterval of 1 second is used
            // as a default and a position will be returned every 1 second.
            //
            // Value of 2000 milliseconds (2 seconds)
            // isn't a requirement, it is just an example.
            m_geolocator = Geolocator();
            m_geolocator.ReportInterval(2000);

            // Subscribe to PositionChanged event to get updated tracking positions
            m_positionChangedToken = m_geolocator.PositionChanged({ get_weak(), &Scenario1_TrackPosition::OnPositionChanged });

            // Subscribe to StatusChanged event to get updates of location status changes
            m_statusChangedToken = m_geolocator.StatusChanged({ get_weak(), &Scenario1_TrackPosition::OnStatusChanged });

            m_rootPage.NotifyUser(L"Waiting for update...", NotifyType::StatusMessage);
            LocationDisabledMessage().Visibility(Visibility::Collapsed);
            StartTrackingButton().IsEnabled(false);
            StopTrackingButton().IsEnabled(true);
            break;

        case GeolocationAccessStatus::Denied:
            m_rootPage.NotifyUser(L"Access to location is denied.", NotifyType::ErrorMessage);
            LocationDisabledMessage().Visibility(Visibility::Visible);
            break;

        case GeolocationAccessStatus::Unspecified:
            m_rootPage.NotifyUser(L"Unspecificed error!", NotifyType::ErrorMessage);
            LocationDisabledMessage().Visibility(Visibility::Collapsed);
            break;
        }
    }

    void Scenario1_TrackPosition::StopTracking(IInspectable const&, RoutedEventArgs const&)
    {
        m_geolocator.PositionChanged(m_positionChangedToken);
        m_geolocator.StatusChanged(m_statusChangedToken);
        m_geolocator = nullptr;

        StartTrackingButton().IsEnabled(true);
        StopTrackingButton().IsEnabled(false);

        // Clear status
        m_rootPage.NotifyUser(L"", NotifyType::StatusMessage);
    }

    // Event handler for PositionChanged events. It is raised when
    // a location is available for the tracking session specified.
    fire_and_forget Scenario1_TrackPosition::OnPositionChanged(Geolocator const&, PositionChangedEventArgs e)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());
        m_rootPage.NotifyUser(L"Location updated.", NotifyType::StatusMessage);
        UpdateLocationData(e.Position());
    }

    // Event handler for StatusChanged events. It is raised when the
    // location status in the system changes.
    fire_and_forget Scenario1_TrackPosition::OnStatusChanged(Geolocator const&, StatusChangedEventArgs e)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Show the location setting message only if status is disabled.
        LocationDisabledMessage().Visibility(Visibility::Collapsed);

        switch (e.Status())
        {
        case PositionStatus::Ready:
            // Location platform is providing valid data.
            ScenarioOutput_Status().Text(L"Ready");
            m_rootPage.NotifyUser(L"Location platform is ready.", NotifyType::StatusMessage);
            break;

        case PositionStatus::Initializing:
            // Location platform is attempting to acquire a fix.
            ScenarioOutput_Status().Text(L"Initializing");
            m_rootPage.NotifyUser(L"Location platform is attempting to obtain a position.", NotifyType::StatusMessage);
            break;

        case PositionStatus::NoData:
            // Location platform could not obtain location data.
            ScenarioOutput_Status().Text(L"No data");
            m_rootPage.NotifyUser(L"Not able to determine the location.", NotifyType::ErrorMessage);
            break;

        case PositionStatus::Disabled:
            // The permission to access location data is denied by the user or other policies.
            ScenarioOutput_Status().Text(L"Disabled");
            m_rootPage.NotifyUser(L"Access to location is denied.", NotifyType::ErrorMessage);

            // Show message to the user to go to location settings
            LocationDisabledMessage().Visibility(Visibility::Visible);

            // Clear cached location data if any
            UpdateLocationData(nullptr);
            break;

        case PositionStatus::NotInitialized:
            // The location platform is not initialized. This indicates that the application
            // has not made a request for location data.
            ScenarioOutput_Status().Text(L"Not initialized");
            m_rootPage.NotifyUser(L"No request for location is made yet.", NotifyType::StatusMessage);
            break;

        case PositionStatus::NotAvailable:
            // The location platform is not available on this version of the OS.
            ScenarioOutput_Status().Text(L"Not available");
            m_rootPage.NotifyUser(L"Location is not available on this version of the OS.", NotifyType::ErrorMessage);
            break;

        default:
            ScenarioOutput_Status().Text(L"Unknown");
            m_rootPage.NotifyUser(L"", NotifyType::StatusMessage);
            break;
        }
    }

    // Updates the user interface with the Geoposition data provided
    void Scenario1_TrackPosition::UpdateLocationData(Geoposition const& position)
    {
        if (position == nullptr)
        {
            ScenarioOutput_Latitude().Text(L"No data");
            ScenarioOutput_Longitude().Text(L"No data");
            ScenarioOutput_Accuracy().Text(L"No data");
        }
        else
        {
            ScenarioOutput_Latitude().Text(to_hstring(position.Coordinate().Point().Position().Latitude));
            ScenarioOutput_Longitude().Text(to_hstring(position.Coordinate().Point().Position().Longitude));
            ScenarioOutput_Accuracy().Text(to_hstring(position.Coordinate().Accuracy()));
        }
    }
}
