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
Imports System
Imports System.Threading.Tasks
Imports Windows.Devices.Sensors
Imports Windows.Foundation
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario2_Polling
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Private sensor As Barometer

        Public Sub New()
            Me.InitializeComponent()
            sensor = Barometer.GetDefault()
            If Nothing Is sensor Then
                rootPage.NotifyUser("No barometer found", NotifyType.ErrorMessage)
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'GetData' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ScenarioGetData(sender As Object, e As RoutedEventArgs)
            If Nothing IsNot sensor Then
                Dim reading As BarometerReading = sensor.GetCurrentReading()
                If Nothing IsNot reading Then
                    ScenarioOutput_hPa.Text = String.Format("{0,5:0.00}", reading.StationPressureInHectopascals)
                End If
            Else
                rootPage.NotifyUser("No barometer found", NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
