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

Namespace Global.BasicMediaCasting

    Public NotInheritable Partial Class Scenario1
        Inherits Page

        Private rootPage As MainPage

        Public Sub New()
            Me.InitializeComponent()
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
    End Class
End Namespace
