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

    Public NotInheritable Partial Class Scenario3_Replace
        Inherits Page

        Private rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Replace an appointment on the user's calendar using the default appointments provider app.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub Replace_Click(sender As Object, e As RoutedEventArgs)
            ' The appointment id argument for ReplaceAppointmentAsync is typically retrieved from AddAppointmentAsync and stored in app data.
            Dim appointmentIdOfAppointmentToReplace As String = AppointmentIdTextBox.Text
            If String.IsNullOrEmpty(appointmentIdOfAppointmentToReplace) Then
                rootPage.NotifyUser("The appointment id cannot be empty", NotifyType.ErrorMessage)
            Else
                ' The Appointment argument for ReplaceAppointmentAsync should contain all of the Appointment's properties including those that may have changed.
                Dim appointment = New Windows.ApplicationModel.Appointments.Appointment()
                ' Get the selection rect of the button pressed to replace this appointment
                Dim rect = MainPage.GetElementRect(TryCast(sender, FrameworkElement))
                ' ReplaceAppointmentAsync returns an updated appointment id when the appointment was successfully replaced.
                ' The updated id may or may not be the same as the original one retrieved from AddAppointmentAsync.
                ' An optional instance start time can be provided to indicate that a specific instance on that date should be replaced
                ' in the case of a recurring appointment.
                ' If the appointment id returned is an empty string, that indicates that the appointment was not replaced.
                Dim updatedAppointmentId As String
                If InstanceStartDateCheckBox.IsChecked.Value Then
                    ' Replace a specific instance starting on the date provided.
                    Dim instanceStartDate = InstanceStartDateDatePicker.Date
                    updatedAppointmentId = Await Windows.ApplicationModel.Appointments.AppointmentManager.ShowReplaceAppointmentAsync(appointmentIdOfAppointmentToReplace, appointment, rect, Windows.UI.Popups.Placement.Default, instanceStartDate)
                Else
                    updatedAppointmentId = Await Windows.ApplicationModel.Appointments.AppointmentManager.ShowReplaceAppointmentAsync(appointmentIdOfAppointmentToReplace, appointment, rect, Windows.UI.Popups.Placement.Default)
                End If

                If updatedAppointmentId <> String.Empty Then
                    rootPage.NotifyUser("Updated Appointment Id: " & updatedAppointmentId, NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser("Appointment not replaced.", NotifyType.ErrorMessage)
                End If
            End If
        End Sub
    End Class
End Namespace
