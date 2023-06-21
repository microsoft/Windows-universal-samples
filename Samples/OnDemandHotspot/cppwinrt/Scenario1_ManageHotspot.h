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

#include "Scenario1_ManageHotspot.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_ManageHotspot : Scenario1_ManageHotspotT<Scenario1_ManageHotspot>
    {
        Scenario1_ManageHotspot();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void RegisterTasks_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void UnregisterTasks_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void UpdateNowButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void UpdateOnDemandButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);        

    private:
        SDKTemplate::MainPage rootPage{ SDKTemplate::implementation::MainPage::Current() };
        winrt::Windows::Devices::WiFi::WiFiOnDemandHotspotNetwork m_network{ nullptr };

        bool SavePropertiesForBackgroundTask();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_ManageHotspot : Scenario1_ManageHotspotT<Scenario1_ManageHotspot, implementation::Scenario1_ManageHotspot>
    {
    };
}
