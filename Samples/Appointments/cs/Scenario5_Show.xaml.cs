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
using System;

namespace SDKTemplate
{
    public sealed partial class Scenario5_Show : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario5_Show()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Show the default appointment provider at a point in time 24 hours from now.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Show_Click(object sender, RoutedEventArgs e)
        {
            var dateToShow = DateTime.Now.AddDays(1);
            var duration = TimeSpan.FromHours(1);
            await Windows.ApplicationModel.Appointments.AppointmentManager.ShowTimeFrameAsync(dateToShow, duration);
            rootPage.NotifyUser("The default appointments provider should have appeared on screen.", NotifyType.StatusMessage);
        }
    }
}
