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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.WiFiDirect;
using Windows.Networking.Sockets;
using Windows.UI.Core;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Storage.Streams;
using Windows.Devices.Enumeration;
using Windows.UI.Popups;
using System.Collections.Generic;
using System;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;
        public ObservableCollection<ConnectedDevice> _connectedDevices
        {
            get;
            private set;
        }
        WiFiDirectAdvertisementPublisher _publisher;
        WiFiDirectConnectionListener _listener;
        StreamSocketListener _listenerSocket;

        public Scenario1()
        {
            this.InitializeComponent();

            _connectedDevices = new ObservableCollection<ConnectedDevice>();
            _listenerSocket = null;

            lvConnectedDevices.ItemsSource = _connectedDevices;
            lvConnectedDevices.SelectionMode = ListViewSelectionMode.Single;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void btnStartAdvertisement_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (_publisher == null)
                {
                    _publisher = new WiFiDirectAdvertisementPublisher();
                }

                if (chkListener.IsChecked == true)
                {
                    if (_listener == null)
                    {
                        _listener = new WiFiDirectConnectionListener();
                    }

                    _listener.ConnectionRequested += OnConnectionRequested;
                }

                _publisher.Advertisement.IsAutonomousGroupOwnerEnabled = (chkPreferGroupOwnerMode.IsChecked == true);

                if (cmbListenState.SelectionBoxItem.ToString().Equals("Normal") == true)
                {
                    _publisher.Advertisement.ListenStateDiscoverability = WiFiDirectAdvertisementListenStateDiscoverability.Normal;
                }
                else if (cmbListenState.SelectionBoxItem.ToString().Equals("Intensive") == true)
                {
                    _publisher.Advertisement.ListenStateDiscoverability = WiFiDirectAdvertisementListenStateDiscoverability.Intensive;
                }
                else if (cmbListenState.SelectionBoxItem.ToString().Equals("None") == true)
                {
                    _publisher.Advertisement.ListenStateDiscoverability = WiFiDirectAdvertisementListenStateDiscoverability.None;
                }

                _publisher.StatusChanged += OnStatusChanged;

                _publisher.Start();

                rootPage.NotifyUser("Advertisement started, waiting for StatusChanged callback...", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error starting Advertisement: " + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private void btnAddIe_Click(object sender, RoutedEventArgs e)
        {
            if (_publisher == null)
            {
                _publisher = new WiFiDirectAdvertisementPublisher();
            }

            if (txtInformationElement.Text == "")
            {
                rootPage.NotifyUser("Please specifiy an IE", NotifyType.ErrorMessage);
                return;
            }

            WiFiDirectInformationElement IE = new WiFiDirectInformationElement();

            // IE blob
            DataWriter dataWriter = new DataWriter();
            dataWriter.UnicodeEncoding = UnicodeEncoding.Utf8;
            dataWriter.ByteOrder = ByteOrder.LittleEndian;
            dataWriter.WriteUInt32(dataWriter.MeasureString(txtInformationElement.Text));
            dataWriter.WriteString(txtInformationElement.Text);
            IE.Value = dataWriter.DetachBuffer();

            // OUI
            DataWriter dataWriterOUI = new DataWriter();
            dataWriterOUI.WriteBytes(Globals.CustomOui);
            IE.Oui = dataWriterOUI.DetachBuffer();

            // OUI Type
            IE.OuiType = Globals.CustomOuiType;

            _publisher.Advertisement.InformationElements.Add(IE);
            txtInformationElement.Text = "";

            rootPage.NotifyUser("IE added successfully", NotifyType.StatusMessage);
        }

        private void btnStopAdvertisement_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (_publisher != null)
                {
                    _publisher.Stop();
                    _publisher.StatusChanged -= OnStatusChanged;
                    rootPage.NotifyUser("Advertisement stopped successfully", NotifyType.StatusMessage);
                }

                if (_listener != null)
                {
                    _listener.ConnectionRequested -= OnConnectionRequested;
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Error stopping Advertisement: " + ex.ToString(), NotifyType.ErrorMessage);
            }
        }

        private async void OnConnectionRequested(WiFiDirectConnectionListener sender, WiFiDirectConnectionRequestedEventArgs connectionEventArgs)
        {
            try
            {
                var connectionRequest = connectionEventArgs.GetConnectionRequest();

                var tcs = new TaskCompletionSource<bool>();
                var dialogTask = tcs.Task;
                var messageDialog = new MessageDialog("Connection request received from " + connectionRequest.DeviceInformation.Name, "Connection Request");

                // Add commands and set their callbacks; both buttons use the same callback function instead of inline event handlers 
                messageDialog.Commands.Add(new UICommand("Accept", null, 0));
                messageDialog.Commands.Add(new UICommand("Decline", null, 1));

                // Set the command that will be invoked by default 
                messageDialog.DefaultCommandIndex = 1;

                // Set the command to be invoked when escape is pressed 
                messageDialog.CancelCommandIndex = 1;

                await Dispatcher.RunAsync(CoreDispatcherPriority.High, async () =>
                {
                    // Show the message dialog 
                    var commandChosen = await messageDialog.ShowAsync();

                    tcs.SetResult((commandChosen.Label == "Accept") ? true : false);
                });

                var fProceed = await dialogTask;

                if (fProceed == true)
                {
                    var tcsWiFiDirectDevice = new TaskCompletionSource<WiFiDirectDevice>();
                    var wfdDeviceTask = tcsWiFiDirectDevice.Task;

                    await Dispatcher.RunAsync(CoreDispatcherPriority.High, async () =>
                    {
                        try
                        {
                            rootPage.NotifyUser("Connecting to " + connectionRequest.DeviceInformation.Name + "...", NotifyType.StatusMessage);

                            WiFiDirectConnectionParameters connectionParams = new WiFiDirectConnectionParameters();
                            connectionParams.GroupOwnerIntent = Convert.ToInt16(txtGOIntent.Text);

                            // IMPORTANT: FromIdAsync needs to be called from the UI thread
                            tcsWiFiDirectDevice.SetResult(await WiFiDirectDevice.FromIdAsync(connectionRequest.DeviceInformation.Id, connectionParams));
                        }
                        catch (Exception ex)
                        {
                            rootPage.NotifyUser("FromIdAsync task threw an exception: " + ex.ToString(), NotifyType.ErrorMessage);
                        }
                    });

                    WiFiDirectDevice wfdDevice = await wfdDeviceTask;

                    // Register for the ConnectionStatusChanged event handler
                    wfdDevice.ConnectionStatusChanged += OnConnectionStatusChanged;

                    await Dispatcher.RunAsync(CoreDispatcherPriority.High, () =>
                    {
                        ConnectedDevice connectedDevice = new ConnectedDevice("Waiting for client to connect...", wfdDevice, null);
                        _connectedDevices.Add(connectedDevice);
                    });

                    var EndpointPairs = wfdDevice.GetConnectionEndpointPairs();

                    _listenerSocket = null;
                    _listenerSocket = new StreamSocketListener();
                    _listenerSocket.ConnectionReceived += OnSocketConnectionReceived;
                    await _listenerSocket.BindEndpointAsync(EndpointPairs[0].LocalHostName, Globals.strServerPort);

                    rootPage.NotifyUserFromBackground("Devices connected on L2, listening on IP Address: " + EndpointPairs[0].LocalHostName.ToString() +
                                            " Port: " + Globals.strServerPort, NotifyType.StatusMessage);
                }
                else
                {
                    // Decline the connection request
                    rootPage.NotifyUserFromBackground("Connection request from " + connectionRequest.DeviceInformation.Name + " was declined", NotifyType.ErrorMessage);
                    connectionRequest.Dispose();
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUserFromBackground("Connect operation threw an exception: " + ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void OnStatusChanged(WiFiDirectAdvertisementPublisher sender, WiFiDirectAdvertisementPublisherStatusChangedEventArgs statusEventArgs)
        {
            rootPage.NotifyUserFromBackground("Advertisement: Status: " + statusEventArgs.Status.ToString() + " Error: " + statusEventArgs.Error.ToString(), NotifyType.StatusMessage);
            return;
        }

        private async void OnSocketConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args)
        {
            rootPage.NotifyUserFromBackground("Connecting to remote side on L4 layer...", NotifyType.StatusMessage);
            StreamSocket serverSocket = args.Socket;

            try
            {
                SocketReaderWriter socketRW = new SocketReaderWriter(serverSocket, rootPage);
                socketRW.ReadMessage();

                while (true)
                {
                    string sessionId = socketRW.GetCurrentMessage();
                    if (sessionId != null)
                    {
                        rootPage.NotifyUserFromBackground("Connected with remote side on L4 layer", NotifyType.StatusMessage);

                        await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            for (int idx = 0; idx < _connectedDevices.Count; idx++)
                            {
                                if (_connectedDevices[idx].DisplayName.Equals("Waiting for client to connect...") == true)
                                {
                                    ConnectedDevice connectedDevice = _connectedDevices[idx];
                                    _connectedDevices.RemoveAt(idx);

                                    connectedDevice.DisplayName = sessionId;
                                    connectedDevice.SocketRW = socketRW;

                                    _connectedDevices.Add(connectedDevice);
                                    break;
                                }
                            }
                        });


                        break;
                    }

                    await Task.Delay(100);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUserFromBackground("Connection failed: " + ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void OnConnectionStatusChanged(WiFiDirectDevice sender, object arg)
        {
            rootPage.NotifyUserFromBackground("Connection status changed: " + sender.ConnectionStatus, NotifyType.StatusMessage);
        }

        private void btnSendMessage_Click(object sender, RoutedEventArgs e)
        {
            if (lvConnectedDevices.SelectedItems.Count == 0)
            {
                rootPage.NotifyUser("Please select a Session to send data", NotifyType.ErrorMessage);
                return;
            }

            if (txtSendMessage.Text == "")
            {
                rootPage.NotifyUser("Please type a message to send", NotifyType.ErrorMessage);
                return;
            }

            try
            {
                foreach (ConnectedDevice connectedDevice in lvConnectedDevices.SelectedItems)
                {
                    connectedDevice.SocketRW.WriteMessage(txtSendMessage.Text);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Send threw an exception: " + ex.Message, NotifyType.ErrorMessage);
            }
        }

        private void btnCloseDevice_Click(object sender, RoutedEventArgs e)
        {
            if (lvConnectedDevices.SelectedItems.Count == 0)
            {
                rootPage.NotifyUser("Please select a device to close", NotifyType.ErrorMessage);
                return;
            }

            try
            {
                foreach (ConnectedDevice connectedDevice in lvConnectedDevices.SelectedItems)
                {
                    // Close socket
                    connectedDevice.SocketRW.Dispose();

                    // Close WiFiDirectDevice object
                    connectedDevice.WfdDevice.Dispose();
                    _connectedDevices.Remove(connectedDevice);

                    rootPage.NotifyUser(connectedDevice.DisplayName + " closed successfully", NotifyType.StatusMessage);
                }
            }
            catch (Exception ex)
            {
                rootPage.NotifyUser("Close threw an exception: " + ex.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
