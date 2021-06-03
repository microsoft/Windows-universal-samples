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
#include "Scenario7_ForegroundVisits.h"
#include "Scenario7_ForegroundVisits.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario7_ForegroundVisits::Scenario7_ForegroundVisits()
    {
        InitializeComponent();
    }

    void Scenario7_ForegroundVisits::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (m_visitMonitor != nullptr)
        {
            m_visitMonitor.Stop();
            m_visitMonitor.VisitStateChanged(m_visitStateChangedToken);
        }
    }

    fire_and_forget Scenario7_ForegroundVisits::StartMonitoring(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Request permission to access location
        auto accessStatus = co_await Geolocator::RequestAccessAsync();

        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:

            m_visitMonitor = GeovisitMonitor();

            // Add visit state changed event handler.
            m_visitStateChangedToken = m_visitMonitor.VisitStateChanged({ get_weak(), &Scenario7_ForegroundVisits::OnVisitStateChanged });

            // Start monitoring with the desired scope.
            // For higher granularity such as building/venue level changes, choose venue.
            // For lower granularity more or less in the range of zipcode level changes, choose city.
            // Choosing Venue here as an example.
            m_visitMonitor.Start(VisitMonitoringScope::Venue);

            LocationDisabledMessage().Visibility(Visibility::Collapsed);
            m_rootPage.NotifyUser(L"Waiting for update...", NotifyType::StatusMessage);

            StartMonitoringButton().IsEnabled(false);
            StopMonitoringButton().IsEnabled(true);
            break;

        case GeolocationAccessStatus::Denied:
            m_rootPage.NotifyUser(L"Access to location is denied.", NotifyType::ErrorMessage);
            LocationDisabledMessage().Visibility(Visibility::Visible);
            break;

        default:
        case GeolocationAccessStatus::Unspecified:
            m_rootPage.NotifyUser(L"Unspecificed error!", NotifyType::ErrorMessage);
            LocationDisabledMessage().Visibility(Visibility::Collapsed);
            break;
        }
    }

    void Scenario7_ForegroundVisits::StopMonitoring(IInspectable const&, RoutedEventArgs const&)
    {
        // Stop visit monitoring.
        m_visitMonitor.Stop();
        m_visitMonitor.VisitStateChanged(m_visitStateChangedToken);
        m_visitMonitor = nullptr;

        StartMonitoringButton().IsEnabled(true);
        StopMonitoringButton().IsEnabled(false);

        // Clear status
        m_rootPage.NotifyUser(L"", NotifyType::StatusMessage);
    }

    // Event handler for VisitStateChanged events. It is raised when
    // a new visit state change is available based on the monitoring scope specified.
    fire_and_forget Scenario7_ForegroundVisits::OnVisitStateChanged(GeovisitMonitor const&, GeovisitStateChangedEventArgs args)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());

        m_rootPage.NotifyUser(L"Visit state changed.", NotifyType::StatusMessage);
        UpdateVisitData(args.Visit());
    }

    // Updates the user interface with the Geovisit data provided
    void Scenario7_ForegroundVisits::UpdateVisitData(Geovisit const& visit)
    {
        // A valid visit is available, extract the state change and Timestamp.
        ScenarioOutput_VisitStateChange().Text(to_hstring(visit.StateChange()));
        ScenarioOutput_Timestamp().Text(FormatDateTime(visit.Timestamp(), L"shortdate shorttime"));

        // If a valid position is available, extract the position information that caused the state change to happen.
        if (visit.Position() == nullptr)
        {
            ScenarioOutput_Latitude().Text(L"No data");
            ScenarioOutput_Longitude().Text(L"No data");
            ScenarioOutput_Accuracy().Text(L"No data");
        }
        else
        {
            ScenarioOutput_Latitude().Text(to_hstring(visit.Position().Coordinate().Point().Position().Latitude));
            ScenarioOutput_Longitude().Text(to_hstring(visit.Position().Coordinate().Point().Position().Longitude));
            ScenarioOutput_Accuracy().Text(to_hstring(visit.Position().Coordinate().Accuracy()));
        }
    }

}
