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

    Public NotInheritable Partial Class Scenario4_Remove
        Inherits Page

        Private rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Removes the appointment associated with a particular appointment id string from the defaul appointment provider app.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub Remove_Click(sender As Object, e As RoutedEventArgs)
            ' The appointment id argument for ShowRemoveAppointmentAsync is typically retrieved from AddAppointmentAsync and stored in app data.
            Dim appointmentId As String = AppointmentIdTextBox.Text
            If String.IsNullOrEmpty(appointmentId) Then
                rootPage.NotifyUser("The appointment id cannot be empty", NotifyType.ErrorMessage)
            Else
                ' Get the selection rect of the button pressed to remove this appointment
                Dim rect = MainPage.GetElementRect(TryCast(sender, FrameworkElement))
                ' ShowRemoveAppointmentAsync returns a boolean indicating whether or not the appointment related to the appointment id given was removed.
                ' An optional instance start time can be provided to indicate that a specific instance on that date should be removed
                ' in the case of a recurring appointment.
                Dim removed As Boolean
                If InstanceStartDateCheckBox.IsChecked.Value Then
                    ' Remove a specific instance starting on the date provided.
                    Dim instanceStartDate = InstanceStartDateDatePicker.Date
                    removed = Await Windows.ApplicationModel.Appointments.AppointmentManager.ShowRemoveAppointmentAsync(appointmentId, rect, Windows.UI.Popups.Placement.Default, instanceStartDate)
                Else
                    removed = Await Windows.ApplicationModel.Appointments.AppointmentManager.ShowRemoveAppointmentAsync(appointmentId, rect, Windows.UI.Popups.Placement.Default)
                End If

                If removed Then
                    rootPage.NotifyUser("Appointment removed", NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser("Appointment not removed", NotifyType.ErrorMessage)
                End If
            End If
        End Sub
    End Class
End Namespace
