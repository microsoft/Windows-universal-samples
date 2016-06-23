'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System.Diagnostics
Imports System.Threading
Imports Windows.ApplicationModel.Background
Imports Windows.Storage

Namespace Global.Tasks

    '
    ' A background task always implements the IBackgroundTask interface.
    '
    Public NotInheritable Class ServicingComplete
        Implements IBackgroundTask

        Dim _cancelRequested As Boolean = False

        '
        ' The Run method is the entry point of a background task.
        '
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            Debug.WriteLine("ServicingComplete " & taskInstance.Task.Name & " starting...")
            AddHandler taskInstance.Canceled, New BackgroundTaskCanceledEventHandler(AddressOf OnCanceled)
            '
            ' Do background task activity for servicing complete.
            '
            Dim Progress As UInteger
            For Progress = 0 To 100 Step 10
                If _cancelRequested Then
                    Exit For
                End If

                taskInstance.Progress = Progress
                Progress += 10
            Next

            Dim settings = ApplicationData.Current.LocalSettings
            Dim key = taskInstance.Task.Name
            settings.Values(key) = If((Progress < 100), "Canceled", "Completed")
            Debug.WriteLine("ServicingComplete " & taskInstance.Task.Name & (If((Progress < 100), " Canceled", " Completed")))
        End Sub

        '
        ' Handles background task cancellation.
        '
        Private Sub OnCanceled(sender As IBackgroundTaskInstance, reason As BackgroundTaskCancellationReason)
            _cancelRequested = True
            Debug.WriteLine("ServicingComplete " & sender.Task.Name & " Cancel Requested...")
        End Sub
    End Class
End Namespace
