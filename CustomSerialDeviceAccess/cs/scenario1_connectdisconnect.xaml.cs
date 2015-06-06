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
using SDKTemplate;

using System;
using System.IO;
using System.Collections.Generic;
using System.Collections.ObjectModel;

using Windows.ApplicationModel;
using Windows.Foundation;

using Windows.UI.Core;

using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using CustomSerialDeviceAccess;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace CustomSerialDeviceAccess
{
    /// <summary>
    /// Demonstrates how to connect to a Serial Device and respond to device disconnects, app suspending and resuming events.
    /// 
    /// To use this sample with your device:
    /// 1) Include the device in the Package.appxmanifest. For instructions, see Package.appxmanifest documentation.
    /// 2) Create a DeviceWatcher object for your device. See the InitializeDeviceWatcher method in this sample.
    /// </summary>
    public sealed partial class Scenario1_ConnectDisconnect : Page
    {
        private const String ButtonNameDisconnectFromDevice = "Disconnect from device";
        private const String ButtonNameDisableReconnectToDevice = "Do not automatically reconnect to device that was just closed";

        // Pointer back to the main page
        private MainPage rootPage = MainPage.Current;

        private SuspendingEventHandler appSuspendEventHandler;
        private EventHandler<Object> appResumeEventHandler;

        private ObservableCollection<DeviceListEntry> listOfDevices;

        private Dictionary<DeviceWatcher, String> mapDeviceWatchersToDeviceSelector;
        private Boolean watchersSuspended;
        private Boolean watchersStarted;

        // Has all the devices enumerated by the device watcher?
        private Boolean isAllDevicesEnumerated;

        public Scenario1_ConnectDisconnect()
        {
            this.InitializeComponent();

            listOfDevices = new ObservableCollection<DeviceListEntry>();

            mapDeviceWatchersToDeviceSelector = new Dictionary<DeviceWatcher, String>();
            watchersStarted = false;
            watchersSuspended = false;

            isAllDevicesEnumerated = false;
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// 
        /// Create the DeviceWatcher objects when the user navigates to this page so the UI list of devices is populated.
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs eventArgs)
        {
            // If we are connected to the device or planning to reconnect, we should disable the list of devices
            // to prevent the user from opening a device without explicitly closing or disabling the auto reconnect
            if (EventHandlerForDevice.Current.IsDeviceConnected
                || (EventHandlerForDevice.Current.IsEnabledAutoReconnect
                && EventHandlerForDevice.Current.DeviceInformation != null))
            {
                UpdateConnectDisconnectButtonsAndList(false);

                // These notifications will occur if we are waiting to reconnect to device when we start the page
                EventHandlerForDevice.Current.OnDeviceConnected = this.OnDeviceConnected;
                EventHandlerForDevice.Current.OnDeviceClose = this.OnDeviceClosing;
            }
            else
            {
                UpdateConnectDisconnectButtonsAndList(true);
            }

            // Begin watching out for events
            StartHandlingAppEvents();

            // Initialize the desired device watchers so that we can watch for when devices are connected/removed
            InitializeDeviceWatchers();
            StartDeviceWatchers();

            DeviceListSource.Source = listOfDevices;
        }

        /// <summary>
        /// Unregister from App events and DeviceWatcher events because this page will be unloaded.
        /// </summary>
        /// <param name="eventArgs"></param>
        protected override void OnNavigatedFrom(NavigationEventArgs eventArgs)
        {
            StopDeviceWatchers();
            StopHandlingAppEvents();

            // We no longer care about the device being connected
            EventHandlerForDevice.Current.OnDeviceConnected = null;
            EventHandlerForDevice.Current.OnDeviceClose = null;
        }

        private async void ConnectToDevice_Click(Object sender, RoutedEventArgs eventArgs)
        {
            var selection = ConnectDevices.SelectedItems;
            DeviceListEntry entry = null;

            if (selection.Count > 0)
            {
                var obj = selection[0];
                entry = (DeviceListEntry)obj;

                if (entry != null)
                {
                    // Create an EventHandlerForDevice to watch for the device we are connecting to
                    EventHandlerForDevice.CreateNewEventHandlerForDevice();

                    // Get notified when the device was successfully connected to or about to be closed
                    EventHandlerForDevice.Current.OnDeviceConnected = this.OnDeviceConnected;
                    EventHandlerForDevice.Current.OnDeviceClose = this.OnDeviceClosing;

                    // It is important that the FromIdAsync call is made on the UI thread because the consent prompt, when present,
                    // can only be displayed on the UI thread. Since this method is invoked by the UI, we are already in the UI thread.
                    Boolean openSuccess = await EventHandlerForDevice.Current.OpenDeviceAsync(entry.DeviceInformation, entry.DeviceSelector);

                    // Disable connect button if we connected to the device
                    UpdateConnectDisconnectButtonsAndList(!openSuccess);
                }
            }
        }

        private void DisconnectFromDevice_Click(Object sender, RoutedEventArgs eventArgs)
        {
            var selection = ConnectDevices.SelectedItems;
            DeviceListEntry entry = null;

            // Prevent auto reconnect because we are voluntarily closing it
            // Re-enable the ConnectDevice list and ConnectToDevice button if the connected/opened device was removed.
            EventHandlerForDevice.Current.IsEnabledAutoReconnect = false;

            if (selection.Count > 0)
            {
                var obj = selection[0];
                entry = (DeviceListEntry)obj;

                if (entry != null)
                {
                    EventHandlerForDevice.Current.CloseDevice();
                }
            }

            UpdateConnectDisconnectButtonsAndList(true);
        }

        /// <summary>
        /// Initialize device watchers to watch for the Serial Devices.
        ///
        /// GetDeviceSelector return an AQS string that can be passed directly into DeviceWatcher.createWatcher() or  DeviceInformation.createFromIdAsync(). 
        ///
        /// In this sample, a DeviceWatcher will be used to watch for devices because we can detect surprise device removals.
        /// </summary>
        private void InitializeDeviceWatchers()
        {
            
            // Target all Serial Devices present on the system
            var deviceSelector = SerialDevice.GetDeviceSelector();

            // Other variations of GetDeviceSelector() usage are commented for reference
            //
            // Target a specific USB Serial Device using its VID and PID (here Arduino VID/PID is used)
            // var deviceSelector = SerialDevice.GetDeviceSelectorFromUsbVidPid(0x2341, 0x0043);
            //
            // Target a specific Serial Device by its COM PORT Name - "COM3"
            // var deviceSelector = SerialDevice.GetDeviceSelector("COM3");
            //
            // Target a specific UART based Serial Device by its COM PORT Name (usually defined in ACPI) - "UART1"
            // var deviceSelector = SerialDevice.GetDeviceSelector("UART1");
            //

            // Create a device watcher to look for instances of the Serial Device that match the device selector
            // used earlier.
            
            var deviceWatcher = DeviceInformation.CreateWatcher(deviceSelector);

            // Allow the EventHandlerForDevice to handle device watcher events that relates or effects our device (i.e. device removal, addition, app suspension/resume)
            AddDeviceWatcher(deviceWatcher, deviceSelector);
        }

        private void StartHandlingAppEvents()
        {
            appSuspendEventHandler = new SuspendingEventHandler(this.OnAppSuspension);
            appResumeEventHandler = new EventHandler<Object>(this.OnAppResume);

            // This event is raised when the app is exited and when the app is suspended
            App.Current.Suspending += appSuspendEventHandler;

            App.Current.Resuming += appResumeEventHandler;
        }

        private void StopHandlingAppEvents()
        {
            // This event is raised when the app is exited and when the app is suspended
            App.Current.Suspending -= appSuspendEventHandler;

            App.Current.Resuming -= appResumeEventHandler;
        }

        /// <summary>
        /// Registers for Added, Removed, and Enumerated events on the provided deviceWatcher before adding it to an internal list.
        /// </summary>
        /// <param name="deviceWatcher"></param>
        /// <param name="deviceSelector">The AQS used to create the device watcher</param>
        private void AddDeviceWatcher(DeviceWatcher deviceWatcher, String deviceSelector)
        {
            deviceWatcher.Added += new TypedEventHandler<DeviceWatcher, DeviceInformation>(this.OnDeviceAdded);
            deviceWatcher.Removed += new TypedEventHandler<DeviceWatcher, DeviceInformationUpdate>(this.OnDeviceRemoved);
            deviceWatcher.EnumerationCompleted += new TypedEventHandler<DeviceWatcher, Object>(this.OnDeviceEnumerationComplete);

            mapDeviceWatchersToDeviceSelector.Add(deviceWatcher, deviceSelector);
        }

        /// <summary>
        /// Starts all device watchers including ones that have been individually stopped.
        /// </summary>
        private void StartDeviceWatchers()
        {
            // Start all device watchers
            watchersStarted = true;
            isAllDevicesEnumerated = false;

            foreach (DeviceWatcher deviceWatcher in mapDeviceWatchersToDeviceSelector.Keys)
            {
                if ((deviceWatcher.Status != DeviceWatcherStatus.Started)
                    && (deviceWatcher.Status != DeviceWatcherStatus.EnumerationCompleted))
                {
                    deviceWatcher.Start();
                }
            }
        }

        /// <summary>
        /// Stops all device watchers.
        /// </summary>
        private void StopDeviceWatchers()
        {
            // Stop all device watchers
            foreach (DeviceWatcher deviceWatcher in mapDeviceWatchersToDeviceSelector.Keys)
            {
                if ((deviceWatcher.Status == DeviceWatcherStatus.Started)
                    || (deviceWatcher.Status == DeviceWatcherStatus.EnumerationCompleted))
                {
                    deviceWatcher.Stop();
                }
            }

            // Clear the list of devices so we don't have potentially disconnected devices around
            ClearDeviceEntries();

            watchersStarted = false;
        }

        /// <summary>
        /// Creates a DeviceListEntry for a device and adds it to the list of devices in the UI
        /// </summary>
        /// <param name="deviceInformation">DeviceInformation on the device to be added to the list</param>
        /// <param name="deviceSelector">The AQS used to find this device</param>
        private void AddDeviceToList(DeviceInformation deviceInformation, String deviceSelector)
        {
            // search the device list for a device with a matching interface ID
            var match = FindDevice(deviceInformation.Id);

            // Add the device if it's new
            if (match == null)
            {
                // Create a new element for this device interface, and queue up the query of its
                // device information
                match = new DeviceListEntry(deviceInformation, deviceSelector);

                // Add the new element to the end of the list of devices
                listOfDevices.Add(match);
            }
        }

        private void RemoveDeviceFromList(String deviceId)
        {
            // Removes the device entry from the interal list; therefore the UI
            var deviceEntry = FindDevice(deviceId);

            listOfDevices.Remove(deviceEntry);
        }

        private void ClearDeviceEntries()
        {
            listOfDevices.Clear();
        }

        /// <summary>
        /// Searches through the existing list of devices for the first DeviceListEntry that has
        /// the specified device Id.
        /// </summary>
        /// <param name="deviceId">Id of the device that is being searched for</param>
        /// <returns>DeviceListEntry that has the provided Id; else a nullptr</returns>
        private DeviceListEntry FindDevice(String deviceId)
        {
            if (deviceId != null)
            {
                foreach (DeviceListEntry entry in listOfDevices)
                {
                    if (entry.DeviceInformation.Id == deviceId)
                    {
                        return entry;
                    }
                }
            }

            return null;
        }

        /// <summary>
        /// We must stop the DeviceWatchers because device watchers will continue to raise events even if
        /// the app is in suspension, which is not desired (drains battery). We resume the device watcher once the app resumes again.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="eventArgs"></param>
        private void OnAppSuspension(Object sender, SuspendingEventArgs args)
        {
            if (watchersStarted)
            {
                watchersSuspended = true;
                StopDeviceWatchers();
            }
            else
            {
                watchersSuspended = false;
            }
        }

        /// <summary>
        /// See OnAppSuspension for why we are starting the device watchers again
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void OnAppResume(Object sender, Object args)
        {
            if (watchersSuspended)
            {
                watchersSuspended = false;
                StartDeviceWatchers();
            }
        }

        /// <summary>
        /// We will remove the device from the UI
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="deviceInformationUpdate"></param>
        private async void OnDeviceRemoved(DeviceWatcher sender, DeviceInformationUpdate deviceInformationUpdate)
        {
            await rootPage.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    rootPage.NotifyUser("Device removed - " + deviceInformationUpdate.Id, NotifyType.StatusMessage);

                    RemoveDeviceFromList(deviceInformationUpdate.Id);
                }));
        }

        /// <summary>
        /// This function will add the device to the listOfDevices so that it shows up in the UI
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="deviceInformation"></param>
        private async void OnDeviceAdded(DeviceWatcher sender, DeviceInformation deviceInformation)
        {
            await rootPage.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    rootPage.NotifyUser("Device added - " + deviceInformation.Id, NotifyType.StatusMessage);

                    AddDeviceToList(deviceInformation, mapDeviceWatchersToDeviceSelector[sender]);
                }));
        }

        /// <summary>
        /// Notify the UI whether or not we are connected to a device
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void OnDeviceEnumerationComplete(DeviceWatcher sender, Object args)
        {
            await rootPage.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    isAllDevicesEnumerated = true;

                    // If we finished enumerating devices and the device has not been connected yet, the OnDeviceConnected method
                    // is responsible for selecting the device in the device list (UI); otherwise, this method does that.
                    if (EventHandlerForDevice.Current.IsDeviceConnected)
                    {
                        SelectDeviceInList(EventHandlerForDevice.Current.DeviceInformation.Id);

                        ButtonDisconnectFromDevice.Content = ButtonNameDisconnectFromDevice;

                        if (EventHandlerForDevice.Current.Device.PortName != "")
                        {
                            rootPage.NotifyUser("Connected to - " +
                                                EventHandlerForDevice.Current.Device.PortName +
                                                " - " +
                                                EventHandlerForDevice.Current.DeviceInformation.Id, NotifyType.StatusMessage);
                        } else 
                        {
                            rootPage.NotifyUser("Connected to - " +
                                                EventHandlerForDevice.Current.DeviceInformation.Id, NotifyType.StatusMessage);
                        }
                    }
                    else if (EventHandlerForDevice.Current.IsEnabledAutoReconnect && EventHandlerForDevice.Current.DeviceInformation != null)
                    {
                        // We will be reconnecting to a device
                        ButtonDisconnectFromDevice.Content = ButtonNameDisableReconnectToDevice;

                        rootPage.NotifyUser("Waiting to reconnect to device -  " + EventHandlerForDevice.Current.DeviceInformation.Id, NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("No device is currently connected", NotifyType.StatusMessage);
                    }
                }));
        }

        /// <summary>
        /// If all the devices have been enumerated, select the device in the list we connected to. Otherwise let the EnumerationComplete event
        /// from the device watcher handle the device selection
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="deviceInformation"></param>
        private void OnDeviceConnected(EventHandlerForDevice sender, DeviceInformation deviceInformation)
        {
            // Find and select our connected device
            if (isAllDevicesEnumerated)
            {
                SelectDeviceInList(EventHandlerForDevice.Current.DeviceInformation.Id);

                ButtonDisconnectFromDevice.Content = ButtonNameDisconnectFromDevice;
            }

            if (EventHandlerForDevice.Current.Device.PortName != "")
            {
                rootPage.NotifyUser("Connected to - " +
                                    EventHandlerForDevice.Current.Device.PortName +
                                    " - " +
                                    EventHandlerForDevice.Current.DeviceInformation.Id, NotifyType.StatusMessage);
            } else 
            {
                rootPage.NotifyUser("Connected to - " +
                                    EventHandlerForDevice.Current.DeviceInformation.Id, NotifyType.StatusMessage);
            }
        }

        /// <summary>
        /// The device was closed. If we will autoreconnect to the device, reflect that in the UI
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="deviceInformation"></param>
        private async void OnDeviceClosing(EventHandlerForDevice sender, DeviceInformation deviceInformation)
        {
            await rootPage.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    // We were connected to the device that was unplugged, so change the "Disconnect from device" button
                    // to "Do not reconnect to device"
                    if (ButtonDisconnectFromDevice.IsEnabled && EventHandlerForDevice.Current.IsEnabledAutoReconnect)
                    {
                        ButtonDisconnectFromDevice.Content = ButtonNameDisableReconnectToDevice;
                    }
                }));
        }

        /// <summary>
        /// Selects the item in the UI's listbox that corresponds to the provided device id. If there are no
        /// matches, we will deselect anything that is selected.
        /// </summary>
        /// <param name="deviceIdToSelect">The device id of the device to select on the list box</param>
        private void SelectDeviceInList(String deviceIdToSelect)
        {
            // Don't select anything by default.
            ConnectDevices.SelectedIndex = -1;

            for (int deviceListIndex = 0; deviceListIndex < listOfDevices.Count; deviceListIndex++)
            {
                if (listOfDevices[deviceListIndex].DeviceInformation.Id == deviceIdToSelect)
                {
                    ConnectDevices.SelectedIndex = deviceListIndex;

                    break;
                }
            }
        }

        /// <summary>
        /// When ButtonConnectToDevice is disabled, ConnectDevices list will also be disabled.
        /// </summary>
        /// <param name="enableConnectButton">The state of ButtonConnectToDevice</param>
        private void UpdateConnectDisconnectButtonsAndList(Boolean enableConnectButton)
        {
            ButtonConnectToDevice.IsEnabled = enableConnectButton;
            ButtonDisconnectFromDevice.IsEnabled = !ButtonConnectToDevice.IsEnabled;

            ConnectDevices.IsEnabled = ButtonConnectToDevice.IsEnabled;
        }
    }
}
