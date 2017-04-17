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
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.WiFiDirect;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Security.Cryptography;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_Connector : Page
    {
        private MainPage rootPage = MainPage.Current;
        DeviceWatcher _deviceWatcher;
        bool _fWatcherStarted = false;
        WiFiDirectAdvertisementPublisher _publisher = new WiFiDirectAdvertisementPublisher();

        public ObservableCollection<DiscoveredDevice> DiscoveredDevices { get; } = new ObservableCollection<DiscoveredDevice>();
        public ObservableCollection<ConnectedDevice> ConnectedDevices { get; } = new ObservableCollection<ConnectedDevice>();

        public Scenario2_Connector()
        {
            this.InitializeComponent();
        }

        private void btnWatcher_Click(object sender, RoutedEventArgs e)
        {
            if (_fWatcherStarted == false)
            {
                _publisher.Start();

                if (_publisher.Status != WiFiDirectAdvertisementPublisherStatus.Started)
                {
                    rootPage.NotifyUser("Failed to start advertisement.", NotifyType.ErrorMessage);
                    return;
                }

                DiscoveredDevices.Clear();
                rootPage.NotifyUser("Finding Devices...", NotifyType.StatusMessage);

                String deviceSelector = WiFiDirectDevice.GetDeviceSelector(
                    Utils.GetSelectedItemTag<WiFiDirectDeviceSelectorType>(cmbDeviceSelector));

                _deviceWatcher = DeviceInformation.CreateWatcher(deviceSelector, new string[] { "System.Devices.WiFiDirect.InformationElements" });

                _deviceWatcher.Added += OnDeviceAdded;
                _deviceWatcher.Removed += OnDeviceRemoved;
                _deviceWatcher.Updated += OnDeviceUpdated;
                _deviceWatcher.EnumerationCompleted += OnEnumerationCompleted;
                _deviceWatcher.Stopped += OnStopped;

                _deviceWatcher.Start();

                btnWatcher.Content = "Stop Watcher";
                _fWatcherStarted = true;
            }
            else
            {
                _publisher.Stop();

                btnWatcher.Content = "Start Watcher";
                _fWatcherStarted = false;

                _deviceWatcher.Added -= OnDeviceAdded;
                _deviceWatcher.Removed -= OnDeviceRemoved;
                _deviceWatcher.Updated -= OnDeviceUpdated;
                _deviceWatcher.EnumerationCompleted -= OnEnumerationCompleted;
                _deviceWatcher.Stopped -= OnStopped;

                _deviceWatcher.Stop();

                rootPage.NotifyUser("Device watcher stopped.", NotifyType.StatusMessage);
            }
        }

        #region DeviceWatcherEvents
        private async void OnDeviceAdded(DeviceWatcher deviceWatcher, DeviceInformation deviceInfo)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DiscoveredDevices.Add(new DiscoveredDevice(deviceInfo));
            });
        }

        private async void OnDeviceRemoved(DeviceWatcher deviceWatcher, DeviceInformationUpdate deviceInfoUpdate)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                foreach (DiscoveredDevice discoveredDevice in DiscoveredDevices)
                {
                    if (discoveredDevice.DeviceInfo.Id == deviceInfoUpdate.Id)
                    {
                        DiscoveredDevices.Remove(discoveredDevice);
                        break;
                    }
                }
            });
        }

        private async void OnDeviceUpdated(DeviceWatcher deviceWatcher, DeviceInformationUpdate deviceInfoUpdate)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                foreach (DiscoveredDevice discoveredDevice in DiscoveredDevices)
                {
                    if (discoveredDevice.DeviceInfo.Id == deviceInfoUpdate.Id)
                    {
                        discoveredDevice.UpdateDeviceInfo(deviceInfoUpdate);
                        break;
                    }
                }
            });
        }

        private void OnEnumerationCompleted(DeviceWatcher deviceWatcher, object o)
        {
            rootPage.NotifyUserFromBackground("DeviceWatcher enumeration completed", NotifyType.StatusMessage);
        }

        private void OnStopped(DeviceWatcher deviceWatcher, object o)
        {
            rootPage.NotifyUserFromBackground("DeviceWatcher stopped", NotifyType.StatusMessage);
        }
        #endregion

        private void btnIe_Click(object sender, RoutedEventArgs e)
        {
            var discoveredDevice = (DiscoveredDevice)lvDiscoveredDevices.SelectedItem;

            if (discoveredDevice == null)
            {
                rootPage.NotifyUser("No device selected, please select one.", NotifyType.ErrorMessage);
                return;
            }

            IList<WiFiDirectInformationElement> informationElements = null;
            try
            {
                informationElements = WiFiDirectInformationElement.CreateFromDeviceInformation(discoveredDevice.DeviceInfo);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("No Information element found: " + ex.Message, NotifyType.ErrorMessage);
            }

            if (informationElements != null)
            {
                StringWriter message = new StringWriter();

                foreach (WiFiDirectInformationElement informationElement in informationElements)
                {
                    string ouiName = CryptographicBuffer.EncodeToHexString(informationElement.Oui);
                    string value = string.Empty;
                    Byte[] bOui = informationElement.Oui.ToArray();

                    if (bOui.SequenceEqual(Globals.MsftOui))
                    {
                        // The format of Microsoft information elements is documented here:
                        // https://msdn.microsoft.com/en-us/library/dn392651.aspx
                        // with errata here:
                        // https://msdn.microsoft.com/en-us/library/mt242386.aspx
                        ouiName += " (Microsoft)";
                    }
                    else if (bOui.SequenceEqual(Globals.WfaOui))
                    {
                        ouiName += " (WFA)";
                    }
                    else if (bOui.SequenceEqual(Globals.CustomOui))
                    {
                        ouiName += " (Custom)";

                        if (informationElement.OuiType == Globals.CustomOuiType)
                        {
                            DataReader dataReader = DataReader.FromBuffer(informationElement.Value);
                            dataReader.UnicodeEncoding = UnicodeEncoding.Utf8;
                            dataReader.ByteOrder = ByteOrder.LittleEndian;

                            // Read the string.
                            try
                            {
                                string data = dataReader.ReadString(dataReader.ReadUInt32());
                                value = $"Data: {data}";
                            }
                            catch (Exception)
                            {
                                value = "(Unable to parse)";
                            }
                        }
                    }

                    message.WriteLine($"OUI {ouiName}, Type {informationElement.OuiType} {value}");
                }

                message.Write($"Information elements found: {informationElements.Count}");

                rootPage.NotifyUser(message.ToString(), NotifyType.StatusMessage);
            }
        }

        private async void btnFromId_Click(object sender, RoutedEventArgs e)
        {
            var discoveredDevice = (DiscoveredDevice)lvDiscoveredDevices.SelectedItem;

            if (discoveredDevice == null)
            {
                rootPage.NotifyUser("No device selected, please select one.", NotifyType.ErrorMessage);
                return;
            }

            rootPage.NotifyUser($"Connecting to {discoveredDevice.DeviceInfo.Name}...", NotifyType.StatusMessage);

            if (!discoveredDevice.DeviceInfo.Pairing.IsPaired)
            {
                if (!await connectionSettingsPanel.RequestPairDeviceAsync(discoveredDevice.DeviceInfo.Pairing))
                {
                    return;
                }
            }

            try
            {
                // IMPORTANT: FromIdAsync needs to be called from the UI thread
                var wfdDevice = await WiFiDirectDevice.FromIdAsync(discoveredDevice.DeviceInfo.Id);

                // Register for the ConnectionStatusChanged event handler
                wfdDevice.ConnectionStatusChanged += OnConnectionStatusChanged;

                IReadOnlyList<EndpointPair> endpointPairs = wfdDevice.GetConnectionEndpointPairs();
                HostName remoteHostName = endpointPairs[0].RemoteHostName;

                rootPage.NotifyUser($"Devices connected on L2 layer, connecting to IP Address: {remoteHostName} Port: {Globals.strServerPort}",
                    NotifyType.StatusMessage);

                // Wait for server to start listening on a socket
                await Task.Delay(2000);

                // Connect to Advertiser on L4 layer
                StreamSocket clientSocket = new StreamSocket();
                await clientSocket.ConnectAsync(remoteHostName, Globals.strServerPort);
                rootPage.NotifyUser("Connected with remote side on L4 layer", NotifyType.StatusMessage);

                SocketReaderWriter socketRW = new SocketReaderWriter(clientSocket, rootPage);

                string sessionId = Path.GetRandomFileName();
                ConnectedDevice connectedDevice = new ConnectedDevice(sessionId, wfdDevice, socketRW);
                ConnectedDevices.Add(connectedDevice);

                // The first message sent over the socket is the name of the connection.
                await socketRW.WriteMessageAsync(sessionId);

                while (await socketRW.ReadMessageAsync() != null)
                {
                    // Keep reading messages
                }

            }
            catch (TaskCanceledException)
            {
                rootPage.NotifyUser("FromIdAsync was canceled by user", NotifyType.ErrorMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser($"Connect operation threw an exception: {ex.Message}", NotifyType.ErrorMessage);
            }
        }

        private void OnConnectionStatusChanged(WiFiDirectDevice sender, object arg)
        {
            rootPage.NotifyUserFromBackground($"Connection status changed: {sender.ConnectionStatus}", NotifyType.StatusMessage);
        }

        private async void btnSendMessage_Click(object sender, RoutedEventArgs e)
        {
            var connectedDevice = (ConnectedDevice)lvConnectedDevices.SelectedItem;
            await connectedDevice.SocketRW.WriteMessageAsync(txtSendMessage.Text);
        }

        private void btnClose_Click(object sender, RoutedEventArgs e)
        {
            var connectedDevice = (ConnectedDevice)lvConnectedDevices.SelectedItem;
            ConnectedDevices.Remove(connectedDevice);

            // Close socket and WiFiDirect object
            connectedDevice.Dispose();
        }

        private async void btnUnpair_Click(object sender, RoutedEventArgs e)
        {
            var discoveredDevice = (DiscoveredDevice)lvDiscoveredDevices.SelectedItem;

            if (discoveredDevice == null)
            {
                rootPage.NotifyUser("No device selected, please select one.", NotifyType.ErrorMessage);
                return;
            }

            DeviceUnpairingResult result = await discoveredDevice.DeviceInfo.Pairing.UnpairAsync();
            rootPage.NotifyUser($"Unpair result: {result.Status}", NotifyType.StatusMessage);
        }
    }
}
