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
Imports Windows.Globalization
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario2_History
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim ActivitySensorClassId As Guid = New Guid("9D9E0118-1807-4F2E-96E4-2CE57142E196")

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Get Activity History' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Async Private Sub ScenarioGetActivityHistory(sender As Object, e As RoutedEventArgs)
            ScenarioOutput_Count.Text = "No data"
            ScenarioOutput_Activity1.Text = "No data"
            ScenarioOutput_Confidence1.Text = "No data"
            ScenarioOutput_Timestamp1.Text = "No data"
            ScenarioOutput_ActivityN.Text = "No data"
            ScenarioOutput_ConfidenceN.Text = "No data"
            ScenarioOutput_TimestampN.Text = "No data"
            rootPage.NotifyUser("", NotifyType.StatusMessage)
            ' Determine if we can access activity sensors
            Dim deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(ActivitySensorClassId)
            If deviceAccessInfo.CurrentStatus = DeviceAccessStatus.Allowed Then
                ' Determine if an activity sensor is present
                ' This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
                Dim activitySensor As ActivitySensor = Await ActivitySensor.GetDefaultAsync()
                If activitySensor IsNot Nothing Then
                    Dim calendar = New Calendar()
                    calendar.SetToNow()
                    calendar.AddDays(-1)
                    Dim yesterday = calendar.GetDateTime()
                    ' Get history from yesterday onwards
                    Dim history = Await ActivitySensor.GetSystemHistoryAsync(yesterday)
                    ScenarioOutput_Count.Text = history.Count.ToString()
                    If history.Count > 0 Then
                        Dim reading1 = history(0)
                        ScenarioOutput_Activity1.Text = reading1.Activity.ToString()
                        ScenarioOutput_Confidence1.Text = reading1.Confidence.ToString()
                        ScenarioOutput_Timestamp1.Text = reading1.Timestamp.ToString("u")
                        Dim readingN = history(history.Count - 1)
                        ScenarioOutput_ActivityN.Text = readingN.Activity.ToString()
                        ScenarioOutput_ConfidenceN.Text = readingN.Confidence.ToString()
                        ScenarioOutput_TimestampN.Text = readingN.Timestamp.ToString("u")
                    End If
                Else
                    rootPage.NotifyUser("No activity sensors found", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("Access to activity sensors is denied", NotifyType.ErrorMessage)
            End If
        End Sub
    End Class
End Namespace
