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
using Windows.Devices.Enumeration;
using Windows.Devices.PointOfService;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_SelectDisplay : Page
    {
        private MainPage rootPage = MainPage.Current;
                
        public Scenario1_SelectDisplay()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            // Enumerate all the LineDisplay devices and put them in our list box.
            DeviceInformationCollection deviceInfoCollection = await DeviceInformation.FindAllAsync(LineDisplay.GetDeviceSelector(PosConnectionTypes.All));
            foreach (DeviceInformation deviceInfo in deviceInfoCollection)
            {
                var item = new ListBoxItem();
                item.Content = deviceInfo.Name;
                item.Tag = deviceInfo.Id;
                DisplaysListBox.Items.Add(item);
            }
        }

        private async void SelectButton_Click(object sender, RoutedEventArgs e)
        {
            var item = (ListBoxItem)DisplaysListBox.SelectedValue;
            if (item != null)
            {
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
                }

                SelectButton.IsEnabled = true;
            }
        }
    }
}
