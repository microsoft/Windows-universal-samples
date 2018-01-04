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
using System.Collections.ObjectModel;
using System.Linq;
using Windows.ApplicationModel;
using Windows.Devices.Custom;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Documents;

namespace SDKTemplate
{
    public class DeviceListEntry
    {
        /// <summary>
        /// The DeviceInformation object (device interface) for this Fx2 device
        /// </summary>
        public Windows.Devices.Enumeration.DeviceInformation Device;

        /// <summary>
        /// The device interface path
        /// </summary>
        public string Id => Device.Id;

        /// <summary>
        /// The device's instance ID
        /// </summary>
        public string InstanceId => (string)Device.Properties["System.Devices.DeviceInstanceId"];

        /// <summary>
        /// Has the device been found during the current enumeration
        /// </summary>
        public bool Matched = true;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="DeviceInterface">The device interface for this entry</param>
        public DeviceListEntry(Windows.Devices.Enumeration.DeviceInformation DeviceInterface)
        {
            this.Device = DeviceInterface;
        }
    }

    class DeviceList
    {
        public static DeviceList Current {get; private set;}

        static DeviceList()
        {
            DeviceList.Current = new DeviceList();
            DeviceList.Current.InitDeviceWatcher();
            
            // Register for app suspend/resume handlers
            App.Current.Suspending += new SuspendingEventHandler(DeviceList.Current.SuspendDeviceWatcher);
            App.Current.Resuming += DeviceList.Current.ResumeDeviceWatcher;
        }

        /// <summary>
        /// List of Fx2 devices currently attached to the system.  This
        /// list can be updated by the PNP watcher.
        /// </summary>
        public ObservableCollection<DeviceListEntry> Fx2Devices => m_Fx2Devices;

        /// <summary>
        /// The currently opened Fx2 device
        /// </summary>
        private CustomDevice m_Fx2Device;
        private string m_Fx2DeviceId;

        public CustomDevice GetSelectedDevice()
        {
            return m_Fx2Device;
        }

        public string GetSelectedDeviceId()
        {
            return m_Fx2DeviceId;
        }

        public void SetSelectedDevice(string Id, CustomDevice Device)
        {
            if ((m_Fx2Device != null) && (this.DeviceClosing != null)) 
            {
                this.DeviceClosing(this, new EventArgs());
            }
            m_Fx2Device = Device;
            m_Fx2DeviceId = Id;
        }

        public void ClearSelectedDevice()
        {
            SetSelectedDevice(null, null);
        }

        /// <summary>
        /// Flag to keep track of whether the device watcher has been started.
        /// </summary>
        public bool WatcherStarted = false;

        public void StartFx2Watcher()
        {
            MainPage.Current.NotifyUser("starting device watcher", NotifyType.StatusMessage);
            foreach (var entry in m_Fx2Devices)
            {
                entry.Matched = false;
            }

            WatcherStarted = true;
            m_Fx2Watcher.Start();

            return;
        }

        public void StopFx2Watcher()
        {
            MainPage.Current.NotifyUser("stopping fx2 watcher", NotifyType.StatusMessage);
            m_Fx2Watcher.Stop();
            WatcherStarted = false;
        }

        public static void CreateBooleanTable(
            InlineCollection Table,
            bool[] NewValues,
            bool[] OldValues,
            string IndexTitle,
            string ValueTitle,
            string TrueValue,
            string FalseValue
            )
        {
            Table.Clear();

            for (var i = 0; i < NewValues.Length; i += 1)
            {
                var line = new Span();
                var block = new Run();
                block.Text = (i + 1).ToString("###");
                line.Inlines.Add(block);

                block = new Run();
                block.Text = "    ";
                line.Inlines.Add(block);

                block = new Run();
                block.Text = NewValues[i] ? TrueValue : FalseValue;

                if ((OldValues != null) && (OldValues[i] != NewValues[i]))
                {
                    var bold = new Bold();
                    bold.Inlines.Add(block);
                    line.Inlines.Add(bold);
                }
                else
                {
                    line.Inlines.Add(block);
                }

                line.Inlines.Add(new LineBreak());

                Table.Add(line);
            }
        }

        public event EventHandler DeviceClosing;

        //
        // private internal state
        //

