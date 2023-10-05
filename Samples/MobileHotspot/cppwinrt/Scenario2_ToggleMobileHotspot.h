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

#include "Scenario2_ToggleMobileHotspot.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_ToggleMobileHotspot : Scenario2_ToggleMobileHotspotT<Scenario2_ToggleMobileHotspot>
    {
        Scenario2_ToggleMobileHotspot();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget MobileHotspotToggle_Toggled(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::Networking::NetworkOperators::NetworkOperatorTetheringManager m_tetheringManager{ nullptr };
        bool m_toggling{ false };
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_ToggleMobileHotspot : Scenario2_ToggleMobileHotspotT<Scenario2_ToggleMobileHotspot, implementation::Scenario2_ToggleMobileHotspot>
    {
    };
}
