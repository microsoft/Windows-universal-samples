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
Imports Windows.Media.Casting
Imports Windows.UI
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Media.Imaging
Imports Windows.UI.Xaml.Navigation

Namespace Global.ScreenCasting

    Public NotInheritable Partial Class Scenario02
        Inherits Page

        Private rootPage As MainPage

        Private picker As CastingDevicePicker = Nothing

        Private video As VideoMetaData = Nothing

        Private connection As CastingConnection

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
            player.Source = video.VideoLink
            Me.LicenseText.Text = "License: " & video.License
            AddHandler(CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)).CastButtonClicked, AddressOf TransportControls_CastButtonClicked
            picker = New CastingDevicePicker()
            picker.Filter.SupportedCastingSources.Add(player.GetAsCastingSource())
            AddHandler picker.CastingDeviceSelected, AddressOf Picker_CastingDeviceSelected
            AddHandler picker.CastingDevicePickerDismissed, AddressOf Picker_CastingDevicePickerDismissed
            picker.Appearance.BackgroundColor = Colors.Black
            picker.Appearance.ForegroundColor = Colors.White
            picker.Appearance.AccentColor = Colors.Gray
            picker.Appearance.SelectedAccentColor = Colors.Gray
            picker.Appearance.SelectedForegroundColor = Colors.White
            picker.Appearance.SelectedBackgroundColor = Colors.Black
        End Sub

        Private Sub TransportControls_CastButtonClicked(sender As Object, e As EventArgs)
            rootPage.NotifyUser("Custom Cast Button Clicked", NotifyType.StatusMessage)
            player.Pause()
            'Get the custom transport controls
            Dim mtc As MediaTransportControlsWithCustomCastButton = CType(Me.player.TransportControls, MediaTransportControlsWithCustomCastButton)
            'Retrieve the location of the casting button
            Dim transform As GeneralTransform = mtc.CastButton.TransformToVisual(TryCast(Window.Current.Content, UIElement))
            Dim pt As Point = transform.TransformPoint(New Point(0, 0))
            picker.Show(New Rect(pt.X, pt.Y, mtc.CastButton.ActualWidth, mtc.CastButton.ActualHeight), Windows.UI.Popups.Placement.Above)
        End Sub

        Private Async Sub Picker_CastingDevicePickerDismissed(sender As CastingDevicePicker, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                player.Play()
            End Sub)
        End Sub

        Private Async Sub Picker_CastingDeviceSelected(sender As CastingDevicePicker, args As CastingDeviceSelectedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                Try
                    rootPage.NotifyUser(String.Format("Picker DeviceSelected event fired for device '{0}'", args.SelectedCastingDevice.FriendlyName), NotifyType.StatusMessage)
                    rootPage.NotifyUser(String.Format("Creating connection for '{0}'", args.SelectedCastingDevice.FriendlyName), NotifyType.StatusMessage)
                    connection = args.SelectedCastingDevice.CreateCastingConnection()
                    AddHandler connection.ErrorOccurred, AddressOf Connection_ErrorOccurred
                    AddHandler connection.StateChanged, AddressOf Connection_StateChanged
                    ' Get the casting source from the MediaElement
                    Dim source As CastingSource = Nothing
                    Try
                        source = player.GetAsCastingSource()
                        rootPage.NotifyUser(String.Format("Starting casting to '{0}'", args.SelectedCastingDevice.FriendlyName), NotifyType.StatusMessage)
                        Dim status As CastingConnectionErrorStatus = Await connection.RequestStartCastingAsync(source)
                        If status = CastingConnectionErrorStatus.Succeeded Then
                            player.Play()
                            rootPage.NotifyUser(String.Format("Starting casting to '{0}'", args.SelectedCastingDevice.FriendlyName), NotifyType.StatusMessage)
                        End If
                    Catch
                        rootPage.NotifyUser(String.Format("Failed to get casting source for video '{0}'", video.Title), NotifyType.ErrorMessage)
                    End Try
                Catch ex As Exception
                    UnhandledExceptionPage.ShowUnhandledException(ex)
                End Try
            End Sub)
        End Sub

#Region " Casting Connection Status Methods "
        Private Async Sub Connection_StateChanged(sender As CastingConnection, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Casting Connection State Changed: " & sender.State.ToString(), NotifyType.StatusMessage)
            End Sub)
        End Sub

        Private Async Sub Connection_ErrorOccurred(sender As CastingConnection, args As CastingConnectionErrorOccurredEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Connection Error Occured: " & args.Message, NotifyType.ErrorMessage)
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
        End Sub

#End Region
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub
    End Class
End Namespace
