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
Imports Windows.Storage
Imports Windows.Storage.Streams
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.ApplicationModel.Background
Imports Windows.Devices.Bluetooth.Advertisement
Imports Windows.Devices.Bluetooth.Background
Imports SDKTemplate

Namespace Global.BluetoothAdvertisement

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Partial Public NotInheritable Class Scenario3_BackgroundWatcher
        Inherits Page

        ' The background task registration for the background advertisement watcher
        Private taskRegistration As IBackgroundTaskRegistration

        ' The watcher trigger used to configure the background task registration
        Private trigger As BluetoothLEAdvertisementWatcherTrigger

        ' A name is given to the task in order for it to be identifiable across context.
        Private taskName As String = "Scenario3_BackgroundTask"

        ' Entry point for the background task.
        Private taskEntryPoint As String = "BackgroundTasks.AdvertisementWatcherTask"

        ' A pointer back to the main page is required to display status messages.
        Private rootPage As MainPage

        Public Sub New()
            Me.InitializeComponent()
            trigger = New BluetoothLEAdvertisementWatcherTrigger()
            ' Configure the advertisement filter to look for the data advertised by the publisher in Scenario 2 or 4.
            ' You need to run Scenario 2 on another Windows platform within proximity of this one for Scenario 3 to 
            ' take effect.
            ' Unlike the APIs in Scenario 1 which operate in the foreground. This API allows the developer to register a background
            ' task to process advertisement packets in the background. It has more restrictions on valid filter configuration.
            ' For example, exactly one single matching filter condition is allowed (no more or less) and the sampling interval
            ' For determining the filter restrictions programatically across APIs, use the following properties:
            '      MinSamplingInterval, MaxSamplingInterval, MinOutOfRangeTimeout, MaxOutOfRangeTimeout
            ' Part 1A: Configuring the advertisement filter to watch for a particular advertisement payload
            ' First, let create a manufacturer data section we wanted to match for. These are the same as the one 
            ' created in Scenario 2 and 4. Note that in the background only a single filter pattern is allowed per trigger.
            Dim manufacturerData = New BluetoothLEManufacturerData()
            manufacturerData.CompanyId = &HFFFE
            ' Finally set the data payload within the manufacturer-specific section
            ' Here, use a 16-bit UUID: 0x1234 -> {0x34, 0x12} (little-endian)
            Dim writer As DataWriter = New DataWriter()
            writer.WriteUInt16(&H1234)
            manufacturerData.Data = writer.DetachBuffer()
            trigger.AdvertisementFilter.Advertisement.ManufacturerData.Add(manufacturerData)
            trigger.SignalStrengthFilter.InRangeThresholdInDBm = -70
            trigger.SignalStrengthFilter.OutOfRangeThresholdInDBm = -75
            trigger.SignalStrengthFilter.OutOfRangeTimeout = TimeSpan.FromMilliseconds(2000)
            trigger.SignalStrengthFilter.SamplingInterval = TimeSpan.FromMilliseconds(1000)
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        '''
        ''' We will enable/disable parts of the UI if the device doesn't support it.
        ''' </summary>
        ''' <param name="eventArgs">Event data that describes how this page was reached. The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            rootPage = MainPage.Current
            If taskRegistration Is Nothing Then
                For Each task In BackgroundTaskRegistration.AllTasks.Values
                    If task.Name = taskName Then
                        taskRegistration = task
                        AddHandler taskRegistration.Completed, AddressOf OnBackgroundTaskCompleted
                        Exit For
                    End If
                Next
            Else
                AddHandler taskRegistration.Completed, AddressOf OnBackgroundTaskCompleted
            End If

            AddHandler App.Current.Suspending, AddressOf App_Suspending
            AddHandler App.Current.Resuming, AddressOf App_Resuming
            rootPage.NotifyUser("Press Run to register watcher.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        ''' </summary>
        ''' <param name="e">
        ''' Event data that can be examined by overriding code. The event data is representative
        ''' of the navigation that will unload the current Page unless canceled. The
        ''' navigation can potentially be canceled by setting Cancel.
        ''' </param>
        Protected Overrides Sub OnNavigatingFrom(e As NavigatingCancelEventArgs)
            RemoveHandler App.Current.Suspending, AddressOf App_Suspending
            RemoveHandler App.Current.Resuming, AddressOf App_Resuming
            If taskRegistration IsNot Nothing Then
                RemoveHandler taskRegistration.Completed, AddressOf OnBackgroundTaskCompleted
            End If

            MyBase.OnNavigatingFrom(e)
        End Sub

        ''' <summary>
        ''' Invoked when application execution is being suspended.  Application state is saved
        ''' without knowing whether the application will be terminated or resumed with the contents
        ''' of memory still intact.
        ''' </summary>
        ''' <param name="sender">The source of the suspend request.</param>
        ''' <param name="e">Details about the suspend request.</param>
        Private Sub App_Suspending(sender As Object, e As Windows.ApplicationModel.SuspendingEventArgs)
            If taskRegistration IsNot Nothing Then
                RemoveHandler taskRegistration.Completed, AddressOf OnBackgroundTaskCompleted
            End If

            rootPage.NotifyUser("App suspending.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked when application execution is being resumed.
        ''' </summary>
        ''' <param name="sender">The source of the resume request.</param>
        ''' <param name="e"></param>
        Private Sub App_Resuming(sender As Object, e As Object)
            If taskRegistration Is Nothing Then
                For Each task In BackgroundTaskRegistration.AllTasks.Values
                    If task.Name = taskName Then
                        taskRegistration = task
                        AddHandler taskRegistration.Completed, AddressOf OnBackgroundTaskCompleted
                        Exit For
                    End If
                Next
            Else
                AddHandler taskRegistration.Completed, AddressOf OnBackgroundTaskCompleted
            End If
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the Run button is pressed.
        ''' </summary>
        ''' <param name="sender">Instance that triggered the event.</param>
        ''' <param name="e">Event data describing the conditions that led to the event.</param>
        Private Async Sub RunButton_Click(sender As Object, e As RoutedEventArgs)
            If taskRegistration IsNot Nothing Then
                rootPage.NotifyUser("Background watcher already registered.", NotifyType.StatusMessage)
                Return
            Else
                ' Applications registering for background trigger must request for permission.
                Dim backgroundAccessStatus As BackgroundAccessStatus = Await BackgroundExecutionManager.RequestAccessAsync()
                ' Here, we do not fail the registration even if the access is not granted. Instead, we allow 
                ' the trigger to be registered and when the access is granted for the Application at a later time,
                ' the trigger will automatically start working again.
                ' At this point we assume we haven't found any existing tasks matching the one we want to register
                ' First, configure the task entry point, trigger and name
                Dim builder = New BackgroundTaskBuilder()
                builder.TaskEntryPoint = taskEntryPoint
                builder.SetTrigger(trigger)
                builder.Name = taskName
                Try
                    taskRegistration = builder.Register()
                    AddHandler taskRegistration.Completed, AddressOf OnBackgroundTaskCompleted
                    If (backgroundAccessStatus = BackgroundAccessStatus.AlwaysAllowed) OrElse (backgroundAccessStatus = BackgroundAccessStatus.AllowedSubjectToSystemPolicy) Then
                        rootPage.NotifyUser("Background watcher registered.", NotifyType.StatusMessage)
                    Else
                        rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage)
                    End If
                Catch ex As Exception
                    Select Case CType(ex.HResult, UInteger)
                        Case &H80070032UI
                            rootPage.NotifyUser("The hardware does not support background advertisement offload.", NotifyType.ErrorMessage)
                        Case Else
                            Throw ex
                    End Select

                End Try
            End If
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the Stop button is pressed.
        ''' </summary>
        ''' <param name="sender">Instance that triggered the event.</param>
        ''' <param name="e">Event data describing the conditions that led to the event.</param>
        Private Sub StopButton_Click(sender As Object, e As RoutedEventArgs)
            If taskRegistration IsNot Nothing Then
                taskRegistration.Unregister(True)
                taskRegistration = Nothing
                rootPage.NotifyUser("Background watcher unregistered.", NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("No registered background watcher found.", NotifyType.StatusMessage)
            End If
        End Sub

        ''' <summary>
        ''' Handle background task completion.
        ''' </summary>
        ''' <param name="task">The task that is reporting completion.</param>
        ''' <param name="e">Arguments of the completion report.</param>
        Private Async Sub OnBackgroundTaskCompleted(task As BackgroundTaskRegistration, eventArgs As BackgroundTaskCompletedEventArgs)
            If ApplicationData.Current.LocalSettings.Values.Keys.Contains(taskName) Then
                Dim backgroundMessage As String = CType(ApplicationData.Current.LocalSettings.Values(taskName), String)
                Await Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                                                                                                ReceivedAdvertisementListBox.Items.Add(backgroundMessage)
                                                                                            End Sub)
            End If
        End Sub
    End Class
End Namespace
