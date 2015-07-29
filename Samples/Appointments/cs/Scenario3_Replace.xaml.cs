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
    public sealed partial class Scenario3_Replace : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3_Replace()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Replace an appointment on the user's calendar using the default appointments provider app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Replace_Click(object sender, RoutedEventArgs e)
        {
            // The appointment id argument for ReplaceAppointmentAsync is typically retrieved from AddAppointmentAsync and stored in app data.
            String appointmentIdOfAppointmentToReplace = AppointmentIdTextBox.Text;

            if (String.IsNullOrEmpty(appointmentIdOfAppointmentToReplace))
            {
                rootPage.NotifyUser("The appointment id cannot be empty", NotifyType.ErrorMessage);
            }
            else
            {
                // The Appointment argument for ReplaceAppointmentAsync should contain all of the Appointment's properties including those that may have changed.
                var appointment = new Windows.ApplicationModel.Appointments.Appointment();

                // Get the selection rect of the button pressed to replace this appointment
                var rect = MainPage.GetElementRect(sender as FrameworkElement);

                // ReplaceAppointmentAsync returns an updated appointment id when the appointment was successfully replaced.
                // The updated id may or may not be the same as the original one retrieved from AddAppointmentAsync.
                // An optional instance start time can be provided to indicate that a specific instance on that date should be replaced
                // in the case of a recurring appointment.
                // If the appointment id returned is an empty string, that indicates that the appointment was not replaced.
                String updatedAppointmentId;
                if (InstanceStartDateCheckBox.IsChecked.Value)
                {
                    // Replace a specific instance starting on the date provided.
                    var instanceStartDate = InstanceStartDateDatePicker.Date;
                    updatedAppointmentId = await Windows.ApplicationModel.Appointments.AppointmentManager.ShowReplaceAppointmentAsync(appointmentIdOfAppointmentToReplace, appointment, rect, Windows.UI.Popups.Placement.Default, instanceStartDate);
                }
                else
                {
                    // Replace an appointment that occurs only once or in the case of a recurring appointment, replace the entire series.
                    updatedAppointmentId = await Windows.ApplicationModel.Appointments.AppointmentManager.ShowReplaceAppointmentAsync(appointmentIdOfAppointmentToReplace, appointment, rect, Windows.UI.Popups.Placement.Default);
                }

                if (updatedAppointmentId != String.Empty)
                {
                    rootPage.NotifyUser("Updated Appointment Id: " + updatedAppointmentId, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("Appointment not replaced.", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
