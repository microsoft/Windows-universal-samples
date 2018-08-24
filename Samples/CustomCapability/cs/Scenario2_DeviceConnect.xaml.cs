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
using System.Threading.Tasks;
using Windows.Devices.Custom;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class DeviceConnect
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public DeviceConnect()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            UpdateStartStopButtons();
            listSource.Source = DeviceList.Current.Fx2Devices;
        }

        private void deviceConnectStart_Click(object sender, RoutedEventArgs e)
        {
            DeviceList.Current.StartFx2Watcher();
            UpdateStartStopButtons();
        }

        private void deviceConnectStop_Click(object sender, RoutedEventArgs e)
        {
            DeviceList.Current.StopFx2Watcher();
            UpdateStartStopButtons();
        }

        private async void deviceConnectDevices_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var entry = (DeviceListEntry)deviceConnectDevices.SelectedItem;
            var currentlySelectedId = DeviceList.Current.GetSelectedDeviceId();
            var newlySelectedId = entry?.Id;

            if (currentlySelectedId != null)
            {
                CloseFx2Device();
            }

            if (newlySelectedId != null)
            {
                await OpenFx2DeviceAsync(newlySelectedId);
            }
        }

        private void UpdateStartStopButtons()
        {
            this.deviceConnectStart.IsEnabled = !DeviceList.Current.WatcherStarted;
            this.deviceConnectStop.IsEnabled = DeviceList.Current.WatcherStarted;
        }

        private async Task OpenFx2DeviceAsync(string Id)
        {
            System.Diagnostics.Debug.Assert(DeviceList.Current.GetSelectedDevice() == null);

            try
            {
                var device = await CustomDevice.FromIdAsync(Id, DeviceAccessMode.ReadWrite, DeviceSharingMode.Exclusive);
                DeviceList.Current.SetSelectedDevice(Id, device);
            }
            catch (Exception e)
            {
                rootPage.NotifyUser($"Error opening Fx2 device @{Id}: {e.Message}", NotifyType.ErrorMessage);
                return;
            }
            rootPage.NotifyUser($"Fx2 {Id} opened", NotifyType.StatusMessage);
            return;
        }

        private void CloseFx2Device()
        {
            if (DeviceList.Current.GetSelectedDevice() != null)
            {
                rootPage.NotifyUser($"Fx2 {DeviceList.Current.GetSelectedDeviceId()} closed", NotifyType.StatusMessage);
                DeviceList.Current.ClearSelectedDevice();
            }
        }
    }
}
