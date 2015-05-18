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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace WiFiScan
{
    public sealed partial class Scenario2_Scan : Page
    {
        private WiFiAdapter firstAdapter;

        public Scenario2_Scan()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
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

                    var button = new Button();
                    button.Content = string.Format("Scan");
                    button.Click += Button_Click;
                    Buttons.Children.Add(button);
                }
                else
                {
                    ScenarioOutput.Text = "No WiFi Adapters detected on this machine";
                }
            }
        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            await firstAdapter.ScanAsync();
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
            ScenarioOutput.Text = message;
        }
    }
}
