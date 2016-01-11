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
Imports System.Diagnostics
Imports Windows.ApplicationModel.Background
Imports Windows.Devices.Sensors
Imports Windows.Storage

Namespace Global.Tasks

    Public NotInheritable Class ActivitySensorBackgroundTask
        Implements IBackgroundTask

        ''' <summary>
        ''' The entry point of a background task.
        ''' </summary>
        ''' <param name="taskInstance">The current background task instance.</param>
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            Debug.WriteLine("Background " & taskInstance.Task.Name & " Starting...")
            AddHandler taskInstance.Canceled, New BackgroundTaskCanceledEventHandler(AddressOf OnCanceled)
            ' Read the activity reports
            Dim triggerDetails As ActivitySensorTriggerDetails = TryCast(taskInstance.TriggerDetails, ActivitySensorTriggerDetails)
            Dim reports = triggerDetails.ReadReports()
            Dim settings = ApplicationData.Current.LocalSettings
            settings.Values("ReportCount") = reports.Count
            If reports.Count > 0 Then
                Dim lastReading = reports(reports.Count - 1).Reading
                settings.Values("LastActivity") = lastReading.Activity.ToString()
                settings.Values("LastConfidence") = lastReading.Confidence.ToString()
                settings.Values("LastTimestamp") = lastReading.Timestamp.ToString("u")
            Else
                settings.Values("LastActivity") = "No data"
                settings.Values("LastConfidence") = "No data"
                settings.Values("LastTimestamp") = "No data"
            End If

            settings.Values("TaskStatus") = "Completed at " & DateTime.Now.ToString("u")
        End Sub

        ''' <summary>
        ''' Handles background task cancellation.
        ''' </summary>
        ''' <param name="sender">The background task instance being cancelled.</param>
        ''' <param name="reason">The cancellation reason.</param>
        Private Sub OnCanceled(sender As IBackgroundTaskInstance, reason As BackgroundTaskCancellationReason)
            Dim settings = ApplicationData.Current.LocalSettings
            Debug.WriteLine("Background " & sender.Task.Name & " Cancel Requested...")
            settings.Values("TaskStatus") = reason.ToString() & " at " & DateTime.Now.ToString("u")
        End Sub
    End Class
End Namespace
