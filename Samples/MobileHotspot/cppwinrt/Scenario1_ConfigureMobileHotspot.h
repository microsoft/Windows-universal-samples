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

#include "Scenario1_ConfigureMobileHotspot.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_ConfigureMobileHotspot : Scenario1_ConfigureMobileHotspotT<Scenario1_ConfigureMobileHotspot>
    {
        Scenario1_ConfigureMobileHotspot();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        fire_and_forget ApplyChanges_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DiscardChanges_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::Networking::NetworkOperators::NetworkOperatorTetheringManager m_tetheringManager{ nullptr };
        bool m_applying{ false };

        void InitializeTetheringControls();

        void AddComboBoxItem(Windows::UI::Xaml::Controls::ComboBox const& comboBox, hstring name, Windows::Foundation::IInspectable const& tag, bool selected);

        template<typename T>
        void AddComboBoxItem(Windows::UI::Xaml::Controls::ComboBox const& comboBox, T value, bool selected)
        {
            AddComboBoxItem(comboBox, SDKTemplate::GetFriendlyName(value), box_value(value), selected);
        }
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_ConfigureMobileHotspot : Scenario1_ConfigureMobileHotspotT<Scenario1_ConfigureMobileHotspot, implementation::Scenario1_ConfigureMobileHotspot>
    {
    };
}
