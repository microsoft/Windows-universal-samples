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
using Windows.ApplicationModel.Core;
using Windows.Devices.Enumeration;
using Windows.Devices.PointOfService;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_SelectDisplay : Page
    {
        private MainPage rootPage = MainPage.Current;
        private DeviceWatcher watcher;
                
        public Scenario1_SelectDisplay()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            RestartWatcher();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            StopWatcher();
        }

        private void RestartWatcher()
        {
            StopWatcher();

            // Clear any old LineDisplay devices from the list box.
            DisplaysListBox.Items.Clear();

            // Enumerate the LineDisplay devices and put them in our list box.
            watcher = DeviceInformation.CreateWatcher(LineDisplay.GetDeviceSelector(PosConnectionTypes.All));
            watcher.Added += Watcher_Added;
            watcher.EnumerationCompleted += Watcher_EnumerationCompleted;
            watcher.Start();
        }

        private void StopWatcher()
        {
            if (watcher != null && (watcher.Status == DeviceWatcherStatus.Started))
            {
                watcher.Stop();
            }
            watcher = null;
        }

        private async void Watcher_Added(DeviceWatcher sender, DeviceInformation deviceInfo)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                var item = new ListBoxItem();
                item.Content = deviceInfo.Name;
                item.Tag = deviceInfo.Id;
                DisplaysListBox.Items.Add(item);
            });
        }

        private void Watcher_EnumerationCompleted(DeviceWatcher sender, object e)
        {
            sender.Stop();
        }

        private async void SelectButton_Click(object sender, RoutedEventArgs e)
        {
            var item = (ListBoxItem)DisplaysListBox.SelectedValue;
            SelectButton.IsEnabled = false;

            var name = (string)item.Content;
            var deviceId = (string)item.Tag;
            using (ClaimedLineDisplay lineDisplay = await ClaimedLineDisplay.FromIdAsync(deviceId))
                {
                    if (lineDisplay != null)
                    {
                        await lineDisplay.DefaultWindow.TryClearTextAsync();
                        rootPage.NotifyUser($"Selected: {name}", NotifyType.StatusMessage);

                        // Save this device ID for other scenarios.
                        rootPage.LineDisplayId = deviceId;
                    }
                    else
                    {
                        rootPage.NotifyUser("Unable to claim the Line Display", NotifyType.ErrorMessage);
                    }

                SelectButton.IsEnabled = true;
            }
        }

        private void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            RestartWatcher();
        }

        // Helpers that are used by XAML binding to enable and disable buttons.
        public bool IsNonNull(object item) => item != null;
    }
}
