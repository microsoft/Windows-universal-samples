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
Imports System.Text
Imports System.Threading
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Background
Imports Windows.Devices.Bluetooth
Imports Windows.Devices.Bluetooth.Background
Imports Windows.Devices.Bluetooth.Advertisement
Imports Windows.Storage
Imports Windows.Storage.Streams

Namespace Global.BackgroundTasks

    ' A background task always implements the IBackgroundTask interface.
    Public NotInheritable Class AdvertisementWatcherTask
        Implements IBackgroundTask

        Private backgroundTaskInstance As IBackgroundTaskInstance

        ''' <summary>
        ''' The entry point of a background task.
        ''' </summary>
        ''' <param name="taskInstance">The current background task instance.</param>
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            backgroundTaskInstance = taskInstance
            Dim details = TryCast(taskInstance.TriggerDetails, BluetoothLEAdvertisementWatcherTriggerDetails)
            If details IsNot Nothing Then
                ' If the background watcher stopped unexpectedly, an error will be available here.
                Dim [error] = details.Error
                ' The Advertisements property is a list of all advertisement events received
                ' since the last task triggered. The list of advertisements here might be valid even if
                ' the Error status is not Success since advertisements are stored until this task is triggered
                Dim advertisements As IReadOnlyList(Of BluetoothLEAdvertisementReceivedEventArgs) = details.Advertisements
                ' The signal strength filter configuration of the trigger is returned such that further 
                ' processing can be performed here using these values if necessary. They are read-only here.
                Dim rssiFilter = details.SignalStrengthFilter
                ' In this example, the background task simply constructs a message communicated
                ' to the App. For more interesting applications, a notification can be sent from here instead.
                Dim eventMessage As String = ""
                eventMessage &= String.Format("ErrorStatus: {0}, EventCount: {1}, HighDBm: {2}, LowDBm: {3}, Timeout: {4}, Sampling: {5}", [error].ToString(), advertisements.Count.ToString(), rssiFilter.InRangeThresholdInDBm.ToString(), rssiFilter.OutOfRangeThresholdInDBm.ToString(), rssiFilter.OutOfRangeTimeout.GetValueOrDefault().TotalMilliseconds.ToString(), rssiFilter.SamplingInterval.GetValueOrDefault().TotalMilliseconds.ToString())
                For Each eventArgs In advertisements
                    ' Check if there are any manufacturer-specific sections.
                    ' If there is, print the raw data of the first manufacturer section (if there are multiple).
                    Dim manufacturerDataString As String = ""
                    Dim manufacturerSections = eventArgs.Advertisement.ManufacturerData
                    If manufacturerSections.Count > 0 Then
                        Dim manufacturerData = manufacturerSections(0)
                        Dim data = New Byte() {}
                        Using reader = DataReader.FromBuffer(manufacturerData.Data)
                            reader.ReadBytes(data)
                        End Using

                        manufacturerDataString = String.Format("0x{0}: {1}", manufacturerData.CompanyId.ToString("X"), BitConverter.ToString(data))
                    End If

                    eventMessage &= String.Format(vbLf & "[{0}] [{1}]: Rssi={2}dBm, localName={3}, manufacturerData=[{4}]", eventArgs.Timestamp.ToString("hh\:mm\:ss\.fff"), eventArgs.AdvertisementType.ToString(), eventArgs.RawSignalStrengthInDBm.ToString(), eventArgs.Advertisement.LocalName, manufacturerDataString)
                Next

                ApplicationData.Current.LocalSettings.Values(taskInstance.Task.Name) = eventMessage
            End If
        End Sub
    End Class

    ' A background task always implements the IBackgroundTask interface.
    Public NotInheritable Class AdvertisementPublisherTask
        Implements IBackgroundTask

        Private backgroundTaskInstance As IBackgroundTaskInstance

        ''' <summary>
        ''' The entry point of a background task.
        ''' </summary>
        ''' <param name="taskInstance">The current background task instance.</param>
        Public Sub Run(taskInstance As IBackgroundTaskInstance) Implements IBackgroundTask.Run
            backgroundTaskInstance = taskInstance
            Dim details = TryCast(taskInstance.TriggerDetails, BluetoothLEAdvertisementPublisherTriggerDetails)
            If details IsNot Nothing Then
                ' If the background publisher stopped unexpectedly, an error will be available here.
                Dim [error] = details.Error
                ' The status of the publisher is useful to determine whether the advertisement payload is being serviced
                ' It is possible for a publisher to stay in a Waiting state while radio resources are in use.
                Dim status = details.Status
                ' In this example, the background task simply constructs a message communicated
                ' to the App. For more interesting applications, a notification can be sent from here instead.
                Dim eventMessage As String = ""
                eventMessage &= String.Format("Publisher Status: {0}, Error: {1}", status.ToString(), [error].ToString())
                ApplicationData.Current.LocalSettings.Values(taskInstance.Task.Name) = eventMessage
            End If
        End Sub
    End Class
End Namespace
