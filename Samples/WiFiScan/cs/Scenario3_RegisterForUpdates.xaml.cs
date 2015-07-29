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
using System.Collections.ObjectModel;

namespace WiFiScan
{
    public sealed partial class Scenario3_RegisterForUpdates : Page
    {
        private WiFiAdapter firstAdapter;

        MainPage rootPage;
        public ObservableCollection<WiFiNetworkDisplay> ResultCollection
        {
            get;
            private set;
        }

        public Scenario3_RegisterForUpdates()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            ResultCollection = new ObservableCollection<WiFiNetworkDisplay>();
            rootPage = MainPage.Current;

            // RequestAccessAsync must have been called at least once by the app before using the API
            // Calling it multiple times is fine but not necessary
            // RequestAccessAsync must be called from the UI thread
            var access = await WiFiAdapter.RequestAccessAsync();
            if (access != WiFiAccessStatus.Allowed)
            {
                rootPage.NotifyUser("Access denied", NotifyType.ErrorMessage);
            }
            else
            {
                DataContext = this;

                var result = await Windows.Devices.Enumeration.DeviceInformation.FindAllAsync(WiFiAdapter.GetDeviceSelector());
                if (result.Count >= 1)
                {
                    firstAdapter = await WiFiAdapter.FromIdAsync(result[0].Id);
                    RegisterButton.IsEnabled = true;
                }
                else
                {
                    rootPage.NotifyUser("No WiFi Adapters detected on this machine", NotifyType.ErrorMessage);
                }
            }
        }

        private void FirstAdapter_AvailableNetworksChanged(WiFiAdapter sender, object args)
        {
            DisplayNetworkReport(sender.NetworkReport, sender);
        }

        private async void DisplayNetworkReport(WiFiNetworkReport report, WiFiAdapter adapter)
        {
            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                rootPage.NotifyUser(string.Format("Network Report Timestamp: {0}", report.Timestamp), NotifyType.StatusMessage);

                ResultCollection.Clear();

                foreach (var network in report.AvailableNetworks)
                {
                    var networkDisplay = new WiFiNetworkDisplay(network, adapter);
                    await networkDisplay.UpdateConnectivityLevel();
                    ResultCollection.Add(networkDisplay);
                }
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
