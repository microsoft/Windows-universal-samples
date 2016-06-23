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
Imports SDKTemplate
Imports Windows.Foundation
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.UI.Xaml.Media
Imports Windows.Storage
Imports Windows.Storage.Pickers
Imports Windows.Storage.Streams
Imports Windows.Media.Casting

Namespace Global.BasicMediaCasting

    Public NotInheritable Partial Class Scenario2
        Inherits Page

        Private rootPage As MainPage

        Private picker As CastingDevicePicker

        Public Sub New()
            Me.InitializeComponent()
            picker = New CastingDevicePicker()
            picker.Filter.SupportsVideo = True
            AddHandler picker.CastingDeviceSelected, AddressOf Picker_CastingDeviceSelected
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

        Private Sub castButton_Click(sender As Object, e As RoutedEventArgs)
            'Retrieve the location of the casting button
            Dim transform As GeneralTransform = castButton.TransformToVisual(TryCast(Window.Current.Content, UIElement))
            Dim pt As Point = transform.TransformPoint(New Point(0, 0))
            picker.Show(New Rect(pt.X, pt.Y, castButton.ActualWidth, castButton.ActualHeight), Windows.UI.Popups.Placement.Above)
            rootPage.NotifyUser("Cast Button Clicked", NotifyType.StatusMessage)
        End Sub

        Private Async Sub Picker_CastingDeviceSelected(sender As CastingDevicePicker, args As CastingDeviceSelectedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                'Create a casting conneciton from our selected casting device
                Dim connection As CastingConnection = args.SelectedCastingDevice.CreateCastingConnection()
                AddHandler connection.ErrorOccurred, AddressOf Connection_ErrorOccurred
                AddHandler connection.StateChanged, AddressOf Connection_StateChanged
                Await connection.RequestStartCastingAsync(video.GetAsCastingSource())
            End Sub)
        End Sub

        Private Async Sub Connection_StateChanged(sender As CastingConnection, args As Object)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Casting Connection State Changed: " & sender.State.ToString(), NotifyType.StatusMessage)
            End Sub)
        End Sub

        Private Async Sub Connection_ErrorOccurred(sender As CastingConnection, args As CastingConnectionErrorOccurredEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Casting Error Occured: " & args.Message, NotifyType.ErrorMessage)
            End Sub)
        End Sub
    End Class
End Namespace
