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

#include "Scenario7_ForegroundVisits.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario7_ForegroundVisits : Scenario7_ForegroundVisitsT<Scenario7_ForegroundVisits>
    {
        Scenario7_ForegroundVisits();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        fire_and_forget StartMonitoring(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void StopMonitoring(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };

        // Provides access to visits
        Windows::Devices::Geolocation::GeovisitMonitor m_visitMonitor{ nullptr };
        event_token m_visitStateChangedToken{};

        fire_and_forget OnVisitStateChanged(Windows::Devices::Geolocation::GeovisitMonitor const&, Windows::Devices::Geolocation::GeovisitStateChangedEventArgs args);
        void UpdateVisitData(Windows::Devices::Geolocation::Geovisit const& visit);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario7_ForegroundVisits : Scenario7_ForegroundVisitsT<Scenario7_ForegroundVisits, implementation::Scenario7_ForegroundVisits>
    {
    };
}
