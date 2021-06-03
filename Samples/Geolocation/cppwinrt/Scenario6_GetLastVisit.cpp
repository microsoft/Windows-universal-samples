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
#include "Scenario6_GetLastVisit.h"
#include "Scenario6_GetLastVisit.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario6_GetLastVisit::Scenario6_GetLastVisit()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario6_GetLastVisit::GetLastVisitButtonClicked(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        GetLastVisitButton().IsEnabled(false);
        LocationDisabledMessage().Visibility(Visibility::Collapsed);

        // Request permission to access location
        auto accessStatus = co_await Geolocator::RequestAccessAsync();

        Geovisit visit = nullptr;
        switch (accessStatus)
        {
        case GeolocationAccessStatus::Allowed:
            m_rootPage.NotifyUser(L"Waiting for update...", NotifyType::StatusMessage);

            // Get the last visit report, if any. This should be a quick operation.
            visit = co_await GeovisitMonitor::GetLastReportAsync();
            m_rootPage.NotifyUser(L"Visit info updated.", NotifyType::StatusMessage);
            break;

        case GeolocationAccessStatus::Denied:
            m_rootPage.NotifyUser(L"Access to location is denied.", NotifyType::ErrorMessage);
            LocationDisabledMessage().Visibility(Visibility::Visible);
            break;

        default:
        case GeolocationAccessStatus::Unspecified:
            m_rootPage.NotifyUser(L"Unspecified error.", NotifyType::ErrorMessage);
            break;
        }

        UpdateLastVisit(visit);

        GetLastVisitButton().IsEnabled(true);
    }

    // Updates the user interface with the Geovisit data provided
    void Scenario6_GetLastVisit::UpdateLastVisit(Geovisit const& visit)
    {
        if (visit == nullptr)
        {
            ScenarioOutput_Latitude().Text(L"No data");
            ScenarioOutput_Longitude().Text(L"No data");
            ScenarioOutput_Accuracy().Text(L"No data");
            ScenarioOutput_Timestamp().Text(L"No data");
            ScenarioOutput_VisitStateChange().Text(L"No data");
        }
        else
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
}
