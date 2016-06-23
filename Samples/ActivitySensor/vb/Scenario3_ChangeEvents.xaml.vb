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
Imports Windows.Foundation
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario3_ChangeEvents
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim ActivitySensorClassId As Guid = New Guid("9D9E0118-1807-4F2E-96E4-2CE57142E196")

        Private _deviceAccessInformation As DeviceAccessInformation

        Private _activitySensor As ActivitySensor

        Public Sub New()
            Me.InitializeComponent()
            _activitySensor = Nothing
            _deviceAccessInformation = DeviceAccessInformation.CreateFromDeviceClassId(ActivitySensorClassId)
            AddHandler _deviceAccessInformation.AccessChanged, New TypedEventHandler(Of DeviceAccessInformation, DeviceAccessChangedEventArgs)(AddressOf AccessChanged)
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached. The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            ScenarioEnableReadingChangedButton.IsEnabled = True
            ScenarioDisableReadingChangedButton.IsEnabled = False
        End Sub

        ''' <summary>
        ''' Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        ''' </summary>
        ''' <param name="e">
        ''' Event data that can be examined by overriding code. The event data is representative
        ''' of the navigation that will unload the current Page unless canceled. The
        ''' navigation can potentially be canceled by setting Cancel.
        ''' </param>
        Protected Overrides Sub OnNavigatingFrom(e As NavigatingCancelEventArgs)
            If ScenarioDisableReadingChangedButton.IsEnabled AndAlso _activitySensor IsNot Nothing Then
                RemoveHandler _activitySensor.ReadingChanged, New TypedEventHandler(Of ActivitySensor, ActivitySensorReadingChangedEventArgs)(AddressOf ReadingChanged)
            End If

            MyBase.OnNavigatingFrom(e)
        End Sub

        ''' <summary>
        ''' This is the event handler for ReadingChanged events.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Async Private Sub ReadingChanged(sender As Object, e As ActivitySensorReadingChangedEventArgs)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                Dim reading As ActivitySensorReading = e.Reading
                ScenarioOutput_Activity.Text = reading.Activity.ToString()
                ScenarioOutput_Confidence.Text = reading.Confidence.ToString()
                ScenarioOutput_ReadingTimestamp.Text = reading.Timestamp.ToString("u")
            End Sub)
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Reading Changed On' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub ScenarioEnableReadingChanged(sender As Object, e As RoutedEventArgs)
            If _deviceAccessInformation.CurrentStatus = DeviceAccessStatus.Allowed Then
                If _activitySensor Is Nothing Then
                    _activitySensor = Await ActivitySensor.GetDefaultAsync()
                End If

                If _activitySensor IsNot Nothing Then
                    _activitySensor.SubscribedActivities.Add(ActivityType.Walking)
                    _activitySensor.SubscribedActivities.Add(ActivityType.Running)
                    _activitySensor.SubscribedActivities.Add(ActivityType.InVehicle)
                    _activitySensor.SubscribedActivities.Add(ActivityType.Biking)
                    _activitySensor.SubscribedActivities.Add(ActivityType.Fidgeting)
                    AddHandler _activitySensor.ReadingChanged, New TypedEventHandler(Of ActivitySensor, ActivitySensorReadingChangedEventArgs)(AddressOf ReadingChanged)
                    ScenarioEnableReadingChangedButton.IsEnabled = False
                    ScenarioDisableReadingChangedButton.IsEnabled = True
                    rootPage.NotifyUser("Subscribed to reading changes", NotifyType.StatusMessage)
                Else
                    rootPage.NotifyUser("No activity sensor found", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("Access denied to activity sensors", NotifyType.ErrorMessage)
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Reading Changed Off' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ScenarioDisableReadingChanged(sender As Object, e As RoutedEventArgs)
            If _activitySensor IsNot Nothing Then
                RemoveHandler _activitySensor.ReadingChanged, New TypedEventHandler(Of ActivitySensor, ActivitySensorReadingChangedEventArgs)(AddressOf ReadingChanged)
                rootPage.NotifyUser("Unsubscribed from reading changes", NotifyType.StatusMessage)
            End If

            ScenarioEnableReadingChangedButton.IsEnabled = True
            ScenarioDisableReadingChangedButton.IsEnabled = False
        End Sub

        ''' <summary>
        ''' This is the event handler for AccessChanged events.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub AccessChanged(sender As DeviceAccessInformation, e As DeviceAccessChangedEventArgs)
            Dim status = e.Status
            If status <> DeviceAccessStatus.Allowed Then
                Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                    rootPage.NotifyUser("Access denied to activity sensors", NotifyType.ErrorMessage)
                    _activitySensor = Nothing
                    ScenarioEnableReadingChangedButton.IsEnabled = True
                    ScenarioDisableReadingChangedButton.IsEnabled = False
                End Sub)
            End If
        End Sub
    End Class
End Namespace
