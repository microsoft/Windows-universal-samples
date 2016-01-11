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

    Public NotInheritable Partial Class Scenario3_Calibration
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim calibrationBar As CalibrationBar = New CalibrationBar()

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached. The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
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
            calibrationBar.Hide()
            MyBase.OnNavigatingFrom(e)
        End Sub

        ''' <summary>
        ''' This is the click handler for high accuracy. Acceptable accuracy met, so
        ''' hide the calibration bar.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub OnHighAccuracy(sender As Object, e As RoutedEventArgs)
            calibrationBar.Hide()
        End Sub

        ''' <summary>
        ''' This is the click handler for approximate accuracy. Acceptable accuracy met, so
        ''' hide the calibration bar.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub OnApproximateAccuracy(sender As Object, e As RoutedEventArgs)
            calibrationBar.Hide()
        End Sub

        ''' <summary>
        ''' This is the click handler for unreliable accuracy. Sensor does not meet accuracy
        ''' requirements. Request to show the calibration bar per the calibration guidance.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub OnUnreliableAccuracy(sender As Object, e As RoutedEventArgs)
            calibrationBar.RequestCalibration(MagnetometerAccuracy.Unreliable)
        End Sub
    End Class
End Namespace
