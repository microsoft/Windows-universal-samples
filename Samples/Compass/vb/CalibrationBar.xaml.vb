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
Imports System.Collections.Generic
Imports System.IO
Imports System.Linq
Imports System.Runtime.InteropServices.WindowsRuntime
Imports Windows.Foundation
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Controls.Primitives
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Media.Imaging
Imports Windows.UI.Xaml.Navigation
Imports Windows.Devices.Sensors

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class CalibrationBar
        Inherits Page

        ' Constants specifying when the calibration bar is shown:
        '   * suppress duration determines how long (in minutes) to wait before 
        '     showing the calibration bar again 
        '   * auto dismiss time determines when to auto dismiss the bar
        '     if it's shown too long
        Private Const SUPPRESS_DURATION_APPROXIMATE_MIN As Double = 10.0

        Private Const SUPPRESS_DURATION_UNRELIABLE_MIN As Double = 5.0

        Private CALIBRATION_POPUP_AUTO_DIMSMISS_TIME As TimeSpan = New TimeSpan(0, 0, 30)

        Private Const LAYOUT_WIDTH_768 As Double = 768.0

        Private _lastCalibrationTime As DateTime = New DateTime(0)

        Private _barDismissed As Boolean = False

        Private _calibrationTimer As DispatcherTimer

        Public Sub New()
            Me.InitializeComponent()
            _calibrationTimer = New DispatcherTimer()
            AddHandler _calibrationTimer.Tick, AddressOf OnAutoDismissTimerExpired
            _calibrationTimer.Interval = CALIBRATION_POPUP_AUTO_DIMSMISS_TIME
        End Sub

        ''' <summary>
        ''' Called when the application would like to show the calibration bar to the user 
        ''' to improve sensor accuracy.
        ''' </summary>
        ''' <param name="accuracy">Current accuracy of the sensor.</param>
        Public Sub RequestCalibration(accuracy As MagnetometerAccuracy)
            If Not CalibrationPopup.IsOpen AndAlso CanShowCalibrationBar(accuracy) Then
                InvalidateLayout()
                AddHandler Window.Current.SizeChanged, AddressOf OnWindowSizeChanged
                HideGuidance()
                CalibrationPopup.IsOpen = True
                _calibrationTimer.Start()
            End If
        End Sub

        ''' <summary>
        ''' Called when sensor accuracy is sufficient and the application would like to 
        ''' hide the calibration bar.
        ''' </summary>
        Public Sub Hide()
            If CalibrationPopup.IsOpen Then
                _lastCalibrationTime = DateTime.Now
                HideGuidance()
                CalibrationPopup.IsOpen = False
                RemoveHandler Window.Current.SizeChanged, AddressOf OnWindowSizeChanged
                _calibrationTimer.Stop()
            End If
        End Sub

        ''' <summary>
        ''' Invoked when the window size is updated.
        ''' </summary>
        ''' <param name="sender">Instance that triggered the event.</param>
        ''' <param name="e">Event data describing the conditions that led to the event.</param>
        Sub OnWindowSizeChanged(sender As Object, e As Windows.UI.Core.WindowSizeChangedEventArgs)
            InvalidateLayout()
        End Sub

        ''' <summary>
        ''' Updates the calibration bar layout based on the current window width.
        ''' </summary>
        Private Sub InvalidateLayout()
            CalibrationGrid.Width = Window.Current.Bounds.Width
            If CalibrationGrid.Width < LAYOUT_WIDTH_768 Then
                Grid.SetRow(DismissPopup, 1)
                Grid.SetColumn(DismissPopup, 1)
                CalibrationGridTop.Margin = New Thickness(0, 24, 20, 15)
                DismissPopup.Margin = New Thickness(66, 15, 0, 0)
            Else
                Grid.SetRow(DismissPopup, 0)
                Grid.SetColumn(DismissPopup, 2)
                CalibrationGridTop.Margin = New Thickness(0, 19, 66, 19)
                DismissPopup.Margin = New Thickness(66, 0, 0, 0)
            End If
        End Sub

        ''' <summary>
        ''' Called to determine if sufficient time has passed since the calibration bar
        ''' was hidden to show the calibration bar again. This suppression time is
        ''' dependent on the current sensor accuracy.
        ''' </summary>
        ''' <param name="accuracy">Current accuracy of the sensor.</param>
        Private Function CanShowCalibrationBar(accuracy As MagnetometerAccuracy) As Boolean
            Dim showBar As Boolean = False
            If Not _barDismissed Then
                If(accuracy = MagnetometerAccuracy.Approximate) AndAlso (DateTime.Now.CompareTo(_lastCalibrationTime.AddMinutes(SUPPRESS_DURATION_APPROXIMATE_MIN)) >= 0) Then
                    showBar = True
                ElseIf(accuracy = MagnetometerAccuracy.Unreliable) AndAlso (DateTime.Now.CompareTo(_lastCalibrationTime.AddMinutes(SUPPRESS_DURATION_UNRELIABLE_MIN)) >= 0) Then
                    showBar = True
                End If
            End If

            Return showBar
        End Function

        ''' <summary>
        ''' This is the click handler for the show guidance button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ShowGuidanceButton_Click(sender As Object, e As RoutedEventArgs)
            If Guidance.Visibility = Windows.UI.Xaml.Visibility.Collapsed Then
                ShowGuidance()
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the hide guidance button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub HideGuidanceButton_Click(sender As Object, e As RoutedEventArgs)
            If Guidance.Visibility = Windows.UI.Xaml.Visibility.Visible Then
                HideGuidance()
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the dismiss popup button. It hides the 
        ''' calibration bar and prevents it from being shown again for the lifetime
        ''' of the app.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub DismissPopup_Click(sender As Object, e As RoutedEventArgs)
            _barDismissed = True
            Hide()
        End Sub

        ''' <summary>
        ''' Expands the calibration bar to show the calibration image to the user.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ShowGuidance()
            ShowGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Collapsed
            HideGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Visible
            Guidance.Visibility = Windows.UI.Xaml.Visibility.Visible
        End Sub

        ''' <summary>
        ''' Collapses the calibration bar to hide the calibration image from the user.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub HideGuidance()
            HideGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Collapsed
            ShowGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Visible
            Guidance.Visibility = Windows.UI.Xaml.Visibility.Collapsed
        End Sub

        ''' <summary>
        ''' This is the handler for the auto dismiss timer. After the calibraiton bar
        ''' has been shown for the maximum time, it is automatically hidden.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub OnAutoDismissTimerExpired(sender As Object, e As Object)
            Hide()
        End Sub
    End Class
End Namespace
