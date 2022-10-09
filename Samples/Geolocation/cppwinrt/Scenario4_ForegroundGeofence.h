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

#pragma once

#include "Scenario4_ForegroundGeofence.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_ForegroundGeofence : Scenario4_ForegroundGeofenceT<Scenario4_ForegroundGeofence>
    {
        Scenario4_ForegroundGeofence();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        void OnRemoveGeofenceItem(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnRegisteredGeofenceListBoxSelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        fire_and_forget OnSetPositionToHere(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnCreateGeofence(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };

        static constexpr Windows::Foundation::TimeSpan defaultDwellTime = std::chrono::seconds(10);

        // This special DateTime value is used by the Geofence to mean "start immediately".
        static constexpr Windows::Foundation::DateTime startImmediately{};

        Windows::Foundation::IAsyncOperation<Windows::Devices::Geolocation::Geoposition> m_pendingOperation;
        Windows::Devices::Geolocation::Geofencing::GeofenceMonitor m_geofenceMonitor{ nullptr };

        event_token m_visibilityChangedToken{};
        event_token m_geofenceStateChangedToken{};
        event_token m_geofenceStatusChangedToken{};

        void OnVisibilityChanged(Windows::Foundation::IInspectable const&, Windows::UI::Core::VisibilityChangedEventArgs const& e);
        fire_and_forget OnGeofenceStatusChanged(Windows::Devices::Geolocation::Geofencing::GeofenceMonitor const& sender, Windows::Foundation::IInspectable const&);
        fire_and_forget OnGeofenceStateChanged(Windows::Devices::Geolocation::Geofencing::GeofenceMonitor const& sender, Windows::Foundation::IInspectable const&);

        void SetStartDateTimeToUI(Windows::Foundation::DateTime dateTime);
        Windows::Devices::Geolocation::Geofencing::Geofence GenerateGeofence();
        void RefreshControlsFromGeofence(Windows::Devices::Geolocation::Geofencing::Geofence const& geofence);
        bool ParseDoubleFromTextBox(Windows::UI::Xaml::Controls::TextBox const& textBox, hstring const& name, double minValue, double maxValue, double& value);
        void AddGeofenceToRegisteredGeofenceListBox(Windows::Devices::Geolocation::Geofencing::Geofence const& geofence);
        void AddEventDescription(hstring const& eventDescription);
        bool ParseTimeSpan(hstring const& field, Windows::Foundation::TimeSpan defaultValue, Windows::Foundation::TimeSpan& value);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_ForegroundGeofence : Scenario4_ForegroundGeofenceT<Scenario4_ForegroundGeofence, implementation::Scenario4_ForegroundGeofence>
    {
    };
}
