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

using System;
using Windows.Networking.NetworkOperators;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_ConfigureMobileHotspot : Page
    {
        NetworkOperatorTetheringManager m_tetheringManager = null;
        bool m_applying = false;

        public Scenario1_ConfigureMobileHotspot()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            m_tetheringManager = Helpers.TryGetCurrentNetworkOperatorTetheringManager();

            if (m_tetheringManager != null)
            {
                InitializeTetheringControls();
                HotspotPanel.Visibility = Visibility.Visible;
            }
        }

        private void AddComboBoxItem(ComboBox comboBox, string name, object value, bool selected)
        {
            var item = new ComboBoxItem { Tag = value, Content = name };
            BandComboBox.Items.Add(item);
            if (selected)
            {
                BandComboBox.SelectedItem = item;
            }
        }

        private void InitializeTetheringControls()
        {
            NetworkOperatorTetheringAccessPointConfiguration configuration = m_tetheringManager.GetCurrentAccessPointConfiguration();

            SsidTextBox.Text = configuration.Ssid;
            PassphraseTextBox.Text = configuration.Passphrase;

            // Fill the Band combo box with supported bands, and select the current one.
            TetheringWiFiBand currentBand = configuration.Band;
            BandComboBox.Items.Clear();
            for (TetheringWiFiBand band = TetheringWiFiBand.Auto; band <= TetheringWiFiBand.SixGigahertz; band++)
            {
                if (Helpers.IsBandSupported(configuration, band))
                {
                    AddComboBoxItem(BandComboBox, Helpers.GetFriendlyName(band), band, band == currentBand);
                }
            }
            if (BandComboBox.SelectedIndex == -1)
            {
                AddComboBoxItem(BandComboBox, Helpers.GetFriendlyName(currentBand), currentBand, true);
            }

            // Fill the Authentication combo box with supported bands, and select the current one.
            TetheringWiFiAuthenticationKind currentAuthenticationKind = configuration.AuthenticationKind;
            AuthenticationComboBox.Items.Clear();
            for (TetheringWiFiAuthenticationKind authenticationKind = TetheringWiFiAuthenticationKind.Wpa2; authenticationKind <= TetheringWiFiAuthenticationKind.Wpa3; authenticationKind++)
            {
                if (Helpers.IsAuthenticationKindSupported(configuration, authenticationKind))
                {
                    AddComboBoxItem(AuthenticationComboBox, Helpers.GetFriendlyName(authenticationKind), authenticationKind, authenticationKind == currentAuthenticationKind);
                }
            }
            if (AuthenticationComboBox.SelectedIndex == -1)
            {
                AddComboBoxItem(AuthenticationComboBox, Helpers.GetFriendlyName(currentAuthenticationKind), currentAuthenticationKind, true);
            }
        }

        private async void ApplyChanges_Click(object sender, RoutedEventArgs e)
        {
            if (m_applying)
            {
                return;

            }
            m_applying = true;

            NetworkOperatorTetheringAccessPointConfiguration configuration = m_tetheringManager.GetCurrentAccessPointConfiguration();

            configuration.Ssid = SsidTextBox.Text;
            configuration.Passphrase = PassphraseTextBox.Text;
            configuration.Band = (TetheringWiFiBand)((ComboBoxItem)BandComboBox.SelectedItem).Tag;
            configuration.AuthenticationKind = (TetheringWiFiAuthenticationKind)((ComboBoxItem)AuthenticationComboBox.SelectedItem).Tag;

            await m_tetheringManager.ConfigureAccessPointAsync(configuration);
            m_applying = false;

            InitializeTetheringControls();
        }

        private void DiscardChanges_Click(object sender, RoutedEventArgs e)
        {
            InitializeTetheringControls();
        }
    }
}
