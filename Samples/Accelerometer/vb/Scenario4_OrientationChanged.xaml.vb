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
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            accelerometerOriginal = Accelerometer.GetDefault(rootPage.AccelerometerReadingType)
            accelerometerReadingTransform = Accelerometer.GetDefault(rootPage.AccelerometerReadingType)
            If accelerometerOriginal Is Nothing OrElse accelerometerReadingTransform Is Nothing Then
                rootPage.NotifyUser(rootPage.AccelerometerReadingType.ToString & " accelerometer not found", NotifyType.ErrorMessage)
            Else
                rootPage.NotifyUser(rootPage.AccelerometerReadingType.ToString & " accelerometer ready", NotifyType.StatusMessage)
                ScenarioEnableButton.IsEnabled = True
            End If

            displayInformation = DisplayInformation.GetForCurrentView()
            AddHandler displayInformation.OrientationChanged, AddressOf displayInformation_OrientationChanged
        End Sub

        Protected Overrides Sub OnNavigatingFrom(e As NavigatingCancelEventArgs)
            If ScenarioDisableButton.IsEnabled Then
                ScenarioDisable()
            End If

            RemoveHandler displayInformation.OrientationChanged, AddressOf displayInformation_OrientationChanged
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
                MainPage.SetReadingText(ScenarioOutputOriginal, args.Reading)
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
                MainPage.SetReadingText(ScenarioOutputReadingTransform, args.Reading)
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
        Sub ScenarioDisable()
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
