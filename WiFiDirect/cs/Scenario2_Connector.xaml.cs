//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using System.Threading;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.WiFiDirect;
using System;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using Windows.UI.Core;
using System.IO;
using System.Threading.Tasks;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Linq;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2 : Page
    {
        private MainPage rootPage;
        DeviceWatcher _deviceWatcher;
        ReaderWriterLockSlim _discoveryRWLock;
        bool _fWatcherStarted;
        public ObservableCollection<DiscoveredDevice> _discoveredDevices
        {
            get;
            private set;
        }
        CancellationTokenSource _cancellationTokenSource;

        public ObservableCollection<ConnectedDevice> _connectedDevices
        {
            get;
            private set;
        }

        public Scenario2()
        {
            this.InitializeComponent();

            _discoveryRWLock = new ReaderWriterLockSlim(LockRecursionPolicy.SupportsRecursion);
            _fWatcherStarted = false;
            _discoveredDevices = new ObservableCollection<DiscoveredDevice>();
            _connectedDevices = new ObservableCollection<ConnectedDevice>();

            lvDiscoveredDevices.ItemsSource = _discoveredDevices;
            lvDiscoveredDevices.SelectionMode = ListViewSelectionMode.Single;

            lvConnectedDevices.ItemsSource = _connectedDevices;
            lvConnectedDevices.SelectionMode = ListViewSelectionMode.Single;
        }

        private void btnWatcher_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (_fWatcherStarted == false)
            {
                btnWatcher.Content = "Stop Watcher";
                _fWatcherStarted = true;

                _discoveredDevices.Clear();
                rootPage.NotifyUser("Finding Devices...", NotifyType.StatusMessage);

                _deviceWatcher = null;
                String deviceSelector = WiFiDirectDevice.GetDeviceSelector((cmbDeviceSelector.ToString().Equals("Device Interface") == true) ?
                                                            WiFiDirectDeviceSelectorType.DeviceInterface : WiFiDirectDeviceSelectorType.AssociationEndpoint);

                _deviceWatcher = DeviceInformation.CreateWatcher(deviceSelector, new string[] { "System.Devices.WiFiDirect.InformationElements" });

                _deviceWatcher.Added += OnDeviceAdded;
                _deviceWatcher.Removed += OnDeviceRemoved;
                _deviceWatcher.Updated += OnDeviceUpdated;
                _deviceWatcher.EnumerationCompleted += OnEnumerationCompleted;
                _deviceWatcher.Stopped += OnStopped;

                _deviceWatcher.Start();
            }
            else
            {
                btnWatcher.Content = "Start Watcher";
                _fWatcherStarted = false;

                _deviceWatcher.Added -= OnDeviceAdded;
                _deviceWatcher.Removed -= OnDeviceRemoved;
                _deviceWatcher.Updated -= OnDeviceUpdated;
                _deviceWatcher.EnumerationCompleted -= OnEnumerationCompleted;
                _deviceWatcher.Stopped -= OnStopped;

                _deviceWatcher.Stop();
            }
        }

        #region DeviceWatcherEvents
        private async void OnDeviceAdded(DeviceWatcher deviceWatcher, DeviceInformation deviceInfo)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                _discoveryRWLock.EnterWriteLock();

                var deviceInfoDisplay = new DiscoveredDevice(deviceInfo);
                _discoveredDevices.Add(deviceInfoDisplay);

                _discoveryRWLock.ExitWriteLock();
            });
        }

        private async void OnDeviceRemoved(DeviceWatcher deviceWatcher, DeviceInformationUpdate deviceInfoUpdate)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                _discoveryRWLock.EnterWriteLock();

                foreach (DiscoveredDevice devInfodisplay in _discoveredDevices)
                {
                    if (devInfodisplay.DeviceInfo.Id == deviceInfoUpdate.Id)
                    {
                        _discoveredDevices.Remove(devInfodisplay);
                        break;
                    }
                }

                _discoveryRWLock.ExitWriteLock();
            });
        }

        private async void OnDeviceUpdated(DeviceWatcher deviceWatcher, DeviceInformationUpdate deviceInfoUpdate)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                _discoveryRWLock.EnterWriteLock();

                for (int idx = 0; idx < _discoveredDevices.Count; idx++)
                {
                    DiscoveredDevice devInfodisplay = _discoveredDevices[idx];
                    if (devInfodisplay.DeviceInfo.Id == deviceInfoUpdate.Id)
                    {
                        devInfodisplay.DeviceInfo.Update(deviceInfoUpdate);
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
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
        }

        private void btnIe_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (lvDiscoveredDevices.SelectedItems.Count == 0)
            {
                rootPage.NotifyUser("No device selected, please select one.", NotifyType.ErrorMessage);
                return;
            }

            var selectedDevices = lvDiscoveredDevices.SelectedItems;

            foreach (DiscoveredDevice discoveredDevice in selectedDevices)
            {
                try
                {
                    var iECollection = WiFiDirectInformationElement.CreateFromDeviceInformation(discoveredDevice.DeviceInfo);

                    if (iECollection.Count == 0)
                    {
                        rootPage.NotifyUser("No Information Elements found", NotifyType.StatusMessage);
                        return;
                    }

                    StringWriter strIeOutput = new StringWriter();
                    String strIe = "N/A";

                    foreach (WiFiDirectInformationElement ie in iECollection)
                    {
                        Byte[] bOui = ie.Oui.ToArray();

                        if (bOui.SequenceEqual(Globals.MsftOui))
                        {
                            strIeOutput.Write("Microsoft IE: ");
                        }
                        else if (bOui.SequenceEqual(Globals.WfaOui))
                        {
                            strIeOutput.Write("WFA IE: ");
                        }
                        else if (bOui.SequenceEqual(Globals.CustomOui))
                        {
                            strIeOutput.Write("Custom IE: ");

                            DataReader dataReader = Windows.Storage.Streams.DataReader.FromBuffer(ie.Value);
                            dataReader.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
                            dataReader.ByteOrder = ByteOrder.LittleEndian;

                            // Read the string.
                            strIe = dataReader.ReadString(dataReader.ReadUInt32());
                        }
                        else
                        {
                            strIeOutput.Write("Unknown IE: ");
                        }

                        strIeOutput.WriteLine(String.Format("OUI Type: {0} OUI: 0x{1} 0x{2} 0x{3} IE Data: {4}\n",
                                              Convert.ToString(ie.OuiType), Convert.ToString(bOui[0], 16), Convert.ToString(bOui[1], 16),
                                              Convert.ToString(bOui[2], 16), strIe));

                        strIe = "N/A";
                    }

                    rootPage.NotifyUser(strIeOutput.ToString(), NotifyType.StatusMessage);
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("No Information element found: " + ex.Message, NotifyType.ErrorMessage);
                }
            }
        }

        private async void btnFromId_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (lvDiscoveredDevices.SelectedItems.Count == 0)
            {
                rootPage.NotifyUser("No device selected, please select atleast one.", NotifyType.ErrorMessage);
                return;
            }

            var selectedDevices = lvDiscoveredDevices.SelectedItems;

            foreach (DiscoveredDevice discoveredDevice in selectedDevices)
            {
                rootPage.NotifyUser("Connecting to " + discoveredDevice.DeviceInfo.Name + "...", NotifyType.StatusMessage);

                try
                {
                    WiFiDirectConnectionParameters connectionParams = new WiFiDirectConnectionParameters();
                    connectionParams.GroupOwnerIntent = Convert.ToInt16(txtGOIntent.Text);

                    _cancellationTokenSource = new CancellationTokenSource();

                    // IMPORTANT: FromIdAsync needs to be called from the UI thread
                    var wfdDevice = await WiFiDirectDevice.FromIdAsync(discoveredDevice.DeviceInfo.Id, connectionParams).AsTask(_cancellationTokenSource.Token);

                    // Register for the ConnectionStatusChanged event handler
                    wfdDevice.ConnectionStatusChanged += OnConnectionStatusChanged;

                    var endpointPairs = wfdDevice.GetConnectionEndpointPairs();

                    rootPage.NotifyUser("Devices connected on L2 layer, connecting to IP Address: " + endpointPairs[0].RemoteHostName +
                                        " Port: " + Globals.strServerPort, NotifyType.StatusMessage);

                    // Wait for server to start listening on a socket
                    await Task.Delay(2000);

                    // Connect to Advertiser on L4 layer
                    StreamSocket clientSocket = new StreamSocket();
                    await clientSocket.ConnectAsync(endpointPairs[0].RemoteHostName, Globals.strServerPort);

                    SocketReaderWriter socketRW = new SocketReaderWriter(clientSocket, rootPage);

                    string sessionId = "Session: " + Path.GetRandomFileName();
                    ConnectedDevice connectedDevice = new ConnectedDevice(sessionId, wfdDevice, socketRW);
                    _connectedDevices.Add(connectedDevice);

                    socketRW.ReadMessage();
                    socketRW.WriteMessage(sessionId);

                    rootPage.NotifyUser("Connected with remote side on L4 layer", NotifyType.StatusMessage);
                }
                catch (TaskCanceledException)
                {
                    rootPage.NotifyUser("FromIdAsync was canceled by user", NotifyType.ErrorMessage);
                }
                catch (Exception ex)
                {
                    rootPage.NotifyUser("Connect operation threw an exception: " + ex.Message, NotifyType.ErrorMessage);
                }

                _cancellationTokenSource = null;
            }
        }

        private void OnConnectionStatusChanged(WiFiDirectDevice sender, object arg)
        {
            rootPage.NotifyUserFromBackground("Connection status changed: " + sender.ConnectionStatus, NotifyType.StatusMessage);
        }

        private void btnSendMessage_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
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

        private void btnClose_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
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
