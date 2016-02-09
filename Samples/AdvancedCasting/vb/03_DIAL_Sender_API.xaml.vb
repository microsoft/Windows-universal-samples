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
Imports ScreenCasting.Controls
Imports ScreenCasting.Data.Azure
Imports ScreenCasting.Data.Common
Imports System
Imports Windows.Devices.Enumeration
Imports Windows.Foundation
Imports Windows.Media.DialProtocol
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    Public NotInheritable Partial Class Scenario03
        Inherits Page

        Private rootPage As MainPage

        Private picker As DialDevicePicker = Nothing

        Private video As VideoMetaData = Nothing

        Private activeDialDevice As DialDevice = Nothing

        Private activeDeviceInformation As DeviceInformation = Nothing

        Public Sub New()
            Me.InitializeComponent()
            rootPage = MainPage.Current
            AddHandler player.MediaOpened, AddressOf Player_MediaOpened
            AddHandler player.MediaFailed, AddressOf Player_MediaFailed
            AddHandler player.CurrentStateChanged, AddressOf Player_CurrentStateChanged
            ' Get an Azure hosted video
            Dim dataProvider As AzureDataProvider = New AzureDataProvider()
            video = dataProvider.GetRandomVideo()
            rootPage.NotifyUser(String.Format("Opening '{0}'", video.Title), NotifyType.StatusMessage)
            Me.player.Source = video.VideoLink
            AddHandler(CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)).CastButtonClicked, AddressOf TransportControls_CastButtonClicked
            Me.dial_launch_args_textbox.Text = String.Format("v={0}&t=0&pairingCode=E4A8136D-BCD3-45F4-8E49-AE01E9A46B5F", video.Id)
            AddHandler(CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)).CastButtonClicked, AddressOf TransportControls_CastButtonClicked
            picker = New DialDevicePicker()
            picker.Filter.SupportedAppNames.Add(Me.dial_appname_textbox.Text)
            AddHandler picker.DialDeviceSelected, AddressOf Picker_DeviceSelected
            AddHandler picker.DisconnectButtonClicked, AddressOf Picker_DisconnectButtonClicked
            AddHandler picker.DialDevicePickerDismissed, AddressOf Picker_DevicePickerDismissed
        End Sub

        Private Sub TransportControls_CastButtonClicked(sender As Object, e As EventArgs)
            rootPage.NotifyUser("Show Device Picker Button Clicked", NotifyType.StatusMessage)
            player.Pause()
            'Get the custom transport controls
            Dim mtc As MediaTransportControlsWithCustomCastButton = CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)
            'Retrieve the location of the casting button
            Dim transform As GeneralTransform = mtc.CastButton.TransformToVisual(TryCast(Window.Current.Content, UIElement))
            Dim pt As Point = transform.TransformPoint(New Point(0, 0))
            picker.Show(New Rect(pt.X, pt.Y, mtc.CastButton.ActualWidth, mtc.CastButton.ActualHeight), Windows.UI.Popups.Placement.Above)
            Try
                If activeDialDevice IsNot Nothing Then
                    picker.SetDisplayStatus(activeDialDevice, DialDeviceDisplayStatus.Connected)
                End If
            Catch ex As Exception
                UnhandledExceptionPage.ShowUnhandledException(ex)
            End Try
        End Sub

