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
using System.Threading;
using Windows.Devices.Enumeration;
using Windows.UI.Core;

namespace SDKTemplate
{
    class PosDeviceWatcher
    {
        // Underlying watcher.
        private DeviceWatcher deviceWatcher;

        // Dispatch updates to this dispatcher to keep list state consistent.
        private CoreDispatcher coreDispatcher;

        /// <summary>
        /// Create a PosDeviceWatcher.
        /// </summary>
        /// <param name="deviceSelector">Device Selector string to use with DeviceWatcher</param>
        /// <param name="dispatcher">Associated UI element's Dispatcher so that updates to this.FoundDeviceList are done safely.</param>
        public PosDeviceWatcher(string deviceSelector, CoreDispatcher dispatcher)
        {
            FoundDeviceList = new ObservableCollection<DeviceInformation>();
            coreDispatcher = dispatcher;

            deviceWatcher = DeviceInformation.CreateWatcher(deviceSelector);
            deviceWatcher.Added += DeviceWatcher_Added;
            deviceWatcher.Removed += DeviceWatcher_Removed;
            deviceWatcher.Updated += DeviceWatcher_Updated;
        }

        // Updated list of found devices
        public ObservableCollection<DeviceInformation> FoundDeviceList
        {
            get;
            private set;
        }

        public void Start()
        {
            deviceWatcher.Start();
        }

        public void Stop()
        {
            deviceWatcher.Added -= DeviceWatcher_Added;
            deviceWatcher.Removed -= DeviceWatcher_Removed;
            deviceWatcher.Updated -= DeviceWatcher_Updated;

            deviceWatcher.Stop();
        }

        private async void DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation args)
        {
            await coreDispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    FoundDeviceList.Add(args);
                });
        }

        private async void DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            await coreDispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    for (int index = 0; index < FoundDeviceList.Count; ++index)
                    {
                        if (FoundDeviceList[index].Id == args.Id)
                        {
                            FoundDeviceList.RemoveAt(index);
                            break;
                        }
                    }
                });
        }

        private async void DeviceWatcher_Updated(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            await coreDispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                () =>
                {
                    for (int index = 0; index < FoundDeviceList.Count; ++index)
                    {
                        if (FoundDeviceList[index].Id == args.Id)
                        {
                            FoundDeviceList[index].Update(args);
                            break;
                        }
                    }
                });
        }
    }
}
