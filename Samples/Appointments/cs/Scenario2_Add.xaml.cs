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
    public sealed partial class Scenario2_Add : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario2_Add()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Adds an appointment to the default appointment provider app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Add_Click(object sender, RoutedEventArgs e)
        {
            // Create an Appointment that should be added the user's appointments provider app.
            var appointment = new Windows.ApplicationModel.Appointments.Appointment();

            // Get the selection rect of the button pressed to add this appointment
            var rect = MainPage.GetElementRect(sender as FrameworkElement);

            // ShowAddAppointmentAsync returns an appointment id if the appointment given was added to the user's calendar.
            // This value should be stored in app data and roamed so that the appointment can be replaced or removed in the future.
            // An empty string return value indicates that the user canceled the operation before the appointment was added.
            String appointmentId = await Windows.ApplicationModel.Appointments.AppointmentManager.ShowAddAppointmentAsync(appointment, rect, Windows.UI.Popups.Placement.Default);
            if (appointmentId != String.Empty)
            {
                rootPage.NotifyUser("Appointment Id: " + appointmentId, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Appointment not added.", NotifyType.ErrorMessage);
            }
        }
    }
}
