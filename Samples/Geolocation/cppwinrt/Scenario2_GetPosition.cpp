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
#include "Scenario2_GetPosition.h"
#include "Scenario2_GetPosition.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace winrt::Windows::Devices::Geolocation;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_GetPosition::Scenario2_GetPosition()
    {
        InitializeComponent();
    }

    void Scenario2_GetPosition::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (m_pendingOperation)
        {
            m_pendingOperation.Cancel();
            m_pendingOperation = nullptr;
        }
    }

    fire_and_forget Scenario2_GetPosition::GetGeolocationButtonClicked(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        GetGeolocationButton().IsEnabled(false);
        CancelGetGeolocationButton().IsEnabled(true);
        LocationDisabledMessage().Visibility(Visibility::Collapsed);

        try
        {
            // Request permission to access location
            auto accessStatus = co_await Geolocator::RequestAccessAsync();

            switch (accessStatus)
            {
            case GeolocationAccessStatus::Allowed:
            {
                m_rootPage.NotifyUser(L"Waiting for update...", NotifyType::StatusMessage);

                // If DesiredAccuracy or DesiredAccuracyInMeters are not set (or value is 0), DesiredAccuracy.Default is used.
                Geolocator geolocator;
                uint32_t desiredAccuracyInMetersValue;
                if (TryParseUInt(DesiredAccuracyInMeters().Text().c_str(), desiredAccuracyInMetersValue))
                {
                    geolocator.DesiredAccuracyInMeters(desiredAccuracyInMetersValue);
                }

                // Carry out the operation
                m_pendingOperation = geolocator.GetGeopositionAsync();
                Geoposition pos = co_await m_pendingOperation;

                UpdateLocationData(pos);
                m_rootPage.NotifyUser(L"Location updated.", NotifyType::StatusMessage);
            }
                break;

            case GeolocationAccessStatus::Denied:
                m_rootPage.NotifyUser(L"Access to location is denied.", NotifyType::ErrorMessage);
                LocationDisabledMessage().Visibility(Visibility::Visible);
                UpdateLocationData(nullptr);
                break;

            case GeolocationAccessStatus::Unspecified:
                m_rootPage.NotifyUser(L"Unspecified error.", NotifyType::ErrorMessage);
                UpdateLocationData(nullptr);
                break;
            }
        }
        catch (hresult_canceled const&)
        {
            m_rootPage.NotifyUser(L"Canceled.", NotifyType::StatusMessage);
        }
        catch (...)
        {
            m_rootPage.NotifyUser(to_message(), NotifyType::ErrorMessage);
        }

        m_pendingOperation = nullptr;
        GetGeolocationButton().IsEnabled(true);
        CancelGetGeolocationButton().IsEnabled(false);
    }

    void Scenario2_GetPosition::CancelGetGeolocationButtonClicked(IInspectable const&, RoutedEventArgs const&)
    {
        if (m_pendingOperation)
        {
            m_pendingOperation.Cancel();
            m_pendingOperation = nullptr;
        }
        GetGeolocationButton().IsEnabled(true);
        CancelGetGeolocationButton().IsEnabled(false);
    }

    // Updates the user interface with the Geoposition data provided
    void Scenario2_GetPosition::UpdateLocationData(Geoposition const& position)
    {
        if (position == nullptr)
        {
            ScenarioOutput_Latitude().Text(L"No data");
            ScenarioOutput_Longitude().Text(L"No data");
            ScenarioOutput_Accuracy().Text(L"No data");
            ScenarioOutput_Source().Text(L"No data");
            ShowSatelliteData(false);
        }
        else
        {
            ScenarioOutput_Latitude().Text(to_hstring(position.Coordinate().Point().Position().Latitude));
            ScenarioOutput_Longitude().Text(to_hstring(position.Coordinate().Point().Position().Longitude));
            ScenarioOutput_Accuracy().Text(to_hstring(position.Coordinate().Accuracy()));
            ScenarioOutput_Source().Text(to_hstring(position.Coordinate().PositionSource()));

            if (position.Coordinate().PositionSource() == PositionSource::Satellite)
            {
                // Show labels and satellite data when available
                GeocoordinateSatelliteData satelliteData = position.Coordinate().SatelliteData();
                ScenarioOutput_PosPrecision().Text(to_hstring(satelliteData.PositionDilutionOfPrecision()));
                ScenarioOutput_HorzPrecision().Text(to_hstring(satelliteData.HorizontalDilutionOfPrecision()));
                ScenarioOutput_VertPrecision().Text(to_hstring(satelliteData.VerticalDilutionOfPrecision()));;
                ShowSatelliteData(true);
            }
            else
            {
                // Hide labels and satellite data
                ShowSatelliteData(false);
            }
        }
    }

    void Scenario2_GetPosition::ShowSatelliteData(bool isVisible)
    {
        auto visibility = isVisible ? Visibility::Visible : Visibility::Collapsed;
        ScenarioOutput_PosPrecision().Visibility(visibility);
        ScenarioOutput_HorzPrecision().Visibility(visibility);
        ScenarioOutput_VertPrecision().Visibility(visibility);
    }

}
