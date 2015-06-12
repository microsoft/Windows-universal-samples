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
using System.Collections.Generic;
using Windows.Devices.WiFiDirect.Services;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// UI that provides a way to discover any Wi-Fi Direct service by name
        /// A real application would likely hard-code the service name to a well-known service rather than expose this in the UI
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.cs and WiFiDirectServicesWrappers.cs
        /// </summary>
        public sealed partial class Scenario3 : Page
        {
            private Object thisLock = new Object();

            private MainPage rootPage;

            private WiFiDirectServiceProvisioningInfo provisioningInfo;

            public Scenario3()
            {
                this.InitializeComponent();
            }

            protected override void OnNavigatedTo(NavigationEventArgs e)
            {
                lock (thisLock)
                {
                    rootPage = MainPage.Current;

                    // Cleanup past discoveries
                    DiscoveredServices.Items.Clear();
                    provisioningInfo = null;

                    UpdateDiscoveryList(WiFiDirectServiceManager.Instance.DiscoveredDevices);
                }
                WiFiDirectServiceManager.Instance.Scenario3 = this;
            }

            protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
            {
                WiFiDirectServiceManager.Instance.Scenario3 = null;
            }

            public async void UpdateDiscoveryList(IList<DiscoveredDeviceWrapper> devices)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    lock (thisLock)
                    {
                        DiscoveredServices.Items.Clear();
                        foreach (var device in devices)
                        {
                            DiscoveredServices.Items.Add(device);
                        }
                    }
                });
            }

            public async void SessionConnected()
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    // Force navigation to the next page for smoother experience
                    rootPage.GoToScenario(3);
                });
            }

            private void Discover_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                WiFiDirectServiceManager.Instance.DiscoverServicesAsync(
                    ServiceName.Text,
                    RequestedServiceInfo.Text
                    );
            }

            // An application has two ways to connect over Wi-Fi Direct services
            // 1) First call GetProvisioningInfoAsync to begin connecting,
            //        a) if a PIN is required (group formation is required and selected config method is PinEntry or PinDisplay) then call ConnectAsync with the PIN
            //        b) else if no PIN is required then call ConnectAsync with no PIN
            // 2) Call ConnectAsync to connect without a PIN and prefer the Wi-Fi Direct Default config method, if possible

            private async void GetProvisioningInfo_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (DiscoveredServices.Items.Count > 0)
                {
                    int index = DiscoveredServices.SelectedIndex;
                    if (index >= 0)
                    {
                        DiscoveredDeviceWrapper service = (DiscoveredDeviceWrapper)DiscoveredServices.Items[index];

                        WiFiDirectServiceConfigurationMethod configMethod = WiFiDirectServiceConfigurationMethod.Default;
                        if (((ComboBoxItem)SelectedConfigMethod.SelectedItem).Content.ToString() == "WFDS Default")
                        {
                            configMethod = WiFiDirectServiceConfigurationMethod.Default;
                        }
                        else if (((ComboBoxItem)SelectedConfigMethod.SelectedItem).Content.ToString() == "PIN Keypad")
                        {
                            configMethod = WiFiDirectServiceConfigurationMethod.PinEntry;
                        }
                        else if (((ComboBoxItem)SelectedConfigMethod.SelectedItem).Content.ToString() == "PIN Display")
                        {
                            configMethod = WiFiDirectServiceConfigurationMethod.PinDisplay;
                        }

                        await service.OpenSessionAsync();
                        await service.SetServiceOptionsAsync(PreferGO.IsChecked ?? false, SessionInfo.Text);
                        provisioningInfo = await service.GetProvisioningInfoAsync(configMethod);
                    }
                }
            }

            private async void DoConnect_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
            {
                if (DiscoveredServices.Items.Count > 0)
                {
                    int index = DiscoveredServices.SelectedIndex;
                    if (index >= 0)
                    {
                        DiscoveredDeviceWrapper service = (DiscoveredDeviceWrapper)DiscoveredServices.Items[index];
                        
                        await service.SetServiceOptionsAsync(PreferGO.IsChecked ?? false, SessionInfo.Text);

                        if (provisioningInfo != null &&
                            provisioningInfo.IsGroupFormationNeeded &&
                            provisioningInfo.SelectedConfigurationMethod != WiFiDirectServiceConfigurationMethod.Default)
                        {
                            // These are the conditions under which a PIN is required
                            service.ConnectAsync(ConnectPin.Text);
                        }
                        else
                        {
                            // No PIN required
                            service.ConnectAsync();
                        }
                    }
                }
            }
        }
    }
}
