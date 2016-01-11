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

    Public NotInheritable Partial Class Scenario5_Show
        Inherits Page

        Private rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Show the default appointment provider at a point in time 24 hours from now.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub Show_Click(sender As Object, e As RoutedEventArgs)
            Dim dateToShow = DateTime.Now.AddDays(1)
            Dim duration = TimeSpan.FromHours(1)
            Await Windows.ApplicationModel.Appointments.AppointmentManager.ShowTimeFrameAsync(dateToShow, duration)
            rootPage.NotifyUser("The default appointments provider should have appeared on screen.", NotifyType.StatusMessage)
        End Sub
    End Class
End Namespace
