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
Imports Windows.ApplicationModel.DataTransfer
Imports Windows.Storage
Imports Windows.Storage.Pickers
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls

Namespace Global.SDKTemplate

    Public NotInheritable Partial Class CopyFiles
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Public Sub New()
            Me.InitializeComponent()
            Me.Init()
        End Sub

        Sub Init()
            AddHandler CopyButton.Click, New RoutedEventHandler(AddressOf CopyButton_Click)
            AddHandler PasteButton.Click, New RoutedEventHandler(AddressOf PasteButton_Click)
        End Sub

        Async Sub CopyButton_Click(sender As Object, e As RoutedEventArgs)
            OutputText.Text = "Storage Items: "
            Dim filePicker = New FileOpenPicker With {.ViewMode = PickerViewMode.List}
            filePicker.FileTypeFilter.Add("*")
            Dim storageItems = Await filePicker.PickMultipleFilesAsync()
            If storageItems.Count > 0 Then
                OutputText.Text &= storageItems.Count & " file(s) are copied into clipboard"
                Dim dataPackage = New DataPackage()
                dataPackage.SetStorageItems(storageItems)
                dataPackage.RequestedOperation = DataPackageOperation.Copy
                Try
                    Windows.ApplicationModel.DataTransfer.Clipboard.SetContent(dataPackage)
                Catch ex As Exception
                    rootPage.NotifyUser("Error copying content to Clipboard: " & ex.Message & ". Try again", NotifyType.ErrorMessage)
                End Try
            Else
                OutputText.Text &= "No file was selected."
            End If
        End Sub

        Async Sub PasteButton_Click(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser("", NotifyType.StatusMessage)
            ' Get data package from clipboard
            Dim dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.GetContent()
            If dataPackageView.Contains(StandardDataFormats.StorageItems) Then
                Dim storageItems As IReadOnlyList(Of IStorageItem) = Nothing
                Try
                    storageItems = Await dataPackageView.GetStorageItemsAsync()
                Catch ex As Exception
                    rootPage.NotifyUser("Error retrieving file(s) from Clipboard: " & ex.Message, NotifyType.ErrorMessage)
                End Try

                If storageItems IsNot Nothing Then
                    OutputText.Text = "Pasting following" & storageItems.Count & " file(s) to the folder " & ApplicationData.Current.LocalFolder.Path & Environment.NewLine
                    Dim operation = dataPackageView.RequestedOperation
                    OutputText.Text &= "Requested Operation: "
                    Select operation
                        Case DataPackageOperation.Copy
                            OutputText.Text &= "Copy"
                             Case DataPackageOperation.Link
                            OutputText.Text &= "Link"
                             Case DataPackageOperation.Move
                            OutputText.Text &= "Move"
                             Case DataPackageOperation.None
                            OutputText.Text &= "None"
                             Case Else
                            OutputText.Text &= "Unknown"
                             End Select

                    OutputText.Text &= Environment.NewLine
                    For Each storageItem In storageItems
                        Dim file = TryCast(storageItem, StorageFile)
                        If file IsNot Nothing Then
                            ' Copy the file
                            Dim newFile = Await file.CopyAsync(ApplicationData.Current.LocalFolder, file.Name, NameCollisionOption.ReplaceExisting)
                            If newFile IsNot Nothing Then
                                OutputText.Text &= file.Name & Environment.NewLine
                            End If
                        Else
                            Dim folder = TryCast(storageItem, StorageFolder)
                            If folder IsNot Nothing Then
                                OutputText.Text &= folder.Path & " is a folder, skipping" & Environment.NewLine
                            End If
                        End If
                    Next
                End If
            Else
                OutputText.Text = "StorageItems format is not available in clipboard"
            End If
        End Sub
    End Class
End Namespace
