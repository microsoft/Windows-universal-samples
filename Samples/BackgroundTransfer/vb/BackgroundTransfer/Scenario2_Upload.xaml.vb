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
Imports System.Threading
Imports System.Threading.Tasks
Imports SDKTemplate
Imports Windows.Networking.BackgroundTransfer
Imports Windows.Storage
Imports Windows.Storage.FileProperties
Imports Windows.Storage.Pickers
Imports Windows.UI.ViewManagement
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.Web

Namespace Global.BackgroundTransfer

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario2_Upload
        Inherits Page
        Implements IDisposable

        ' A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        ' as NotifyUser()
        Private rootPage As MainPage = MainPage.Current

        Private cts As CancellationTokenSource

        Private Const maxUploadFileSize As Integer = 100 * 1024 * 1024

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
            Await DiscoverActiveUploadsAsync()
        End Sub

        ' Enumerate the uploads that were going on in the background while the app was closed.
        Private Async Function DiscoverActiveUploadsAsync() As Task
            Dim uploads As IReadOnlyList(Of UploadOperation) = Nothing
            Try
                uploads = Await BackgroundUploader.GetCurrentUploadsAsync()
            Catch ex As Exception
                If Not IsExceptionHandled("Discovery error", ex) Then
                    Throw
                End If

                Return
            End Try

            Log("Loading background uploads: " & uploads.Count)
            If uploads.Count > 0 Then
                Dim tasks As List(Of Task) = New List(Of Task)()
                For Each upload In uploads
                    Log(String.Format(CultureInfo.CurrentCulture, "Discovered background upload: {0}, Status: {1}", upload.Guid, upload.Progress.Status))
                    tasks.Add(HandleUploadAsync(upload, False))
                Next

                Await Task.WhenAll(tasks)
            End If
        End Function

        Private Sub StartUpload_Click(sender As Object, e As RoutedEventArgs)
            ' Validating the URI is required since it was received from an untrusted source (user input).
            ' The URI is validated by calling Uri.TryCreate() that will return 'false' for strings that are not valid URIs.
            ' Note that when enabling the text box users may provide URIs to machines on the intrAnet that require
            ' the "Home or Work Networking" capability.
            Dim uri As Uri = Nothing
            If Not Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, uri) Then
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage)
                Return
            End If

            Dim picker As FileOpenPicker = New FileOpenPicker()
            picker.FileTypeFilter.Add("*")
            StartSingleFileUpload(picker, uri)
        End Sub

        Private Async Sub UploadSingleFile(uri As Uri, file As StorageFile)
            If file Is Nothing Then
                rootPage.NotifyUser("No file selected.", NotifyType.ErrorMessage)
                Return
            End If

            Dim properties As BasicProperties = Await file.GetBasicPropertiesAsync()
            If properties.Size > maxUploadFileSize Then
                rootPage.NotifyUser(String.Format(CultureInfo.CurrentCulture, "Selected file exceeds max. upload file size ({0} MB).", maxUploadFileSize / (1024 * 1024)), NotifyType.ErrorMessage)
                Return
            End If

            Dim uploader As BackgroundUploader = New BackgroundUploader()
            uploader.SetRequestHeader("Filename", file.Name)
            Dim upload As UploadOperation = uploader.CreateUpload(uri, file)
            Log(String.Format(CultureInfo.CurrentCulture, "Uploading {0} to {1}, {2}", file.Name, uri.AbsoluteUri, upload.Guid))
            Await HandleUploadAsync(upload, True)
        End Sub

        Private Sub StartMultipartUpload_Click(sender As Object, e As RoutedEventArgs)
            ' By default 'serverAddressField' is disabled and URI validation is not required. When enabling the text
            ' box validating the URI is required since it was received from an untrusted source (user input).
            ' The URI is validated by calling Uri.TryCreate() that will return 'false' for strings that are not valid URIs.
            ' Note that when enabling the text box users may provide URIs to machines on the intrAnet that require
            ' the "Home or Work Networking" capability.
            Dim uri As Uri = Nothing
            If Not Uri.TryCreate(serverAddressField.Text.Trim(), UriKind.Absolute, uri) Then
                rootPage.NotifyUser("Invalid URI.", NotifyType.ErrorMessage)
                Return
            End If

            Dim picker As FileOpenPicker = New FileOpenPicker()
            picker.FileTypeFilter.Add("*")
            StartMultipleFilesUpload(picker, uri)
        End Sub

        Private Async Sub StartSingleFileUpload(picker As FileOpenPicker, uri As Uri)
            Dim file As StorageFile = Await picker.PickSingleFileAsync()
            UploadSingleFile(uri, file)
        End Sub

        Private Async Sub StartMultipleFilesUpload(picker As FileOpenPicker, uri As Uri)
            Dim files As IReadOnlyList(Of StorageFile) = Await picker.PickMultipleFilesAsync()
            UploadMultipleFiles(uri, files)
        End Sub

        Private Async Sub UploadMultipleFiles(uri As Uri, files As IReadOnlyList(Of StorageFile))
            If files.Count = 0 Then
                rootPage.NotifyUser("No file selected.", NotifyType.ErrorMessage)
                Return
            End If

            Dim totalFileSize As ULong = 0
            For i = 0 To files.Count - 1
                Dim properties As BasicProperties = Await files(i).GetBasicPropertiesAsync()
                totalFileSize += properties.Size
                If totalFileSize > maxUploadFileSize Then
                    rootPage.NotifyUser(String.Format(CultureInfo.CurrentCulture, "Size of selected files exceeds max. upload file size ({0} MB).", maxUploadFileSize / (1024 * 1024)), NotifyType.ErrorMessage)
                    Return
                End If
            Next

            Dim parts As List(Of BackgroundTransferContentPart) = New List(Of BackgroundTransferContentPart)()
            For i = 0 To files.Count - 1
                Dim part As BackgroundTransferContentPart = New BackgroundTransferContentPart("File" & i, files(i).Name)
                part.SetFile(files(i))
                parts.Add(part)
            Next

            Dim uploader As BackgroundUploader = New BackgroundUploader()
            Dim upload As UploadOperation = Await uploader.CreateUploadAsync(uri, parts)
            Dim fileNames As String = files(0).Name
            For i = 1 To files.Count - 1
                fileNames &= ", " & files(i).Name
            Next

            Log(String.Format(CultureInfo.CurrentCulture, "Uploading {0} to {1}, {2}", fileNames, uri.AbsoluteUri, upload.Guid))
            Await HandleUploadAsync(upload, True)
        End Sub

        Private Sub CancelAll_Click(sender As Object, e As RoutedEventArgs)
            Log("Canceling all active uploads")
            cts.Cancel()
            cts.Dispose()
            cts = New CancellationTokenSource()
        End Sub

        ' Note that this event is invoked on a background thread, so we cannot access the UI directly.
        Private Sub UploadProgress(upload As UploadOperation)
            ' UploadOperation.Progress is updated in real-time while the operation is ongoing. Therefore,
            ' we must make a local copy so that we can have a consistent view of that ever-changing state
            ' throughout this Sub procedure's lifetime.
            Dim currentProgress As BackgroundUploadProgress = upload.Progress
            MarshalLog(String.Format(CultureInfo.CurrentCulture, "Progress: {0}, Status: {1}", upload.Guid, currentProgress.Status))
            Dim percentSent As Double = 100
            If currentProgress.TotalBytesToSend > 0 Then
                percentSent = currentProgress.BytesSent * 100 / currentProgress.TotalBytesToSend
            End If

            MarshalLog(String.Format(CultureInfo.CurrentCulture, " - Sent bytes: {0} of {1} ({2}%), Received bytes: {3} of {4}", currentProgress.BytesSent, currentProgress.TotalBytesToSend, percentSent, currentProgress.BytesReceived, currentProgress.TotalBytesToReceive))
            If currentProgress.HasRestarted Then
                MarshalLog(" - Upload restarted")
            End If

            If currentProgress.HasResponseChanged Then
                MarshalLog(" - Response updated; Header count: " & upload.GetResponseInformation().Headers.Count)
            End If
        End Sub

        Private Async Function HandleUploadAsync(upload As UploadOperation, start As Boolean) As Task
            Try
                LogStatus("Running: " & upload.Guid.ToString(), NotifyType.StatusMessage)
                Dim progressCallback As Progress(Of UploadOperation) = New Progress(Of UploadOperation)(AddressOf UploadProgress)
                If start Then
                    Await upload.StartAsync().AsTask(cts.Token, progressCallback)
                Else
                    Await upload.AttachAsync().AsTask(cts.Token, progressCallback)
                End If

                Dim response As ResponseInformation = upload.GetResponseInformation()
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Completed: {0}, Status Code: {1}", upload.Guid, response.StatusCode), NotifyType.StatusMessage)
            Catch ex As TaskCanceledException
                LogStatus("Canceled: " & upload.Guid.ToString(), NotifyType.StatusMessage)
            Catch ex As Exception
                If Not IsExceptionHandled("Error", ex, upload) Then
                    Throw
                End If

            End Try
        End Function

        Private Function IsExceptionHandled(title As String, ex As Exception, Optional upload As UploadOperation = Nothing) As Boolean
            Dim [error] As WebErrorStatus = BackgroundTransferError.GetStatus(ex.HResult)
            If [error] = WebErrorStatus.Unknown Then
                Return False
            End If

            If upload Is Nothing Then
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0}: {1}", title, [error]), NotifyType.ErrorMessage)
            Else
                LogStatus(String.Format(CultureInfo.CurrentCulture, "Error: {0} - {1}: {2}", upload.Guid, title, [error]), NotifyType.ErrorMessage)
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
