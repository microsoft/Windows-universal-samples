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
using Windows.Security.ExchangeActiveSyncProvisioning;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_GetDeviceInformation : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_GetDeviceInformation()
        {
            this.InitializeComponent();
        }

        private void Launch_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                EasClientDeviceInformation CurrentDeviceInfor = new EasClientDeviceInformation();
                DeviceID.Text = CurrentDeviceInfor.Id.ToString();
                OperatingSystem.Text = CurrentDeviceInfor.OperatingSystem;
                FriendlyName.Text = CurrentDeviceInfor.FriendlyName;
                SystemManufacturer.Text = CurrentDeviceInfor.SystemManufacturer;
                SystemProductName.Text = CurrentDeviceInfor.SystemProductName;
                SystemSku.Text = CurrentDeviceInfor.SystemSku;
            }
            catch (Exception Error)
            {
                rootPage.NotifyUser(Error.Message, NotifyType.ErrorMessage);
            }
        }

        private void Reset_Click(object sender, RoutedEventArgs e)
        {
            DeviceID.Text = "";
            OperatingSystem.Text = "";
            FriendlyName.Text = "";
            SystemManufacturer.Text = "";
            SystemProductName.Text = "";
            SystemSku.Text = "";
        }
    }
}
