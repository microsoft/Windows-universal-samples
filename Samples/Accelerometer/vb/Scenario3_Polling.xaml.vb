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
Imports Windows.Devices.Sensors
Imports Windows.UI.Core

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class Scenario3_Polling
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Private _accelerometer As Accelerometer

        Private _desiredReportInterval As UInteger

        Private _dispatcherTimer As DispatcherTimer

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            _accelerometer = Accelerometer.GetDefault(rootPage.AccelerometerReadingType)
            If _accelerometer IsNot Nothing Then
                ' Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                ' This value will be used later to activate the sensor.
                _desiredReportInterval = Math.Max(_accelerometer.MinimumReportInterval, 16)

                ' Set up a DispatchTimer
                _dispatcherTimer = New DispatcherTimer()
                AddHandler _dispatcherTimer.Tick, AddressOf DisplayCurrentReading
                _dispatcherTimer.Interval = New TimeSpan(0, 0, 0, 0, CType(_desiredReportInterval, Integer))

                rootPage.NotifyUser(rootPage.AccelerometerReadingType.ToString & " accelerometer ready", NotifyType.StatusMessage)
                ScenarioEnableButton.IsEnabled = True
            Else
                rootPage.NotifyUser(rootPage.AccelerometerReadingType.ToString & " accelerometer not found", NotifyType.ErrorMessage)
            End If
        End Sub

        Protected Overrides Sub OnNavigatingFrom(e As NavigatingCancelEventArgs)
            If ScenarioDisableButton.IsEnabled Then
                ScenarioDisable()
            End If
        End Sub

        ''' <summary>
        ''' This is the event handler for VisibilityChanged events. You would register for these notifications
        ''' if handling sensor data when the app is not visible could cause unintended actions in the app.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e">
        ''' Event data that can be examined for the current visibility state.
        ''' </param>
        Private Sub VisibilityChanged(sender As Object, e As VisibilityChangedEventArgs)
            If ScenarioDisableButton.IsEnabled Then
                If e.Visible Then
                    _dispatcherTimer.Start()
                Else
                    _dispatcherTimer.Stop()
                End If
            End If
        End Sub

        ''' <summary>
        ''' This is the dispatcher callback.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Private Sub DisplayCurrentReading(sender As Object, args As Object)
            Dim reading As AccelerometerReading = _accelerometer.GetCurrentReading()
            If reading IsNot Nothing Then
                MainPage.SetReadingText(ScenarioOutput, reading)
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Enable' button.
        ''' </summary>
        Private Sub ScenarioEnable()
            _accelerometer.ReportInterval = _desiredReportInterval
            AddHandler Window.Current.VisibilityChanged, AddressOf VisibilityChanged
            _dispatcherTimer.Start()
            ScenarioEnableButton.IsEnabled = False
            ScenarioDisableButton.IsEnabled = True
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Disable' button.
        ''' </summary>
        Private Sub ScenarioDisable()
            RemoveHandler Window.Current.VisibilityChanged, AddressOf VisibilityChanged
            _dispatcherTimer.Stop()
            _accelerometer.ReportInterval = 0
            ScenarioEnableButton.IsEnabled = True
            ScenarioDisableButton.IsEnabled = False
        End Sub
    End Class
End Namespace
