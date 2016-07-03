'*********************************************************
'
' Copyright (c) Microsoft. All rights reserved.
' THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
' ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
' IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
' PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
'
'*********************************************************
Imports System
Imports System.Diagnostics
Imports System.Threading
Imports Windows.ApplicationModel.Background
Imports Windows.Foundation
Imports Windows.Storage
Imports Windows.System.Threading

Namespace Global.Tasks

    '
    ' A background task always implements the IBackgroundTask interface.
    '
    Public NotInheritable Class SampleBackgroundTask
        Implements IBackgroundTask

        Dim _cancelReason As BackgroundTaskCancellationReason = BackgroundTaskCancellationReason.Abort

        Dim _cancelRequested As Boolean = False

        Dim _deferral As BackgroundTaskDeferral = Nothing

        Dim _periodicTimer As ThreadPoolTimer = Nothing

        Dim _progress As UInteger = 0

        Dim _taskInstance As IBackgroundTaskInstance = Nothing

        '
        ' The Run method is the entry point of a background task.
        '
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            Debug.WriteLine("Background " & taskInstance.Task.Name & " Starting...")
            '
            ' Query BackgroundWorkCost
            ' Guidance: If BackgroundWorkCost is high, then perform only the minimum amount
            ' of work in the background task and return immediately.
            '
            Dim cost = BackgroundWorkCost.CurrentBackgroundWorkCost
            Dim settings = ApplicationData.Current.LocalSettings
            settings.Values("BackgroundWorkCost") = cost.ToString()
            AddHandler taskInstance.Canceled, New BackgroundTaskCanceledEventHandler(AddressOf OnCanceled)
            _deferral = taskInstance.GetDeferral()
            _taskInstance = taskInstance
            _periodicTimer = ThreadPoolTimer.CreatePeriodicTimer(New TimerElapsedHandler(AddressOf PeriodicTimerCallback), TimeSpan.FromSeconds(1))
        End Sub

        '
        ' Handles background task cancellation.
        '
        Private Sub OnCanceled(sender As IBackgroundTaskInstance, reason As BackgroundTaskCancellationReason)
            _cancelRequested = True
            _cancelReason = reason
            Debug.WriteLine("Background " & sender.Task.Name & " Cancel Requested...")
        End Sub

        '
        ' Simulate the background task activity.
        '
        Private Sub PeriodicTimerCallback(timer As ThreadPoolTimer)
            If(_cancelRequested = False) AndAlso (_progress < 100) Then
                _progress += 10
                _taskInstance.Progress = _progress
            Else
                _periodicTimer.Cancel()
                Dim status As String = If((_progress < 100), "Canceled with reason: " & _cancelReason.ToString(), "Completed")
                Dim key = _taskInstance.Task.Name
                Dim settings = ApplicationData.Current.LocalSettings
                settings.Values(key) = status
                Debug.WriteLine("Background " & _taskInstance.Task.Name & status)
                _deferral.Complete()
            End If
        End Sub
    End Class
End Namespace
