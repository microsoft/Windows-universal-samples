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
Imports Windows.Devices.Sensors
Imports Windows.Foundation
Imports System.Threading.Tasks
Imports Windows.UI.Core
Imports Windows.Graphics.Display

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario4_OrientationChanged
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim accelerometerOriginal As Accelerometer

        Dim accelerometerReadingTransform As Accelerometer

        Dim displayInformation As DisplayInformation

        Public Sub New()
            Me.InitializeComponent()
            accelerometerOriginal = Accelerometer.GetDefault()
            accelerometerReadingTransform = Accelerometer.GetDefault()
            If accelerometerOriginal Is Nothing OrElse accelerometerReadingTransform Is Nothing Then
                rootPage.NotifyUser("No accelerometer found", NotifyType.ErrorMessage)
            End If

            displayInformation = DisplayInformation.GetForCurrentView()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached. The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            If accelerometerOriginal Is Nothing OrElse accelerometerReadingTransform Is Nothing Then
                ScenarioEnableButton.IsEnabled = False
            Else
                ScenarioEnableButton.IsEnabled = True
            End If

            ScenarioDisableButton.IsEnabled = False
            AddHandler displayInformation.OrientationChanged, AddressOf displayInformation_OrientationChanged
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
            If ScenarioDisableButton.IsEnabled Then
                RemoveHandler Window.Current.VisibilityChanged, AddressOf Current_VisibilityChanged
                RemoveHandler accelerometerOriginal.ReadingChanged, AddressOf _accelerometerOriginal_ReadingChanged
                RemoveHandler accelerometerReadingTransform.ReadingChanged, AddressOf _accelerometerReadingTransform_ReadingChanged
                accelerometerOriginal.ReportInterval = 0
                accelerometerReadingTransform.ReportInterval = 0
            End If

            RemoveHandler displayInformation.OrientationChanged, AddressOf displayInformation_OrientationChanged
            MyBase.OnNavigatingFrom(e)
        End Sub

        ''' <summary>
        ''' Invoked when there is a change in the display orientation.
        ''' </summary>
        ''' <param name="sender">
        ''' DisplayInformation object from which the new Orientation can be determined
        ''' </param>
        ''' <param name="e"></param>
        Sub displayInformation_OrientationChanged(sender As DisplayInformation, args As Object)
            If Nothing IsNot accelerometerReadingTransform Then
                accelerometerReadingTransform.ReadingTransform = sender.CurrentOrientation
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Enable' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Sub ScenarioEnable(sender As Object, e As RoutedEventArgs)
            accelerometerOriginal.ReportInterval = accelerometerOriginal.MinimumReportInterval
            accelerometerReadingTransform.ReportInterval = accelerometerReadingTransform.MinimumReportInterval
            accelerometerReadingTransform.ReadingTransform = displayInformation.CurrentOrientation
            AddHandler Window.Current.VisibilityChanged, AddressOf Current_VisibilityChanged
            AddHandler accelerometerOriginal.ReadingChanged, AddressOf _accelerometerOriginal_ReadingChanged
            AddHandler accelerometerReadingTransform.ReadingChanged, AddressOf _accelerometerReadingTransform_ReadingChanged
            ScenarioEnableButton.IsEnabled = False
            ScenarioDisableButton.IsEnabled = True
        End Sub

        ''' <summary>
        ''' This is the event handler for ReadingChanged event of the 'accelerometerOriginal' and should 
        ''' notify of the accelerometer reading changes.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args">
        ''' Event data represents the accelerometer reading in its original reference frame.
        ''' </param>
        Async Sub _accelerometerOriginal_ReadingChanged(sender As Accelerometer, args As AccelerometerReadingChangedEventArgs)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                Dim reading As AccelerometerReading = args.Reading
                ScenarioOutput_X_Original.Text = String.Format("{0,5:0.00}", reading.AccelerationX)
                ScenarioOutput_Y_Original.Text = String.Format("{0,5:0.00}", reading.AccelerationY)
                ScenarioOutput_Z_Original.Text = String.Format("{0,5:0.00}", reading.AccelerationZ)
            End Sub)
        End Sub

        ''' <summary>
        ''' This is the event handler for ReadingChanged event of the 'accelerometerReadingTransform' and should 
        ''' notify of the accelerometer reading changes.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args">
        ''' Event data represents the accelerometer reading in its original reference frame.
        ''' </param>
        Async Sub _accelerometerReadingTransform_ReadingChanged(sender As Accelerometer, args As AccelerometerReadingChangedEventArgs)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                Dim reading As AccelerometerReading = args.Reading
                ScenarioOutput_X_ReadingTransform.Text = String.Format("{0,5:0.00}", reading.AccelerationX)
                ScenarioOutput_Y_ReadingTransform.Text = String.Format("{0,5:0.00}", reading.AccelerationY)
                ScenarioOutput_Z_ReadingTransform.Text = String.Format("{0,5:0.00}", reading.AccelerationZ)
            End Sub)
        End Sub

        ''' <summary>
        ''' This is the event handler for VisibilityChanged events. You would register for these notifications
        ''' if handling sensor data when the app is not visible could cause unintended actions in the app.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e">
        ''' Event data that can be examined for the current visibility state.
        ''' </param>
        Sub Current_VisibilityChanged(sender As Object, e As VisibilityChangedEventArgs)
            If ScenarioDisableButton.IsEnabled Then
                If e.Visible Then
                    AddHandler accelerometerOriginal.ReadingChanged, AddressOf _accelerometerOriginal_ReadingChanged
                    AddHandler accelerometerReadingTransform.ReadingChanged, AddressOf _accelerometerReadingTransform_ReadingChanged
                Else
                    RemoveHandler accelerometerOriginal.ReadingChanged, AddressOf _accelerometerOriginal_ReadingChanged
                    RemoveHandler accelerometerReadingTransform.ReadingChanged, AddressOf _accelerometerReadingTransform_ReadingChanged
                End If
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Disable' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Sub ScenarioDisable(sender As Object, e As RoutedEventArgs)
            RemoveHandler Window.Current.VisibilityChanged, AddressOf Current_VisibilityChanged
            RemoveHandler accelerometerOriginal.ReadingChanged, AddressOf _accelerometerOriginal_ReadingChanged
            RemoveHandler accelerometerReadingTransform.ReadingChanged, AddressOf _accelerometerReadingTransform_ReadingChanged
            accelerometerOriginal.ReportInterval = 0
            accelerometerReadingTransform.ReportInterval = 0
            ScenarioEnableButton.IsEnabled = True
            ScenarioDisableButton.IsEnabled = False
        End Sub
    End Class
End Namespace
