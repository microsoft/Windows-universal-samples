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
#include "Scenario1_ConfigureMobileHotspot.h"
#include "Scenario1_ConfigureMobileHotspot.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::NetworkOperators;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_ConfigureMobileHotspot::Scenario1_ConfigureMobileHotspot()
    {
        InitializeComponent();
    }

    void Scenario1_ConfigureMobileHotspot::OnNavigatedTo(NavigationEventArgs const&)
    {
        m_tetheringManager = SDKTemplate::TryGetCurrentNetworkOperatorTetheringManager();

        if (m_tetheringManager)
        {
            InitializeTetheringControls();
            HotspotPanel().Visibility(Visibility::Visible);
        }
    }

    void Scenario1_ConfigureMobileHotspot::AddComboBoxItem(ComboBox const& comboBox, hstring name, IInspectable const& tag, bool selected)
    {
        ComboBoxItem item;
        item.Tag(tag);
        item.Content(box_value(name));
        comboBox.Items().Append(item);
        if (selected)
        {
            comboBox.SelectedItem(item);
        }
    }

    void Scenario1_ConfigureMobileHotspot::InitializeTetheringControls()
    {
        NetworkOperatorTetheringAccessPointConfiguration configuration = m_tetheringManager.GetCurrentAccessPointConfiguration();

        SsidTextBox().Text(configuration.Ssid());
        PassphraseTextBox().Text(configuration.Passphrase());

        // Fill the Band combo box with supported bands, and select the current one.
        TetheringWiFiBand currentBand = configuration.Band();
        BandComboBox().Items().Clear();
        for (TetheringWiFiBand band = TetheringWiFiBand::Auto; band <= TetheringWiFiBand::FiveGigahertz; band = (TetheringWiFiBand)((int)band + 1))
        {
            if (SDKTemplate::IsBandSupported(configuration, band))
            {
                AddComboBoxItem(BandComboBox(), band, band == currentBand);
            }
        }
        if (BandComboBox().SelectedIndex() == -1)
        {
            AddComboBoxItem(BandComboBox(), currentBand, true);
        }
    }

    fire_and_forget Scenario1_ConfigureMobileHotspot::ApplyChanges_Click(IInspectable const&, RoutedEventArgs const& e)
    {
        if (m_applying)
        {
            co_return;

        }
        m_applying = true;

        NetworkOperatorTetheringAccessPointConfiguration configuration = m_tetheringManager.GetCurrentAccessPointConfiguration();

        configuration.Ssid(SsidTextBox().Text());
        configuration.Passphrase(PassphraseTextBox().Text());
        configuration.Band(BandComboBox().SelectedItem().as<ComboBoxItem>().Tag().as<TetheringWiFiBand>());

        co_await m_tetheringManager.ConfigureAccessPointAsync(configuration);
        m_applying = false;

        InitializeTetheringControls();
    }

    void Scenario1_ConfigureMobileHotspot::DiscardChanges_Click(IInspectable const&, RoutedEventArgs const&)
    {
        InitializeTetheringControls();
    }
}
