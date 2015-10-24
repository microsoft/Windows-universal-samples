' Copyright (c) Microsoft. All rights reserved.
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Background
Imports Windows.Devices.Background
Imports Windows.Devices.Sensors
Imports Windows.Foundation
Imports Windows.Storage

Namespace Global.BackgroundTask

    Public NotInheritable Class Scenario1_BackgroundTask
        Implements IBackgroundTask
        Implements IDisposable

        Private Accelerometer As Accelerometer

        Private Deferral As BackgroundTaskDeferral

        Private SampleCount As ULong

        ''' <summary> 
        ''' Background task entry point.
        ''' </summary> 
        ''' <param name="taskInstance"></param>
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            Accelerometer = Accelerometer.GetDefault()
            If Nothing IsNot Accelerometer Then
                SampleCount = 0
                ' Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                Dim minReportIntervalMsecs As UInteger = Accelerometer.MinimumReportInterval
                Accelerometer.ReportInterval = If(minReportIntervalMsecs > 16, minReportIntervalMsecs, 16)
                AddHandler Accelerometer.ReadingChanged, New TypedEventHandler(Of Accelerometer, AccelerometerReadingChangedEventArgs)(AddressOf ReadingChanged)
                Deferral = taskInstance.GetDeferral()
                AddHandler taskInstance.Canceled, New BackgroundTaskCanceledEventHandler(AddressOf OnCanceled)
                ApplicationData.Current.LocalSettings.Values("IsBackgroundTaskActive") = True
            End If
        End Sub

        ''' <summary> 
        ''' Called when the background task is canceled by the app or by the system.
        ''' </summary> 
        ''' <param name="sender"></param>
        ''' <param name="reason"></param>
        Private Sub OnCanceled(sender As IBackgroundTaskInstance, reason As BackgroundTaskCancellationReason)
            ApplicationData.Current.LocalSettings.Values("TaskCancelationReason") = reason.ToString()
            ApplicationData.Current.LocalSettings.Values("SampleCount") = SampleCount
            ApplicationData.Current.LocalSettings.Values("IsBackgroundTaskActive") = False
            If Nothing IsNot Accelerometer Then
                RemoveHandler Accelerometer.ReadingChanged, New TypedEventHandler(Of Accelerometer, AccelerometerReadingChangedEventArgs)(AddressOf ReadingChanged)
                Accelerometer.ReportInterval = 0
            End If

            Deferral.Complete()
        End Sub

        ''' <summary>
        ''' Frees resources held by this background task.
        ''' </summary>
        Public Sub Dispose() Implements IDisposable.Dispose
        End Sub

        ''' <summary>
        ''' This is the event handler for acceleroemter ReadingChanged events.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ReadingChanged(sender As Object, e As AccelerometerReadingChangedEventArgs)
            SampleCount = SampleCount + 1
            ' Save the sample count if the foreground app is visible.
            Dim appVisible As Boolean = CType(ApplicationData.Current.LocalSettings.Values("IsAppVisible"), Boolean)
            If appVisible Then
                ApplicationData.Current.LocalSettings.Values("SampleCount") = SampleCount
            End If
        End Sub
    End Class
End Namespace
