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
Imports System.Diagnostics
Imports System.Globalization
Imports System.IO
Imports System.Threading
Imports System.Threading.Tasks
Imports SDKTemplate
Imports Windows.Data.Xml.Dom
Imports Windows.Networking.BackgroundTransfer
Imports Windows.Storage
Imports Windows.UI.Notifications
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.Web

Namespace Global.BackgroundTransfer

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario3_Notifications
        Inherits Page

        ' A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        ' as NotifyUser()
        Private rootPage As MainPage = MainPage.Current

        Private notificationsGroup As BackgroundTransferGroup

        Private Shadows baseUri As Uri

        Private Shared runId As Integer = 0

        Private Enum ScenarioType
            Toast
            Tile
        End Enum

        Public Sub New()
            notificationsGroup = BackgroundTransferGroup.CreateGroup("{296628BF-5AE6-48CE-AA36-86A85A726B6A}")
            notificationsGroup.TransferBehavior = BackgroundTransferBehavior.Parallel
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Async Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            Await CancelActiveDownloadsAsync()
        End Sub

        Private Async Function CancelActiveDownloadsAsync() As Task
            Dim downloads As IReadOnlyList(Of DownloadOperation) = Nothing
            Try
                downloads = Await BackgroundDownloader.GetCurrentDownloadsForTransferGroupAsync(notificationsGroup)
            Catch ex As Exception
                If Not IsExceptionHandled("Discovery error", ex) Then
                    Throw
                End If

                Return
            End Try

            If downloads.Count > 0 Then
                Dim canceledToken As CancellationTokenSource = New CancellationTokenSource()
                canceledToken.Cancel()
                Dim tasks As Task() = New Task(downloads.Count - 1) {}
                For i = 0 To downloads.Count - 1
                    tasks(i) = downloads(i).AttachAsync().AsTask(canceledToken.Token)
                Next

                Try
                    Await Task.WhenAll(tasks)
                Catch ex As TaskCanceledException
                End Try

                Log(String.Format(CultureInfo.CurrentCulture, "Canceled {0} downloads from previous instances of this scenario.", downloads.Count))
            End If

            ToastNotificationButton.IsEnabled = True
            TileNotificationButton.IsEnabled = True
        End Function

        Private Async Sub ToastNotificationButton_Click(sender As Object, e As RoutedEventArgs)
            ' Create a downloader and associate all its downloads with the transfer group used for this scenario.
            Dim downloader As BackgroundDownloader = New BackgroundDownloader()
            downloader.TransferGroup = notificationsGroup
            ' Create a ToastNotification that should be shown when all transfers succeed.
            Dim successToastXml As XmlDocument = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01)
            successToastXml.GetElementsByTagName("text").Item(0).InnerText = "All three downloads completed successfully."
            Dim successToast As ToastNotification = New ToastNotification(successToastXml)
            downloader.SuccessToastNotification = successToast
            ' Create a ToastNotification that should be shown if at least one transfer fails.
            Dim failureToastXml As XmlDocument = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastText01)
            failureToastXml.GetElementsByTagName("text").Item(0).InnerText = "At least one download completed with failure."
            Dim failureToast As ToastNotification = New ToastNotification(failureToastXml)
            downloader.FailureToastNotification = failureToast
            Await RunDownloadsAsync(downloader, ScenarioType.Toast)
        End Sub

        Private Async Sub TileNotificationButton_Click(sender As Object, e As RoutedEventArgs)
            ' Create a downloader and associate all its downloads with the transfer group used for this scenario.
            Dim downloader As BackgroundDownloader = New BackgroundDownloader()
            downloader.TransferGroup = notificationsGroup
            ' Create a TileNotification that should be shown when all transfers succeed.
            Dim successTileXml As XmlDocument = TileUpdateManager.GetTemplateContent(TileTemplateType.TileSquare150x150Text03)
            Dim successTextNodes As XmlNodeList = successTileXml.GetElementsByTagName("text")
            successTextNodes.Item(0).InnerText = "All three"
            successTextNodes.Item(1).InnerText = "downloads"
            successTextNodes.Item(2).InnerText = "completed"
            successTextNodes.Item(3).InnerText = "successfully."
            Dim successTile As TileNotification = New TileNotification(successTileXml)
            successTile.ExpirationTime = DateTime.Now.AddMinutes(10)
            downloader.SuccessTileNotification = successTile
            ' Create a TileNotification that should be shown if at least one transfer fails.
            Dim failureTileXml As XmlDocument = TileUpdateManager.GetTemplateContent(TileTemplateType.TileSquare150x150Text03)
            Dim failureTextNodes As XmlNodeList = failureTileXml.GetElementsByTagName("text")
            failureTextNodes.Item(0).InnerText = "At least"
            failureTextNodes.Item(1).InnerText = "one download"
            failureTextNodes.Item(2).InnerText = "completed"
            failureTextNodes.Item(3).InnerText = "with failure."
            Dim failureTile As TileNotification = New TileNotification(failureTileXml)
            failureTile.ExpirationTime = DateTime.Now.AddMinutes(10)
            downloader.FailureTileNotification = failureTile
            Await RunDownloadsAsync(downloader, ScenarioType.Tile)
        End Sub

        Private Async Function RunDownloadsAsync(downloader As BackgroundDownloader, type As ScenarioType) As Task
            If Not Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, baseUri) Then
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage)
                Return
            End If

            runId = runId + 1
            Dim downloads As DownloadOperation() = New DownloadOperation(2) {}
            Try
                downloads(0) = Await CreateDownload(downloader, 1, String.Format(CultureInfo.InvariantCulture, "{0}.{1}.FastDownload.txt", type, runId))
                downloads(1) = Await CreateDownload(downloader, 5, String.Format(CultureInfo.InvariantCulture, "{0}.{1}.MediumDownload.txt", type, runId))
                downloads(2) = Await CreateDownload(downloader, 10, String.Format(CultureInfo.InvariantCulture, "{0}.{1}.SlowDownload.txt", type, runId))
            Catch ex As FileNotFoundException
                Return
            End Try

            ' Once all downloads participating in the toast/tile update have been created, start them.
            Dim downloadTasks As Task() = New Task(downloads.Length - 1) {}
            For i = 0 To downloads.Length - 1
                downloadTasks(i) = DownloadAsync(downloads(i))
            Next

            Await Task.WhenAll(downloadTasks)
        End Function

        Private Async Function CreateDownload(downloader As BackgroundDownloader, delaySeconds As Integer, fileName As String) As Task(Of DownloadOperation)
            Dim source As Uri = New Uri(baseUri, String.Format(CultureInfo.InvariantCulture, "?delay={0}", delaySeconds))
            Dim destinationFile As StorageFile
            Try
                destinationFile = Await KnownFolders.PicturesLibrary.CreateFileAsync(fileName, CreationCollisionOption.GenerateUniqueName)
            Catch ex As FileNotFoundException
                rootPage.NotifyUser("Error while creating file: " & ex.Message, NotifyType.ErrorMessage)
                Throw
            End Try

            Return downloader.CreateDownload(source, destinationFile)
        End Function

        Private Async Function DownloadAsync(download As DownloadOperation) As Task
            Log(String.Format(CultureInfo.CurrentCulture, "Downloading {0}", download.ResultFile.Name))
            Try
                Await download.StartAsync()
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Downloading {0} completed.", download.ResultFile.Name), NotifyType.StatusMessage)
            Catch ex As TaskCanceledException
            Catch ex As Exception
                If Not IsExceptionHandled("Execution error", ex, download) Then
                    Throw
                End If

            End Try
        End Function

        Private Function IsExceptionHandled(title As String, ex As Exception, Optional download As DownloadOperation = Nothing) As Boolean
            Dim [error] As WebErrorStatus = BackgroundTransferError.GetStatus(ex.HResult)
            If [error] = WebErrorStatus.Unknown Then
                Return False
            End If

            If download Is Nothing Then
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0}: {1}", title, [error]), NotifyType.ErrorMessage)
            Else
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0} - {1}: {2}", download.ResultFile.Name, title, [error]), NotifyType.ErrorMessage)
            End If

            Return True
        End Function

        Private Sub Log(message As String)
            outputField.Text &= message & vbCrLf
        End Sub

        Private Sub LogStatus(message As String, type As NotifyType)
            rootPage.NotifyUser(message, type)
            Log(message)
        End Sub
    End Class
End Namespace