#Region " Device Picker Methods "
        Private Async Sub Picker_DevicePickerDismissed(sender As DialDevicePicker, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                Try
                    rootPage.NotifyUser(String.Format("Picker DevicePickerDismissed event fired for device"), NotifyType.StatusMessage)
                    If activeDialDevice IsNot Nothing Then
                        Me.player.Play()
                    End If
                Catch ex As Exception
                    UnhandledExceptionPage.ShowUnhandledException(ex)
                End Try
            End Sub)
        End Sub

        Private Async Sub Picker_DisconnectButtonClicked(sender As DialDevicePicker, args As DialDisconnectButtonClickedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                Try
                    rootPage.NotifyUser(String.Format("Picker DisconnectButtonClicked event fired for device '{0}'", activeDeviceInformation.Name), NotifyType.StatusMessage)
                    ' Get the DialDevice instance for the selected device
                    Dim selectedDialDevice As DialDevice = Await DialDevice.FromIdAsync(args.Device.Id)
                    picker.SetDisplayStatus(selectedDialDevice, DialDeviceDisplayStatus.Connecting)
                    Dim app As DialApp = selectedDialDevice.GetDialApp(Me.dial_appname_textbox.Text)
                    'Get the current application state
                    'DialAppStateDetails stateDetails = await app.GetAppStateAsync();
                    Dim result As DialAppStopResult = Await app.StopAsync()
                    If result = DialAppStopResult.Stopped Then
                        rootPage.NotifyUser("Application stopped successfully.", NotifyType.StatusMessage)
                        picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Disconnected)
                        activeDialDevice = Nothing
                        activeDeviceInformation = Nothing
                        picker.Hide()
                    Else
                        If result = DialAppStopResult.StopFailed OrElse result = DialAppStopResult.NetworkFailure Then
                            rootPage.NotifyUser(String.Format("Error occured trying to stop application. Status: '{0}'", result.ToString()), NotifyType.StatusMessage)
                            picker.SetDisplayStatus(args.Device, DialDeviceDisplayStatus.Error)
                        Else
                            rootPage.NotifyUser(String.Format("Stop is not supported by device: '{0}'", activeDeviceInformation.Name), NotifyType.ErrorMessage)
                            activeDialDevice = Nothing
                            activeDeviceInformation = Nothing
                        End If
                    End If
                Catch ex As Exception
                    UnhandledExceptionPage.ShowUnhandledException(ex)
                End Try
            End Sub)
        End Sub

        Private Async Sub Picker_DeviceSelected(sender As DialDevicePicker, args As DialDeviceSelectedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                Try
                    rootPage.NotifyUser(String.Format("Picker DeviceSelected event fired"), NotifyType.StatusMessage)
                    picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Connecting)
                    rootPage.NotifyUser(String.Format("Resolving DialDevice'"), NotifyType.StatusMessage)
                    'Get the DialApp object for the specific application on the selected device
                    Dim app As DialApp = args.SelectedDialDevice.GetDialApp(Me.dial_appname_textbox.Text)
                    If app Is Nothing Then
                        picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Error)
                    Else
                        rootPage.NotifyUser(String.Format("Attempting to launch '{0}'", app.AppName), NotifyType.StatusMessage)
                        'Launch the application on the 1st screen device
                        Dim result As DialAppLaunchResult = Await app.RequestLaunchAsync(Me.dial_launch_args_textbox.Text)
                        If result = DialAppLaunchResult.Launched Then
                            rootPage.NotifyUser(String.Format("Launched '{0}'", app.AppName), NotifyType.StatusMessage)
                            activeDialDevice = args.SelectedDialDevice
                            Dim selectedDeviceInformation As DeviceInformation = Await DeviceInformation.CreateFromIdAsync(args.SelectedDialDevice.Id)
                            activeDeviceInformation = selectedDeviceInformation
                            picker.SetDisplayStatus(activeDialDevice, DialDeviceDisplayStatus.Connected)
                            picker.Hide()
                        Else
                            rootPage.NotifyUser(String.Format("Attempting to launch '{0}'", app.AppName), NotifyType.StatusMessage)
                            picker.SetDisplayStatus(args.SelectedDialDevice, DialDeviceDisplayStatus.Error)
                        End If
                    End If
                Catch ex As Exception
                    UnhandledExceptionPage.ShowUnhandledException(ex)
                End Try
            End Sub)
        End Sub

#End Region
#Region " Media Element Status Methods "
        Private Sub Player_CurrentStateChanged(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser(String.Format("{0} '{1}'", Me.player.CurrentState, video.Title), NotifyType.StatusMessage)
        End Sub

        Private Sub Player_MediaFailed(sender As Object, e As ExceptionRoutedEventArgs)
            rootPage.NotifyUser(String.Format("Failed to load '{0}'", video.Title), NotifyType.ErrorMessage)
        End Sub

        Private Sub Player_MediaOpened(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser(String.Format("Openend '{0}'", video.Title), NotifyType.StatusMessage)
            player.Play()
        End Sub

#End Region
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub
    End Class
End Namespace
