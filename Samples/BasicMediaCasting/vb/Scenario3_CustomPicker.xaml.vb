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
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports SDKTemplate
Imports Windows.Storage
Imports Windows.Storage.Pickers
Imports Windows.Storage.Streams
Imports Windows.Media.Casting
Imports Windows.Devices.Enumeration

Namespace Global.BasicMediaCasting

    Public NotInheritable Partial Class Scenario3
        Inherits Page

        Private rootPage As MainPage

        Private watcher As DeviceWatcher

        Private connection As CastingConnection

        Public Sub New()
            Me.InitializeComponent()
            watcher = DeviceInformation.CreateWatcher(CastingDevice.GetDeviceSelector(CastingPlaybackTypes.Video))
            AddHandler watcher.Added, AddressOf Watcher_Added
            AddHandler watcher.Removed, AddressOf Watcher_Removed
            AddHandler watcher.Stopped, AddressOf Watcher_Stopped
            AddHandler watcher.EnumerationCompleted, AddressOf Watcher_EnumerationCompleted
        End Sub

        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
        End Sub

        Private Async Sub loadButton_Click(sender As Object, e As RoutedEventArgs)
            'Create a new picker
            Dim filePicker As FileOpenPicker = New FileOpenPicker()
            filePicker.FileTypeFilter.Add(".wmv")
            filePicker.FileTypeFilter.Add(".mp4")
            filePicker.SuggestedStartLocation = PickerLocationId.VideosLibrary
            'Retrieve file from picker
            Dim file As StorageFile = Await filePicker.PickSingleFileAsync()
            If file IsNot Nothing Then
                Dim stream As IRandomAccessStream = Await file.OpenAsync(FileAccessMode.Read)
                video.SetSource(stream, file.ContentType)
                rootPage.NotifyUser("Content Selected", NotifyType.StatusMessage)
            End If
        End Sub

        Private Sub watcherControlButton_Click(sender As Object, e As RoutedEventArgs)
            If watcher.Status <> DeviceWatcherStatus.Started Then
                castingDevicesList.Items.Clear()
                watcher.Start()
                rootPage.NotifyUser("Watcher has been started", NotifyType.StatusMessage)
                watcherControlButton.Content = "Stop Device Watcher"
                progressText.Text = "Searching"
                progressRing.IsActive = True
            Else
                progressText.Text = ""
                watcher.Stop()
            End If
        End Sub

        Private Async Sub Watcher_Added(sender As DeviceWatcher, args As DeviceInformation)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                'Add each discovered device to our listbox
                Dim addedDevice As CastingDevice = Await CastingDevice.FromIdAsync(args.Id)
                castingDevicesList.Items.Add(addedDevice)
            End Sub)
        End Sub

        Private Async Sub Watcher_Removed(sender As DeviceWatcher, args As DeviceInformationUpdate)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                For Each currentDevice In castingDevicesList.Items
                    If currentDevice.Id = args.Id Then
                        castingDevicesList.Items.Remove(currentDevice)
                        Exit For
                    End If
                Next
            End Sub)
        End Sub

        Private Async Sub Watcher_EnumerationCompleted(sender As DeviceWatcher, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Watcher completed enumeration of devices", NotifyType.StatusMessage)
                progressText.Text = "Enumeration Completed"
                watcher.Stop()
            End Sub)
        End Sub

        Private Async Sub Watcher_Stopped(sender As DeviceWatcher, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Watcher has been stopped", NotifyType.StatusMessage)
                watcherControlButton.Content = "Start Device Watcher"
                progressRing.IsActive = False
            End Sub)
        End Sub

        Private Async Sub castingDevicesList_SelectionChanged(sender As Object, e As SelectionChangedEventArgs)
            If castingDevicesList.SelectedItem IsNot Nothing Then
                If watcher.Status <> DeviceWatcherStatus.Stopped Then
                    progressText.Text = ""
                    watcher.Stop()
                End If

                connection =(CType(castingDevicesList.SelectedItem, CastingDevice)).CreateCastingConnection()
                AddHandler connection.ErrorOccurred, AddressOf Connection_ErrorOccurred
                AddHandler connection.StateChanged, AddressOf Connection_StateChanged
                Await connection.RequestStartCastingAsync(video.GetAsCastingSource())
            End If
        End Sub

        Private Async Sub Connection_StateChanged(sender As CastingConnection, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                If sender.State = CastingConnectionState.Connected OrElse sender.State = CastingConnectionState.Rendering Then
                    disconnectButton.Visibility = Visibility.Visible
                    progressText.Text = "Connected"
                    progressRing.IsActive = False
                ElseIf sender.State = CastingConnectionState.Disconnected Then
                    disconnectButton.Visibility = Visibility.Collapsed
                    castingDevicesList.SelectedItem = Nothing
                    progressText.Text = ""
                    progressRing.IsActive = False
                ElseIf sender.State = CastingConnectionState.Connecting Then
                    disconnectButton.Visibility = Visibility.Collapsed
                    progressText.Text = "Connecting"
                    progressRing.IsActive = True
                Else
                    disconnectButton.Visibility = Visibility.Collapsed
                    progressText.Text = "Disconnecting"
                    progressRing.IsActive = True
                End If
            End Sub)
        End Sub

        Private Async Sub Connection_ErrorOccurred(sender As CastingConnection, args As CastingConnectionErrorOccurredEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Casting Error: " & args.Message, NotifyType.ErrorMessage)
                castingDevicesList.SelectedItem = Nothing
            End Sub)
        End Sub

        Private Async Sub disconnectButton_Click(sender As Object, e As RoutedEventArgs)
            If connection IsNot Nothing Then
                Await connection.DisconnectAsync()
            End If
        End Sub
    End Class
End Namespace
