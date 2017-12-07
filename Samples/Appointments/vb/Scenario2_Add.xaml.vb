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

    Public NotInheritable Partial Class Scenario2_Add
        Inherits Page

        Private rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Adds an appointment to the default appointment provider app.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub Add_Click(sender As Object, e As RoutedEventArgs)
            ' Create an Appointment that should be added the user's appointments provider app.
            Dim appointment = New Windows.ApplicationModel.Appointments.Appointment()
            ' Get the selection rect of the button pressed to add this appointment
            Dim rect = MainPage.GetElementRect(TryCast(sender, FrameworkElement))
            ' ShowAddAppointmentAsync returns an appointment id if the appointment given was added to the user's calendar.
            ' This value should be stored in app data and roamed so that the appointment can be replaced or removed in the future.
            ' An empty string return value indicates that the user canceled the operation before the appointment was added.
            Dim appointmentId As String = Await Windows.ApplicationModel.Appointments.AppointmentManager.ShowAddAppointmentAsync(appointment, rect, Windows.UI.Popups.Placement.Default)
            If appointmentId <> String.Empty Then
                rootPage.NotifyUser("Appointment Id: " & appointmentId, NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("Appointment not added.", NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
