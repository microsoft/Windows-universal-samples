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
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.WiFiDirect;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using Windows.Security.Cryptography;
using Windows.UI.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_Advertiser : Page
    {
        private MainPage rootPage = MainPage.Current;
        private ObservableCollection<ConnectedDevice> ConnectedDevices = new ObservableCollection<ConnectedDevice>();
        WiFiDirectAdvertisementPublisher _publisher;
        WiFiDirectConnectionListener _listener;
        List<WiFiDirectInformationElement> _informationElements = new List<WiFiDirectInformationElement>();
        ConcurrentDictionary<StreamSocketListener, WiFiDirectDevice> _pendingConnections = new ConcurrentDictionary<StreamSocketListener, WiFiDirectDevice>();

        public Scenario1_Advertiser()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (btnStopAdvertisement.IsEnabled)
            {
                StopAdvertisement();
            }
        }

        private void btnStartAdvertisement_Click(object sender, RoutedEventArgs e)
        {
            _publisher = new WiFiDirectAdvertisementPublisher();
            _publisher.StatusChanged += OnStatusChanged;

            _listener = new WiFiDirectConnectionListener();

            if (chkListener.IsChecked.Value)
            {
                try
                {
                    // This can raise an exception if the machine does not support WiFi. Sorry.
                    _listener.ConnectionRequested += OnConnectionRequested;
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser($"Error preparing Advertisement: {ex}", NotifyType.ErrorMessage);
                    return;
                }
            }

            var discoverability = Utils.GetSelectedItemTag<WiFiDirectAdvertisementListenStateDiscoverability>(cmbListenState);
            _publisher.Advertisement.ListenStateDiscoverability = discoverability;

            _publisher.Advertisement.IsAutonomousGroupOwnerEnabled = chkPreferGroupOwnerMode.IsChecked.Value;

            // Legacy settings are meaningful only if IsAutonomousGroupOwnerEnabled is true.
            if (_publisher.Advertisement.IsAutonomousGroupOwnerEnabled && chkLegacySetting.IsChecked.Value)
            {
                _publisher.Advertisement.LegacySettings.IsEnabled = true;
                if (!String.IsNullOrEmpty(txtPassphrase.Text))
                {
                    var creds = new Windows.Security.Credentials.PasswordCredential();
                    creds.Password = txtPassphrase.Text;
                    _publisher.Advertisement.LegacySettings.Passphrase = creds;
                }

                if (!String.IsNullOrEmpty(txtSsid.Text))
                {
                    _publisher.Advertisement.LegacySettings.Ssid = txtSsid.Text;
                }
            }

            // Add the information elements.
            foreach (WiFiDirectInformationElement informationElement in _informationElements)
            {
                _publisher.Advertisement.InformationElements.Add(informationElement);
            }

            _publisher.Start();

            if (_publisher.Status == WiFiDirectAdvertisementPublisherStatus.Started)
            {
                btnStartAdvertisement.IsEnabled = false;
                btnStopAdvertisement.IsEnabled = true;
                rootPage.NotifyUser("Advertisement started.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser($"Advertisement failed to start. Status is {_publisher.Status}", NotifyType.ErrorMessage);
            }
        }

        private void btnAddIe_Click(object sender, RoutedEventArgs e)
        {
            WiFiDirectInformationElement informationElement = new WiFiDirectInformationElement();

            // Information element blob
            DataWriter dataWriter = new DataWriter();
            dataWriter.UnicodeEncoding = UnicodeEncoding.Utf8;
            dataWriter.ByteOrder = ByteOrder.LittleEndian;
            dataWriter.WriteUInt32(dataWriter.MeasureString(txtInformationElement.Text));
            dataWriter.WriteString(txtInformationElement.Text);
            informationElement.Value = dataWriter.DetachBuffer();

            // Organizational unit identifier (OUI)
            informationElement.Oui = CryptographicBuffer.CreateFromByteArray(Globals.CustomOui);

            // OUI Type
            informationElement.OuiType = Globals.CustomOuiType;

            // Save this information element so we can add it when we advertise.
            _informationElements.Add(informationElement);

            txtInformationElement.Text = "";
            rootPage.NotifyUser("IE added successfully", NotifyType.StatusMessage);
        }

        private void btnStopAdvertisement_Click(object sender, RoutedEventArgs e)
        {
            StopAdvertisement();
            rootPage.NotifyUser("Advertisement stopped successfully", NotifyType.StatusMessage);
        }

        private void StopAdvertisement()
        {
            _publisher.Stop();
            _publisher.StatusChanged -= OnStatusChanged;

            _listener.ConnectionRequested -= OnConnectionRequested;

            connectionSettingsPanel.Reset();
            _informationElements.Clear();

            btnStartAdvertisement.IsEnabled = true;
            btnStopAdvertisement.IsEnabled = false;
        }

        private async Task<bool> HandleConnectionRequestAsync(WiFiDirectConnectionRequest connectionRequest)
        {
            string deviceName = connectionRequest.DeviceInformation.Name;

            bool isPaired = (connectionRequest.DeviceInformation.Pairing?.IsPaired == true) ||
                            (await IsAepPairedAsync(connectionRequest.DeviceInformation.Id));

            // Show the prompt only in case of WiFiDirect reconnection or Legacy client connection.
            if (isPaired || _publisher.Advertisement.LegacySettings.IsEnabled)
            {
                var messageDialog = new MessageDialog($"Connection request received from {deviceName}", "Connection Request");

                // Add two commands, distinguished by their tag.
                // The default command is "Decline", and if the user cancels, we treat it as "Decline".
                messageDialog.Commands.Add(new UICommand("Accept", null, true));
                messageDialog.Commands.Add(new UICommand("Decline", null, null));
                messageDialog.DefaultCommandIndex = 1;
                messageDialog.CancelCommandIndex = 1;

                // Show the message dialog
                var commandChosen = await messageDialog.ShowAsync();

                if (commandChosen.Id == null)
                {
                    return false;
                }
            }

            rootPage.NotifyUser($"Connecting to {deviceName}...", NotifyType.StatusMessage);

            // Pair device if not already paired and not using legacy settings
            if (!isPaired && !_publisher.Advertisement.LegacySettings.IsEnabled)
            {
                if (!await connectionSettingsPanel.RequestPairDeviceAsync(connectionRequest.DeviceInformation.Pairing))
                {
                    return false;
                }
            }

            WiFiDirectDevice wfdDevice = null;
            try
            {
                // IMPORTANT: FromIdAsync needs to be called from the UI thread
                wfdDevice = await WiFiDirectDevice.FromIdAsync(connectionRequest.DeviceInformation.Id);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser($"Exception in FromIdAsync: {ex}", NotifyType.ErrorMessage);
                return false;
            }

            // Register for the ConnectionStatusChanged event handler
            wfdDevice.ConnectionStatusChanged += OnConnectionStatusChanged;

            var listenerSocket = new StreamSocketListener();

            // Save this (listenerSocket, wfdDevice) pair so we can hook it up when the socket connection is made.
            _pendingConnections[listenerSocket] = wfdDevice;

            var EndpointPairs = wfdDevice.GetConnectionEndpointPairs();

            listenerSocket.ConnectionReceived += OnSocketConnectionReceived;
            try
            {
                await listenerSocket.BindEndpointAsync(EndpointPairs[0].LocalHostName, Globals.strServerPort);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser($"Connect operation threw an exception: {ex.Message}", NotifyType.ErrorMessage);
                return false;
            }

            rootPage.NotifyUser($"Devices connected on L2, listening on IP Address: {EndpointPairs[0].LocalHostName}" +
                                $" Port: {Globals.strServerPort}", NotifyType.StatusMessage);
            return true;
        }

        private async void OnConnectionRequested(WiFiDirectConnectionListener sender, WiFiDirectConnectionRequestedEventArgs connectionEventArgs)
        {
            WiFiDirectConnectionRequest connectionRequest = connectionEventArgs.GetConnectionRequest();
            bool success = await Dispatcher.RunTaskAsync(async () =>
            {
                return await HandleConnectionRequestAsync(connectionRequest);
            });

            if (!success)
            {
                // Decline the connection request
                rootPage.NotifyUserFromBackground($"Connection request from {connectionRequest.DeviceInformation.Name} was declined", NotifyType.ErrorMessage);
                connectionRequest.Dispose();
            }
        }

        private async Task<bool> IsAepPairedAsync(string deviceId)
        {
            List<string> additionalProperties = new List<string>();
            additionalProperties.Add("System.Devices.Aep.DeviceAddress");
            String deviceSelector = $"System.Devices.Aep.AepId:=\"{deviceId}\"";
            DeviceInformation devInfo = null;

            try
            {
                devInfo = await DeviceInformation.CreateFromIdAsync(deviceId, additionalProperties);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("DeviceInformation.CreateFromIdAsync threw an exception: " + ex.Message, NotifyType.ErrorMessage);
            }

            if (devInfo == null)
            {
                rootPage.NotifyUser("Device Information is null", NotifyType.ErrorMessage);
                return false;
            }

            deviceSelector = $"System.Devices.Aep.DeviceAddress:=\"{devInfo.Properties["System.Devices.Aep.DeviceAddress"]}\"";
            DeviceInformationCollection pairedDeviceCollection = await DeviceInformation.FindAllAsync(deviceSelector, null, DeviceInformationKind.Device);
            return pairedDeviceCollection.Count > 0;
        }

        private async void OnStatusChanged(WiFiDirectAdvertisementPublisher sender, WiFiDirectAdvertisementPublisherStatusChangedEventArgs statusEventArgs)
        {
            if (statusEventArgs.Status == WiFiDirectAdvertisementPublisherStatus.Started)
            {
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    if (sender.Advertisement.LegacySettings.IsEnabled)
                    {
                        // Show the autogenerated passphrase and SSID.
                        if (String.IsNullOrEmpty(txtPassphrase.Text))
                        {
                            txtPassphrase.Text = _publisher.Advertisement.LegacySettings.Passphrase.Password;
                        }

                        if (String.IsNullOrEmpty(txtSsid.Text))
                        {
                            txtSsid.Text = _publisher.Advertisement.LegacySettings.Ssid;
                        }
                    }
                });
            }

            rootPage.NotifyUserFromBackground($"Advertisement: Status: {statusEventArgs.Status}, Error: {statusEventArgs.Error}", NotifyType.StatusMessage);
            return;
        }

        private void OnSocketConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args)
        {
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                rootPage.NotifyUser("Connecting to remote side on L4 layer...", NotifyType.StatusMessage);
                StreamSocket serverSocket = args.Socket;

                SocketReaderWriter socketRW = new SocketReaderWriter(serverSocket, rootPage);
                // The first message sent is the name of the connection.
                string message = await socketRW.ReadMessageAsync();

                // Find the pending connection and add it to the list of active connections.
                WiFiDirectDevice wfdDevice;
                if (_pendingConnections.TryRemove(sender, out wfdDevice))
                {
                    ConnectedDevices.Add(new ConnectedDevice(message, wfdDevice, socketRW));
                }

                while (message != null)
                {
                    message = await socketRW.ReadMessageAsync();
                }
            });
        }

        private void OnConnectionStatusChanged(WiFiDirectDevice sender, object arg)
        {
            rootPage.NotifyUserFromBackground($"Connection status changed: {sender.ConnectionStatus}", NotifyType.StatusMessage);

            if (sender.ConnectionStatus == WiFiDirectConnectionStatus.Disconnected)
            {
                // TODO: Should we remove this connection from the list?
                // (Yes, probably.)
            }
        }

        private async void btnSendMessage_Click(object sender, RoutedEventArgs e)
        {
            var connectedDevice = (ConnectedDevice)lvConnectedDevices.SelectedItem;
            await connectedDevice.SocketRW.WriteMessageAsync(txtSendMessage.Text);
        }

        private bool CanCloseDevice(object connectedDevice)
        {
            return connectedDevice != null;
        }

        private void btnCloseDevice_Click(object sender, RoutedEventArgs e)
        {
            var connectedDevice = (ConnectedDevice)lvConnectedDevices.SelectedItem;
            ConnectedDevices.Remove(connectedDevice);

            // Close socket and WiFiDirect object
            connectedDevice.Dispose();
        }
    }
}

