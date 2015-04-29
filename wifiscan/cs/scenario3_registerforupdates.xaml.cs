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
using Windows.UI.Xaml.Controls;
using Windows.Devices.WiFi;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace WiFiScan
{
    public sealed partial class Scenario3_RegisterForUpdates : Page
    {
        private WiFiAdapter firstAdapter;

        private MainPage rootPage;

        public Scenario3_RegisterForUpdates()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // RequestAccessAsync must have been called at least once by the app before using the API
            // Calling it multiple times is fine but not necessary
            // RequestAccessAsync must be called from the UI thread
            var access = await WiFiAdapter.RequestAccessAsync();
            if (access != WiFiAccessStatus.Allowed)
            {
                ScenarioOutput.Text = "Access denied";
            }
            else
            {
                var result = await Windows.Devices.Enumeration.DeviceInformation.FindAllAsync(WiFiAdapter.GetDeviceSelector());
                if (result.Count >= 1)
                {
                    firstAdapter = await WiFiAdapter.FromIdAsync(result[0].Id);
                    RegisterButton.IsEnabled = true;
                }
                else
                {
                    ScenarioOutput.Text = "No WiFi Adapters detected on this machine";
                }
            }
        }

        private void FirstAdapter_AvailableNetworksChanged(WiFiAdapter sender, object args)
        {
            DisplayNetworkReport(firstAdapter.NetworkReport);
        }

        private void DisplayNetworkReport(WiFiNetworkReport report)
        {
            var message = string.Format("Network Report Timestamp: {0}", report.Timestamp);
            foreach (var network in report.AvailableNetworks)
            {
                message += string.Format("\nNetworkName: {0}, BSSID: {1}, RSSI: {2}dBm, Channel Frequency: {3}kHz",
                    network.Ssid, network.Bssid, network.NetworkRssiInDecibelMilliwatts, network.ChannelCenterFrequencyInKilohertz);
            }

            // there is no guarantee of what thread the AvailableNetworksChanged callback is run on
            rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
                () =>
                {
                    ScenarioOutput.Text = message;
                });
        }

        private void Button_Click_Register(object sender, RoutedEventArgs e)
        {
            firstAdapter.AvailableNetworksChanged += FirstAdapter_AvailableNetworksChanged;
            RegisterButton.IsEnabled = false;
            UnregisterButton.IsEnabled = true;
        }

        private void Button_Click_Unregister(object sender, RoutedEventArgs e)
        {
            firstAdapter.AvailableNetworksChanged -= FirstAdapter_AvailableNetworksChanged;
            UnregisterButton.IsEnabled = false;
            RegisterButton.IsEnabled = true;
        }
    }
}
