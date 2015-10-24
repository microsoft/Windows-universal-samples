'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' This code is licensed under the MIT License (MIT).
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports System

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario6_Recurrence
        Inherits Page

        Private rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Creates a Recurrence object based on input fields and validates it.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub Create_Click(sender As Object, e As RoutedEventArgs)
            Dim errorMessage As String = Nothing
            Dim recurrence = New Windows.ApplicationModel.Appointments.AppointmentRecurrence()
            Select UnitComboBox.SelectedIndex
                Case 0
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Daily
                     Case 1
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Weekly
                     Case 2
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Monthly
                     Case 3
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.MonthlyOnDay
                     Case 4
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Yearly
                     Case 5
                    recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.YearlyOnDay
                     End Select

            If OccurrencesRadioButton.IsChecked.Value Then
                recurrence.Occurrences = CType(OccurrencesSlider.Value, UInteger)
            End If

            If UntilRadioButton.IsChecked.Value Then
                recurrence.Until = UntilDatePicker.Date
            End If

            recurrence.Interval = CType(IntervalSlider.Value, UInteger)
            Select WeekOfMonthComboBox.SelectedIndex
                Case 0
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.First
                     Case 1
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Second
                     Case 2
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Third
                     Case 3
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Fourth
                     Case 4
                    recurrence.WeekOfMonth = Windows.ApplicationModel.Appointments.AppointmentWeekOfMonth.Last
                     End Select

            If SundayCheckBox.IsChecked.Value Then
                recurrence.DaysOfWeek = recurrence.DaysOfWeek OrElse Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Sunday
            End If

            If MondayCheckBox.IsChecked.Value Then
                recurrence.DaysOfWeek = recurrence.DaysOfWeek OrElse Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Monday
            End If

            If TuesdayCheckBox.IsChecked.Value Then
                recurrence.DaysOfWeek = recurrence.DaysOfWeek OrElse Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Tuesday
            End If

            If WednesdayCheckBox.IsChecked.Value Then
                recurrence.DaysOfWeek = recurrence.DaysOfWeek OrElse Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Wednesday
            End If

            If ThursdayCheckBox.IsChecked.Value Then
                recurrence.DaysOfWeek = recurrence.DaysOfWeek OrElse Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Thursday
            End If

            If FridayCheckBox.IsChecked.Value Then
                recurrence.DaysOfWeek = recurrence.DaysOfWeek OrElse Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Friday
            End If

            If SaturdayCheckBox.IsChecked.Value Then
                recurrence.DaysOfWeek = recurrence.DaysOfWeek OrElse Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.Saturday
            End If

            If((recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.Weekly) OrElse (recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.MonthlyOnDay) OrElse (recurrence.Unit = Windows.ApplicationModel.Appointments.AppointmentRecurrenceUnit.YearlyOnDay)) AndAlso (recurrence.DaysOfWeek = Windows.ApplicationModel.Appointments.AppointmentDaysOfWeek.None) Then
                errorMessage = "The recurrence specified is invalid. For Weekly, MonthlyOnDay or YearlyOnDay recurrence unit values, at least one day must be specified."
            End If

            recurrence.Month = CType(MonthSlider.Value, UInteger)
            recurrence.Day = CType(DaySlider.Value, UInteger)
            If errorMessage Is Nothing Then
                rootPage.NotifyUser("The recurrence specified was created successfully and is valid.", NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser(errorMessage, NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
