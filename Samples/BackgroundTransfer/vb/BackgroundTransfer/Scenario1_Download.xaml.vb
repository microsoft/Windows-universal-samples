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
Imports System.Globalization
Imports System.IO
Imports System.Threading
Imports System.Threading.Tasks
Imports SDKTemplate
Imports Windows.Networking.BackgroundTransfer
Imports Windows.Storage
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.Web

Namespace Global.BackgroundTransfer

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario1_Download
        Inherits Page
        Implements IDisposable

        ' A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        ' as NotifyUser()
        Private rootPage As MainPage = MainPage.Current

        Private activeDownloads As List(Of DownloadOperation)

        Private cts As CancellationTokenSource

        Public Sub New()
            cts = New CancellationTokenSource()
            Me.InitializeComponent()
        End Sub

        Public Sub Dispose() Implements IDisposable.Dispose
            If cts IsNot Nothing Then
                cts.Dispose()
                cts = Nothing
            End If

            GC.SuppressFinalize(Me)
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Async Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            Await DiscoverActiveDownloadsAsync()
        End Sub

        ' Enumerate the downloads that were going on in the background while the app was closed.
        Private Async Function DiscoverActiveDownloadsAsync() As Task
            activeDownloads = New List(Of DownloadOperation)()
            Dim downloads As IReadOnlyList(Of DownloadOperation) = Nothing
            Try
                downloads = Await BackgroundDownloader.GetCurrentDownloadsAsync()
            Catch ex As Exception
                If Not IsExceptionHandled("Discovery error", ex) Then
                    Throw
                End If

                Return
            End Try

            Log("Loading background downloads: " & downloads.Count)
            If downloads.Count > 0 Then
                Dim tasks As List(Of Task) = New List(Of Task)()
                For Each download In downloads
                    Log(String.Format(CultureInfo.CurrentCulture, "Discovered background download: {0}, Status: {1}", download.Guid, download.Progress.Status))
                    tasks.Add(HandleDownloadAsync(download, False))
                Next

                Await Task.WhenAll(tasks)
            End If
        End Function

        Private Async Sub StartDownload(priority As BackgroundTransferPriority)
            ' Validating the URI is required since it was received from an untrusted source (user input).
            ' The URI is validated by calling Uri.TryCreate() that will return 'false' for strings that are not valid URIs.
            ' Note that when enabling the text box users may provide URIs to machines on the intrAnet that require
            ' the "Home or Work Networking" capability.
            Dim source As Uri = Nothing
            If Not Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, source) Then
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage)
                Return
            End If

            Dim destination As String = fileNameField.Text.Trim()
            If String.IsNullOrWhiteSpace(destination) Then
                rootPage.NotifyUser("A local file name is required.", NotifyType.ErrorMessage)
                Return
            End If

            Dim destinationFile As StorageFile
            Try
                destinationFile = Await KnownFolders.PicturesLibrary.CreateFileAsync(destination, CreationCollisionOption.GenerateUniqueName)
            Catch ex As FileNotFoundException
                rootPage.NotifyUser("Error while creating file: " & ex.Message, NotifyType.ErrorMessage)
                Return
            End Try

            Dim downloader As BackgroundDownloader = New BackgroundDownloader()
            Dim download As DownloadOperation = downloader.CreateDownload(source, destinationFile)
            Log(String.Format(CultureInfo.CurrentCulture, "Downloading {0} to {1} with {2} priority, {3}", source.AbsoluteUri, destinationFile.Name, priority, download.Guid))
            download.Priority = priority
            Await HandleDownloadAsync(download, True)
        End Sub

        Private Sub StartDownload_Click(sender As Object, e As RoutedEventArgs)
            StartDownload(BackgroundTransferPriority.Default)
        End Sub

        Private Sub StartHighPriorityDownload_Click(sender As Object, e As RoutedEventArgs)
            StartDownload(BackgroundTransferPriority.High)
        End Sub

        Private Sub PauseAll_Click(sender As Object, e As RoutedEventArgs)
            Log("Downloads: " & activeDownloads.Count)
            For Each download In activeDownloads
                ' DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
                ' we must make a local copy so that we can have a consistent view of that ever-changing state
                ' throughout this Sub procedure's lifetime.
                Dim currentProgress As BackgroundDownloadProgress = download.Progress
                If currentProgress.Status = BackgroundTransferStatus.Running Then
                    download.Pause()
                    Log("Paused: " & download.Guid.ToString())
                Else
                    Log(String.Format(CultureInfo.CurrentCulture, "Skipped: {0}, Status: {1}", download.Guid, currentProgress.Status))
                End If
            Next
        End Sub

        Private Sub ResumeAll_Click(sender As Object, e As RoutedEventArgs)
            Log("Downloads: " & activeDownloads.Count)
            For Each download In activeDownloads
                ' DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
                ' we must make a local copy so that we can have a consistent view of that ever-changing state
                ' throughout this Sub procedure's lifetime.
                Dim currentProgress As BackgroundDownloadProgress = download.Progress
                If currentProgress.Status = BackgroundTransferStatus.PausedByApplication Then
                    download.Resume()
                    Log("Resumed: " & download.Guid.ToString())
                Else
                    Log(String.Format(CultureInfo.CurrentCulture, "Skipped: {0}, Status: {1}", download.Guid, currentProgress.Status))
                End If
            Next
        End Sub

        Private Sub CancelAll_Click(sender As Object, e As RoutedEventArgs)
            Log("Canceling Downloads: " & activeDownloads.Count)
            cts.Cancel()
            cts.Dispose()
            cts = New CancellationTokenSource()
            activeDownloads = New List(Of DownloadOperation)()
        End Sub

        ' Note that this event is invoked on a background thread, so we cannot access the UI directly.
        Private Sub DownloadProgress(download As DownloadOperation)
            ' DownloadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
            ' we must make a local copy so that we can have a consistent view of that ever-changing state
            ' throughout this Sub procedure's lifetime.
            Dim currentProgress As BackgroundDownloadProgress = download.Progress
            MarshalLog(String.Format(CultureInfo.CurrentCulture, "Progress: {0}, Status: {1}", download.Guid, currentProgress.Status))
            Dim percent As Double = 100
            If currentProgress.TotalBytesToReceive > 0 Then
                percent = currentProgress.BytesReceived * 100 / currentProgress.TotalBytesToReceive
            End If

            MarshalLog(String.Format(CultureInfo.CurrentCulture, " - Transfered bytes: {0} of {1}, {2}%", currentProgress.BytesReceived, currentProgress.TotalBytesToReceive, percent))
            If currentProgress.HasRestarted Then
                MarshalLog(" - Download restarted")
            End If

            If currentProgress.HasResponseChanged Then
                ' We have received new response headers from the server.
                ' Be aware that GetResponseInformation() returns null for non-HTTP transfers (e.g., FTP).
                Dim response As ResponseInformation = download.GetResponseInformation()
                Dim headersCount As Integer = If(response IsNot Nothing, response.Headers.Count, 0)
                MarshalLog(" - Response updated; Header count: " & headersCount)
            End If
        End Sub

        Private Async Function HandleDownloadAsync(download As DownloadOperation, start As Boolean) As Task
            Try
                LogStatus("Running: " & download.Guid.ToString(), NotifyType.StatusMessage)
                activeDownloads.Add(download)
                Dim progressCallback As Progress(Of DownloadOperation) = New Progress(Of DownloadOperation)(AddressOf DownloadProgress)
                If start Then
                    Await download.StartAsync().AsTask(cts.Token, progressCallback)
                Else
                    Await download.AttachAsync().AsTask(cts.Token, progressCallback)
                End If

                Dim response As ResponseInformation = download.GetResponseInformation()
                ' GetResponseInformation() returns null for non-HTTP transfers (e.g., FTP).
                Dim statusCode As String = If(response IsNot Nothing, response.StatusCode.ToString(), String.Empty)
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Completed: {0}, Status Code: {1}", download.Guid, statusCode), NotifyType.StatusMessage)
            Catch ex As TaskCanceledException
                LogStatus("Canceled: " & download.Guid.ToString(), NotifyType.StatusMessage)
            Catch ex As Exception
                If Not IsExceptionHandled("Execution error", ex, download) Then
                    Throw
                End If

            Finally
                activeDownloads.Remove(download)
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
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0} - {1}: {2}", download.Guid, title, [error]), NotifyType.ErrorMessage)
            End If

            Return True
        End Function

        ' When operations happen on a background thread we have to marshal UI updates back to the UI thread.
        Private Sub MarshalLog(value As String)
            Dim ignore = Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                Log(value)
            End Sub)
        End Sub

        Private Sub Log(message As String)
            outputField.Text &= message & vbCrLf
        End Sub

        Private Sub LogStatus(message As String, type As NotifyType)
            rootPage.NotifyUser(message, type)
            Log(message)
        End Sub
    End Class
End Namespace
