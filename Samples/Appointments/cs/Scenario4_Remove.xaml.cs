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
    public sealed partial class Scenario4_Remove : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario4_Remove()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Removes the appointment associated with a particular appointment id string from the defaul appointment provider app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Remove_Click(object sender, RoutedEventArgs e)
        {
            // The appointment id argument for ShowRemoveAppointmentAsync is typically retrieved from AddAppointmentAsync and stored in app data.
            String appointmentId = AppointmentIdTextBox.Text;

            // The appointment id cannot be null or empty.
            if (String.IsNullOrEmpty(appointmentId))
            {
                rootPage.NotifyUser("The appointment id cannot be empty", NotifyType.ErrorMessage);
            }
            else
            {
                // Get the selection rect of the button pressed to remove this appointment
                var rect = MainPage.GetElementRect(sender as FrameworkElement);

                // ShowRemoveAppointmentAsync returns a boolean indicating whether or not the appointment related to the appointment id given was removed.
                // An optional instance start time can be provided to indicate that a specific instance on that date should be removed
                // in the case of a recurring appointment.
                bool removed;
                if (InstanceStartDateCheckBox.IsChecked.Value)
                {
                    // Remove a specific instance starting on the date provided.
                    var instanceStartDate = InstanceStartDateDatePicker.Date;
                    removed = await Windows.ApplicationModel.Appointments.AppointmentManager.ShowRemoveAppointmentAsync(appointmentId, rect, Windows.UI.Popups.Placement.Default, instanceStartDate);
                }
                else
                {
                    // Remove an appointment that occurs only once or in the case of a recurring appointment, replace the entire series.
                    removed = await Windows.ApplicationModel.Appointments.AppointmentManager.ShowRemoveAppointmentAsync(appointmentId, rect, Windows.UI.Popups.Placement.Default);
                }

                if (removed)
                {
                    rootPage.NotifyUser("Appointment removed", NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Appointment not removed", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
