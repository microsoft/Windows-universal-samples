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
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Background
Imports Windows.Data.Xml.Dom
Imports Windows.Networking.BackgroundTransfer
Imports Windows.Storage
Imports Windows.UI.Notifications

Namespace Global.Tasks

    Public NotInheritable Class CompletionGroupTask
        Implements IBackgroundTask

        ' Completion groups allow us to immediately take action after a set of downloads completes.
        ' In this sample, the server intentionally replies with an error status for some of the downloads.
        ' Based on the trigger details, we can determine which of the downloads have failed and try them again
        ' using a new completion group.
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            Dim details As BackgroundTransferCompletionGroupTriggerDetails = TryCast(taskInstance.TriggerDetails, BackgroundTransferCompletionGroupTriggerDetails)
            If details Is Nothing Then
                Return
            End If

            Dim failedDownloads As List(Of DownloadOperation) = New List(Of DownloadOperation)()
            Dim succeeded As Integer = 0
            For Each download In details.Downloads
                If IsFailed(download) Then
                    failedDownloads.Add(download)
                Else
                    succeeded = succeeded + 1
                End If
            Next

            If failedDownloads.Count > 0 Then
                RetryDownloads(failedDownloads)
            End If

            InvokeSimpleToast(succeeded, failedDownloads.Count)
        End Sub

        Private Function IsFailed(download As DownloadOperation) As Boolean
            Dim status As BackgroundTransferStatus = download.Progress.Status
            If status = BackgroundTransferStatus.Error OrElse status = BackgroundTransferStatus.Canceled Then
                Return True
            End If

            Dim response As ResponseInformation = download.GetResponseInformation()
            If response.StatusCode <> 200 Then
                Return True
            End If

            Return False
        End Function

        Private Sub RetryDownloads(downloads As IEnumerable(Of DownloadOperation))
            Dim downloader As BackgroundDownloader = CompletionGroupTask.CreateBackgroundDownloader()
            For Each download In downloads
                Dim download1 As DownloadOperation = downloader.CreateDownload(download.RequestedUri, download.ResultFile)
                Dim startTask As Task(Of DownloadOperation) = download1.StartAsync().AsTask()
            Next

            downloader.CompletionGroup.Enable()
        End Sub

        Public Sub InvokeSimpleToast(succeeded As Integer, failed As Integer)
            Dim toastXml As XmlDocument = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastImageAndText02)
            Dim stringElements As XmlNodeList = toastXml.GetElementsByTagName("text")
            stringElements.Item(0).AppendChild(toastXml.CreateTextNode(String.Format(CultureInfo.InvariantCulture, "{0} downloads succeeded.", succeeded)))
            stringElements.Item(1).AppendChild(toastXml.CreateTextNode(String.Format(CultureInfo.InvariantCulture, "{0} downloads failed.", failed)))
            Dim toast As ToastNotification = New ToastNotification(toastXml)
            ToastNotificationManager.CreateToastNotifier().Show(toast)
        End Sub

        Public Shared Function CreateBackgroundDownloader() As BackgroundDownloader
            Dim completionGroup As BackgroundTransferCompletionGroup = New BackgroundTransferCompletionGroup()
            Dim builder As BackgroundTaskBuilder = New BackgroundTaskBuilder()
            builder.TaskEntryPoint = "Tasks.CompletionGroupTask"
            builder.SetTrigger(completionGroup.Trigger)
            Dim taskRegistration As BackgroundTaskRegistration = builder.Register()
            Dim downloader As BackgroundDownloader = New BackgroundDownloader(completionGroup)
            Return downloader
        End Function
    End Class
End Namespace
