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
Imports SDKTemplate
Imports System
Imports System.Collections.Generic
Imports System.Globalization
Imports System.IO
Imports System.Linq
Imports System.Runtime.InteropServices.WindowsRuntime
Imports System.Threading.Tasks
Imports Tasks
Imports Windows.ApplicationModel.Background
Imports Windows.Foundation
Imports Windows.Foundation.Collections
Imports Windows.Networking.BackgroundTransfer
Imports Windows.Storage
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Controls.Primitives
Imports Windows.UI.Xaml.Data
Imports Windows.UI.Xaml.Input
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.BackgroundTransfer

    Public NotInheritable Partial Class Scenario4_CompletionGroups
        Inherits Page

        ' A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        ' as NotifyUser()
        Private rootPage As MainPage = MainPage.Current

        Private downloadsCompleted As Integer

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Async Sub StartDownloadsButton_Click(sender As Object, e As RoutedEventArgs)
            Dim baseUri As Uri = Nothing
            If Not Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, baseUri) Then
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage)
                Return
            End If

            Dim downloader As BackgroundDownloader = CompletionGroupTask.CreateBackgroundDownloader()
            downloadsCompleted = 0
            For i = 0 To 10 - 1
                Dim uri As Uri = New Uri(baseUri, String.Format(CultureInfo.InvariantCulture, "?id={0}", i))
                Dim download As DownloadOperation = downloader.CreateDownload(uri, Await CreateResultFileAsync(i))
                Dim startTask As Task(Of DownloadOperation) = download.StartAsync().AsTask()
                Dim continueTask As Task = startTask.ContinueWith(AddressOf OnDownloadCompleted)
            Next

            downloader.CompletionGroup.Enable()
            SetSubstatus("Completion group enabled.")
        End Sub

        Private Async Function CreateResultFileAsync(id As Integer) As Task(Of IStorageFile)
            Dim resultFile As IStorageFile = Await KnownFolders.PicturesLibrary.CreateFileAsync(String.Format(CultureInfo.InvariantCulture, "picture{0}.png", id), CreationCollisionOption.GenerateUniqueName)
            Return resultFile
        End Function

        Private Sub SetSubstatus(text As String)
            Dim ignore = SubstatusBlock.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                SubstatusBlock.Text = text
            End Sub)
        End Sub

        Private Sub OnDownloadCompleted(task As Task(Of DownloadOperation))
            downloadsCompleted += 1
            Dim ignore = rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser(String.Format(CultureInfo.InvariantCulture, "{0} downloads completed.", downloadsCompleted), NotifyType.StatusMessage)
            End Sub)
        End Sub
    End Class
End Namespace
