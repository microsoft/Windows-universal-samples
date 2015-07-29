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
    public sealed partial class Scenario6_Recurrence : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario6_Recurrence()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Creates a Recurrence object based on input fields and validates it.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Create_Click(object sender, RoutedEventArgs e)
        {
            string errorMessage = null;
            var recurrence = new Windows.ApplicationModel.Appointments.AppointmentRecurrence();

            // Unit
            switch (UnitComboBox.SelectedIndex)
            {
                case 0:
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Daily;
                    break;
                case 1:
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Weekly;
                    break;
                case 2:
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Monthly;
                    break;
                case 3:
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.MonthlyOnDay;
                    break;
                case 4:
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Yearly;
                    break;
                case 5:
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.YearlyOnDay;
                    break;
            }

            // Occurrences
            // Note: Occurrences and Until properties are mutually exclusive.
            if (OccurrencesRadioButton.IsChecked.Value)
            {
                recurrence.Occurrences = (uint)OccurrencesSlider.Value;
            }

            // Until
            // Note: Until and Occurrences properties are mutually exclusive.
            if (UntilRadioButton.IsChecked.Value)
            {
                recurrence.Until = UntilDatePicker.Date;
            }

            // Interval
            recurrence.Interval = (uint)IntervalSlider.Value;

            // Week of the month
            switch (WeekOfMonthComboBox.SelectedIndex)
            {
                case 0:
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.First;
                    break;
                case 1:
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Second;
                    break;
                case 2:
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Third;
                    break;
                case 3:
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Fourth;
                    break;
                case 4:
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Last;
                    break;
            }

            // Days of the Week
            // Note: For Weekly, MonthlyOnDay or YearlyOnDay recurrence unit values, at least one day must be specified.
            if (SundayCheckBox.IsChecked.Value) { recurrence.DaysOfWeek |= Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Sunday; }
            if (MondayCheckBox.IsChecked.Value) { recurrence.DaysOfWeek |= Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Monday; }
            if (TuesdayCheckBox.IsChecked.Value) { recurrence.DaysOfWeek |= Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Tuesday; }
            if (WednesdayCheckBox.IsChecked.Value) { recurrence.DaysOfWeek |= Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Wednesday; }
            if (ThursdayCheckBox.IsChecked.Value) { recurrence.DaysOfWeek |= Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Thursday; }
            if (FridayCheckBox.IsChecked.Value) { recurrence.DaysOfWeek |= Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Friday; }
            if (SaturdayCheckBox.IsChecked.Value) { recurrence.DaysOfWeek |= Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Saturday; }

            if (((recurrence.Unit == Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Weekly) ||
                 (recurrence.Unit == Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.MonthlyOnDay) ||
                 (recurrence.Unit == Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.YearlyOnDay)) &&
                (recurrence.DaysOfWeek == Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.None))
            {
                errorMessage = "The recurrence specified is invalid. For Weekly, MonthlyOnDay or YearlyOnDay recurrence unit values, at least one day must be specified.";
            }

            // Month of the year
            recurrence.Month = (uint)MonthSlider.Value;

            // Day of the month
            recurrence.Day = (uint)DaySlider.Value;

            if (errorMessage == null)
            {
                rootPage.NotifyUser("The recurrence specified was created successfully and is valid.", NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser(errorMessage, NotifyType.ErrorMessage);
            }
        }
    }
}
