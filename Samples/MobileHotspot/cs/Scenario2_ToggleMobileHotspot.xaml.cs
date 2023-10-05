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
    public sealed partial class Scenario2_ToggleMobileHotspot : Page
    {
        NetworkOperatorTetheringManager m_tetheringManager = null;
        bool m_toggling = false;

        public Scenario2_ToggleMobileHotspot()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            MainPage.Current.NotifyUser("", NotifyType.StatusMessage);

            m_tetheringManager = Helpers.TryGetCurrentNetworkOperatorTetheringManager();

            if (m_tetheringManager != null)
            {
                NetworkOperatorTetheringAccessPointConfiguration configuration = m_tetheringManager.GetCurrentAccessPointConfiguration();
                SsidRun.Text = configuration.Ssid;
                PasswordRun.Text = configuration.Passphrase;
                BandRun.Text = Helpers.GetFriendlyName(configuration.Band);

                m_toggling = true;
                bool isTethered = m_tetheringManager.TetheringOperationalState == TetheringOperationalState.On;
                MobileHotspotToggle.IsOn = isTethered;
                m_toggling = false;

                HotspotPanel.Visibility = Visibility.Visible;
            }
        }

        private async void MobileHotspotToggle_Toggled(object sender, RoutedEventArgs e)
        {
            if (m_toggling)
            {
                return;
            }
            MainPage.Current.NotifyUser("", NotifyType.StatusMessage);

            bool isTethered = m_tetheringManager.TetheringOperationalState == TetheringOperationalState.On;
            if (MobileHotspotToggle.IsOn == isTethered)
            {
                // Already in desired state.
                return;
            }

            // Switch to opposite state.
            m_toggling = true;

            NetworkOperatorTetheringOperationResult result;
            if (isTethered)
            {
                result = await m_tetheringManager.StopTetheringAsync();
            }
            else
            {
                result = await m_tetheringManager.StartTetheringAsync();
            }

            if (result.Status == TetheringOperationStatus.Success)
            {
                // Change to new state.
                MobileHotspotToggle.IsOn = !isTethered;
            }
            else
            {
                // Return to old state.
                MobileHotspotToggle.IsOn = isTethered;

                switch (result.Status)
                {
                    case TetheringOperationStatus.WiFiDeviceOff:
                        MainPage.Current.NotifyUser(
                            "Wi-Fi adapter is either turned off or missing.",
                            NotifyType.ErrorMessage);
                        break;

                    default:
                        MainPage.Current.NotifyUser(
                            $"Unhandled result while toggling Mobile Hotspot: {result.Status}",
                            NotifyType.ErrorMessage);
                        break;
                }
            }
            m_toggling = false;
        }
    }
}
