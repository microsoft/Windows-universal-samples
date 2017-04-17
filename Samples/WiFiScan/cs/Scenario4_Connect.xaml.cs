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

using SDKTemplate;
using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Devices.WiFi;
using Windows.Networking.Connectivity;
using Windows.Security.Credentials;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace WiFiScan
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario4_Connect : Page
    {
        MainPage rootPage;
        private WiFiAdapter firstAdapter;
        private string savedProfileName = null;

        public ObservableCollection<WiFiNetworkDisplay> ResultCollection
        {
            get;
            private set;
        }

        public Scenario4_Connect()
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

                    var scanButton = new Button();
                    scanButton.Content = string.Format("Scan");
                    scanButton.Click += ScanButton_Click;
                    Buttons.Children.Add(scanButton);

                    var disconnectButton = new Button();
                    disconnectButton.Content = string.Format("Disconnect");
                    disconnectButton.Click += DisconnectButton_Click; ;
                    Buttons.Children.Add(disconnectButton);

                    // Monitor network status changes
                    await UpdateConnectivityStatusAsync();
                    NetworkInformation.NetworkStatusChanged += NetworkInformation_NetworkStatusChanged;
                }
                else
                {
                    rootPage.NotifyUser("No WiFi Adapters detected on this machine", NotifyType.ErrorMessage);
                }
            }
        }

        private void DisconnectButton_Click(object sender, RoutedEventArgs e)
        {
            firstAdapter.Disconnect();
        }

        private async void NetworkInformation_NetworkStatusChanged(object sender)
        {
            await UpdateConnectivityStatusAsync();

            // Update the connectivity level displayed for each
            foreach (var network in ResultCollection)
            {
                await network.UpdateConnectivityLevel();
            }
        }

        private async Task UpdateConnectivityStatusAsync()
        {
            var connectedProfile = await firstAdapter.NetworkAdapter.GetConnectedProfileAsync();
            if (connectedProfile != null && !connectedProfile.ProfileName.Equals(savedProfileName))
            {
                savedProfileName = connectedProfile.ProfileName;
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser(string.Format("WiFi adapter connected to: {0} ({1})", connectedProfile.ProfileName, connectedProfile.GetNetworkConnectivityLevel()), NotifyType.StatusMessage);
                });
            }
            else if (connectedProfile == null && savedProfileName != null)
            {
                savedProfileName = null;
                await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser("WiFi adapter disconnected", NotifyType.StatusMessage);
                });
            }
        }

        private async void ScanButton_Click(object sender, RoutedEventArgs e)
        {
            await firstAdapter.ScanAsync();
            ConnectionBar.Visibility = Visibility.Collapsed;
            DisplayNetworkReport(firstAdapter.NetworkReport);
        }

        private async void DisplayNetworkReport(WiFiNetworkReport report)
        {
            rootPage.NotifyUser(string.Format("Network Report Timestamp: {0}", report.Timestamp), NotifyType.StatusMessage);

            ResultCollection.Clear();

            foreach (var network in report.AvailableNetworks)
            {
                var networkDisplay = new WiFiNetworkDisplay(network, firstAdapter);
                await networkDisplay.UpdateConnectivityLevel();
                ResultCollection.Add(networkDisplay);
            }
        }

        private void ResultsListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var selectedNetwork = ResultsListView.SelectedItem as WiFiNetworkDisplay;
            if (selectedNetwork == null)
            {
                return;
            }

            // Show the connection bar
            ConnectionBar.Visibility = Visibility.Visible;

            // Only show the password box if needed
            if (selectedNetwork.AvailableNetwork.SecuritySettings.NetworkAuthenticationType == NetworkAuthenticationType.Open80211 &&
                selectedNetwork.AvailableNetwork.SecuritySettings.NetworkEncryptionType == NetworkEncryptionType.None)
            {
                NetworkKeyInfo.Visibility = Visibility.Collapsed;
            }
            else
            {
                NetworkKeyInfo.Visibility = Visibility.Visible;
            }
        }

        private async void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            var selectedNetwork = ResultsListView.SelectedItem as WiFiNetworkDisplay;
            if (selectedNetwork == null || firstAdapter == null)
            {
                rootPage.NotifyUser("Network not selcted", NotifyType.ErrorMessage);
                return;
            }
            WiFiReconnectionKind reconnectionKind = WiFiReconnectionKind.Manual;
            if (IsAutomaticReconnection.IsChecked.HasValue && IsAutomaticReconnection.IsChecked == true)
            {
                reconnectionKind = WiFiReconnectionKind.Automatic;
            }

            WiFiConnectionResult result;
            if (selectedNetwork.AvailableNetwork.SecuritySettings.NetworkAuthenticationType == NetworkAuthenticationType.Open80211 &&
                selectedNetwork.AvailableNetwork.SecuritySettings.NetworkEncryptionType == NetworkEncryptionType.None)
            {
                result = await firstAdapter.ConnectAsync(selectedNetwork.AvailableNetwork, reconnectionKind);
            }
            else
            {
                // Only the password portion of the credential need to be supplied
                var credential = new PasswordCredential();

                // Make sure Credential.Password property is not set to an empty string. 
                // Otherwise, a System.ArgumentException will be thrown.
                // The default empty password string will still be passed to the ConnectAsync method,
                // which should return an "InvalidCredential" error
                if (!string.IsNullOrEmpty(NetworkKey.Password))
                {
                    credential.Password = NetworkKey.Password;
                }

                result = await firstAdapter.ConnectAsync(selectedNetwork.AvailableNetwork, reconnectionKind, credential);
            }

            if (result.ConnectionStatus == WiFiConnectionStatus.Success)
            {
                rootPage.NotifyUser(string.Format("Successfully connected to {0}.", selectedNetwork.Ssid), NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(string.Format("Could not connect to {0}. Error: {1}", selectedNetwork.Ssid, result.ConnectionStatus), NotifyType.ErrorMessage);
            }
        }

    }
}

