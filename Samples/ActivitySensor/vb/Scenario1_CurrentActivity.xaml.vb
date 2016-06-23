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
Imports Windows.Devices.Enumeration
Imports Windows.Devices.Sensors
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario1_CurrentActivity
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim ActivitySensorClassId As Guid = New Guid("9D9E0118-1807-4F2E-96E4-2CE57142E196")

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Get Current Activity' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Async Private Sub ScenarioGetCurrentActivity(sender As Object, e As RoutedEventArgs)
            ScenarioOutput_Activity.Text = "No data"
            ScenarioOutput_Confidence.Text = "No data"
            ScenarioOutput_Timestamp.Text = "No data"
            rootPage.NotifyUser("", NotifyType.StatusMessage)
            ' Determine if we can access activity sensors
            Dim deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(ActivitySensorClassId)
            If deviceAccessInfo.CurrentStatus = DeviceAccessStatus.Allowed Then
                ' Get the default sensor
                Dim activitySensor As ActivitySensor = Await ActivitySensor.GetDefaultAsync()
                If activitySensor IsNot Nothing Then
                    ' Get the current activity reading
                    Dim reading = Await activitySensor.GetCurrentReadingAsync()
                    If reading IsNot Nothing Then
                        ScenarioOutput_Activity.Text = reading.Activity.ToString()
                        ScenarioOutput_Confidence.Text = reading.Confidence.ToString()
                        ScenarioOutput_Timestamp.Text = reading.Timestamp.ToString("u")
                    End If
                Else
                    rootPage.NotifyUser("No activity sensor found", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("Access denied to activity sensors", NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
