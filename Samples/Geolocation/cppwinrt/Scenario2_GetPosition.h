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

#include "Scenario2_GetPosition.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_GetPosition : Scenario2_GetPositionT<Scenario2_GetPosition>
    {
        Scenario2_GetPosition();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget GetGeolocationButtonClicked(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void CancelGetGeolocationButtonClicked(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);


    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Foundation::IAsyncOperation<Windows::Devices::Geolocation::Geoposition> m_pendingOperation;

        void UpdateLocationData(Windows::Devices::Geolocation::Geoposition const& position);
        void ShowSatelliteData(bool isVisible);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_GetPosition : Scenario2_GetPositionT<Scenario2_GetPosition, implementation::Scenario2_GetPosition>
    {
    };
}
