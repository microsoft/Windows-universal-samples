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
Imports Windows.ApplicationModel.Background
Imports Windows.Devices.Enumeration
Imports Windows.Devices.Sensors
Imports Windows.Storage
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario4_BackgroundActivity
        Inherits Page

        Public Const SampleBackgroundTaskName As String = "ActivitySensorBackgroundTask"

        Public Const SampleBackgroundTaskEntryPoint As String = "Tasks.ActivitySensorBackgroundTask"

        Dim ActivitySensorClassId As Guid = New Guid("9D9E0118-1807-4F2E-96E4-2CE57142E196")

        Dim rootPage As MainPage = MainPage.Current

        Dim registered As Boolean = False

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            For Each task In BackgroundTaskRegistration.AllTasks
                If task.Value.Name = Scenario4_BackgroundActivity.SampleBackgroundTaskName Then
                    registered = True
                    Exit For
                End If
            Next

            UpdateUIAsync(If(registered, "Registered", "Unregistered"))
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Register Task' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Async Private Sub ScenarioRegisterTask(sender As Object, e As RoutedEventArgs)
            ' Determine if we can access activity sensors
            Dim deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(ActivitySensorClassId)
            If deviceAccessInfo.CurrentStatus = DeviceAccessStatus.Allowed Then
                ' Determine if an activity sensor is present
                ' This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
                Dim activitySensor As ActivitySensor = Await ActivitySensor.GetDefaultAsync()
                If activitySensor IsNot Nothing Then
                    Dim status = Await BackgroundExecutionManager.RequestAccessAsync()
                    If(BackgroundAccessStatus.AlwaysAllowed = status) OrElse (BackgroundAccessStatus.AllowedSubjectToSystemPolicy = status) Then
                        RegisterBackgroundTask()
                    Else
                        rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage)
                    End If
                Else
                    rootPage.NotifyUser("No activity sensors found", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("Access denied to activity sensors", NotifyType.ErrorMessage)
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Unregister Task' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ScenarioUnregisterTask(sender As Object, e As RoutedEventArgs)
            For Each cur In BackgroundTaskRegistration.AllTasks
                If SampleBackgroundTaskName = cur.Value.Name Then
                    cur.Value.Unregister(True)
                    registered = False
                End If
            Next

            If Not registered Then
                UpdateUIAsync("Unregistered")
            End If
        End Sub

        ''' <summary>
        ''' This is the event handler for background task completion.
        ''' </summary>
        ''' <param name="task">The task that is reporting completion.</param>
        ''' <param name="args">The completion report arguments.</param>
        Private Sub OnCompleted(task As IBackgroundTaskRegistration, args As BackgroundTaskCompletedEventArgs)
            Dim status As String = "Completed"
            Try
                args.CheckResult()
            Catch e As Exception
                status = e.Message
            End Try

            UpdateUIAsync(status)
        End Sub

        ''' <summary>
        ''' Registers the background task.
        ''' </summary>
        Private Sub RegisterBackgroundTask()
            Dim builder = New BackgroundTaskBuilder() With {.Name = SampleBackgroundTaskName, .TaskEntryPoint = SampleBackgroundTaskEntryPoint}
            Const reportIntervalMs As UInt32 = 3000
            Dim trigger = New ActivitySensorTrigger(reportIntervalMs)
            trigger.SubscribedActivities.Add(ActivityType.Walking)
            trigger.SubscribedActivities.Add(ActivityType.Running)
            trigger.SubscribedActivities.Add(ActivityType.Biking)
            builder.SetTrigger(trigger)
            Dim task As BackgroundTaskRegistration = builder.Register()
            AddHandler task.Completed, New BackgroundTaskCompletedEventHandler(AddressOf OnCompleted)
            registered = True
            UpdateUIAsync("Registered")
        End Sub

        ''' <summary>
        ''' Updates the scenario UI.
        ''' </summary>
        ''' <param name="status">The status string to display.</param>
        Async Private Sub UpdateUIAsync(status As String)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                ScenarioRegisterTaskButton.IsEnabled = Not registered
                ScenarioUnregisterTaskButton.IsEnabled = registered
                ScenarioOutput_TaskRegistration.Text = status
                Dim settings = ApplicationData.Current.LocalSettings
                If settings.Values.ContainsKey("ReportCount") Then
                    ScenarioOutput_ReportCount.Text = settings.Values("ReportCount").ToString()
                End If

                If settings.Values.ContainsKey("TaskStatus") Then
                    ScenarioOutput_TaskStatus.Text = settings.Values("TaskStatus").ToString()
                End If

                If settings.Values.ContainsKey("LastActivity") Then
                    ScenarioOutput_LastActivity.Text = CType(settings.Values("LastActivity"), String)
                End If

                If settings.Values.ContainsKey("LastConfidence") Then
                    ScenarioOutput_LastConfidence.Text = CType(settings.Values("LastConfidence"), String)
                End If

                If settings.Values.ContainsKey("LastTimestamp") Then
                    ScenarioOutput_LastTimestamp.Text = settings.Values("LastTimestamp").ToString()
                End If
            End Sub)
        End Sub
    End Class
End Namespace