        /// <summary>
        /// The device watcher that we setup to look for Fx2 devices
        /// </summary>
        Windows.Devices.Enumeration.DeviceWatcher m_Fx2Watcher = null;

        /// <summary>
        /// Internal list of devices.
        /// </summary>
        ObservableCollection<DeviceListEntry> m_Fx2Devices = new ObservableCollection<DeviceListEntry>();

        void InitDeviceWatcher()
        {
            // Define the selector to enumerate all of the fx2 device interface class instances
            var selector = CustomDevice.GetDeviceSelector(Fx2Driver.DeviceInterfaceGuid);

            // Create a device watcher to look for instances of the fx2 device interface
            m_Fx2Watcher = Windows.Devices.Enumeration.DeviceInformation.CreateWatcher(
                            selector,
                            new string[] { "System.Devices.DeviceInstanceId" }
                            );

            m_Fx2Watcher.Added += OnFx2Added;
            m_Fx2Watcher.Removed += OnFx2Removed;
            m_Fx2Watcher.EnumerationCompleted += OnFx2EnumerationComplete;
        }

        /// <summary>
        /// Event handler for arrival of Fx2 devices
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="devInterface">The device interface which was added</param>
        private async void OnFx2Added(Windows.Devices.Enumeration.DeviceWatcher sender, Windows.Devices.Enumeration.DeviceInformation devInterface)
        {
            await
            MainPage.Current.Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    MainPage.Current.NotifyUser(String.Format("OnFx2Added: {0}", devInterface.Id), NotifyType.StatusMessage);

                    // search the device list for a device with a matching interface ID
                    DeviceListEntry match = FindInList(devInterface.Id);

                    // If we found a match then mark it as verified and return
                    if (match != null)
                    {
                        match.Matched = true;
                        return;
                    }


                    // Create a new element for this device interface, and queue up the query of its
                    // device information
                    match = new DeviceListEntry(devInterface);

                    // Add the new element to the end of the list of devices
                    m_Fx2Devices.Add(match);
                }
            );
        }

        /// <summary>
        /// Event handler for the removal of an Fx2 device
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="devInformation">The device interface that was removed</param>
        private async void OnFx2Removed(Windows.Devices.Enumeration.DeviceWatcher sender, Windows.Devices.Enumeration.DeviceInformationUpdate devInformation)
        {
            await
            MainPage.Current.Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    var deviceId = devInformation.Id;
                    MainPage.Current.NotifyUser(String.Format("OnFx2Removed: {0}", deviceId), NotifyType.StatusMessage);

                    // Search the list of devices for one with a matching ID
                    var match = FindInList(deviceId);
                    if (match != null)
                    {
                        // Remove the matched item
                        MainPage.Current.NotifyUser(String.Format("OnFx2Removed: {0} removed", deviceId), NotifyType.StatusMessage);
                        m_Fx2Devices.Remove(match);
                    }
                }
            );
        }

        /// <summary>
        /// Event handler for the end of an enumeration/reenumeration started
        /// by calling Start on the device watcher.  This culls out any entries
        /// in the list which are no longer matched.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void OnFx2EnumerationComplete(Windows.Devices.Enumeration.DeviceWatcher sender, object args)
        {
            await
            MainPage.Current.Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    MainPage.Current.NotifyUser(String.Format("OnFx2EnumerationComplete"), NotifyType.StatusMessage);

                    DeviceListEntry removedDevice;

                    while ((removedDevice = m_Fx2Devices.FirstOrDefault(e => !e.Matched)) != null)
                    {
                        MainPage.Current.NotifyUser("OnFx2EnumerationComplete: Removing missing device " + removedDevice.Id, NotifyType.StatusMessage);
                        m_Fx2Devices.Remove(removedDevice);
                    }
                }
            );
        }

        private bool m_WatcherSuspended = false;

        private DeviceListEntry FindInList(string id)
        {
            return m_Fx2Devices.FirstOrDefault(entry => entry.Id == id);
        }

        private void SuspendDeviceWatcher(object sender, SuspendingEventArgs e)
        {
            if (WatcherStarted)
            {
                m_WatcherSuspended = WatcherStarted;
                StopFx2Watcher();
            }
        }

        private void ResumeDeviceWatcher(object sender, object e)
        {
            if (m_WatcherSuspended)
            {
                StartFx2Watcher();
                m_WatcherSuspended = false;
            }
        }
    }
}
