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

#include "Scenario1_TrackPosition.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_TrackPosition : Scenario1_TrackPositionT<Scenario1_TrackPosition>
    {
        Scenario1_TrackPosition();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget StartTracking(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void StopTracking(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Devices::Geolocation::Geolocator m_geolocator{ nullptr };
        event_token m_positionChangedToken{};
        event_token m_statusChangedToken{};

        fire_and_forget OnPositionChanged(Windows::Devices::Geolocation::Geolocator const&, Windows::Devices::Geolocation::PositionChangedEventArgs e);
        fire_and_forget OnStatusChanged(Windows::Devices::Geolocation::Geolocator const&, Windows::Devices::Geolocation::StatusChangedEventArgs e);
        void UpdateLocationData(Windows::Devices::Geolocation::Geoposition const& position);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_TrackPosition : Scenario1_TrackPositionT<Scenario1_TrackPosition, implementation::Scenario1_TrackPosition>
    {
    };
}
