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
Imports System.Threading.Tasks
Imports Windows.Foundation.Collections
Imports Windows.UI.Xaml.Controls
Imports Windows.ApplicationModel.Background
Imports Windows.Storage
Imports Windows.UI.Xaml

Namespace Global.SDKTemplate

    Public Partial Class MainPage
        Inherits Page

        Public Const FEATURE_NAME As String = "Background Activation"

        Public ReadOnly Property scenarios As New List(Of Scenario) From {New Scenario() With {.Title = "Background Task", .ClassType = GetType(SampleBackgroundTask)}, New Scenario() With {.Title = "Background Task with Condition", .ClassType = GetType(SampleBackgroundTaskWithCondition)}, New Scenario() With {.Title = "Servicing Complete Task", .ClassType = GetType(ServicingCompleteTask)}, New Scenario() With {.Title = "Background Task with Time Trigger", .ClassType = GetType(TimeTriggeredTask)}, New Scenario() With {.Title = "Background Task with Application Trigger", .ClassType = GetType(ApplicationTriggerTask)}, New Scenario() With {.Title = "Grouped Background Task", .ClassType = GetType(GroupedBackgroundTask)}}
    End Class

    Public Class Scenario

        Public Property Title As String

        Public Property ClassType As Type
    End Class
End Namespace

Namespace Global.SDKTemplate

    Class BackgroundTaskSample

        Public Const SampleBackgroundTaskName As String = "SampleBackgroundTask"

        Public Shared SampleBackgroundTaskProgress As String = ""

        Public Shared SampleBackgroundTaskRegistered As Boolean = False

        Public Const SampleBackgroundTaskWithConditionName As String = "SampleBackgroundTaskWithCondition"

        Public Shared SampleBackgroundTaskWithConditionProgress As String = ""

        Public Shared SampleBackgroundTaskWithConditionRegistered As Boolean = False

        Public Const ServicingCompleteTaskName As String = "ServicingCompleteTask"

        Public Shared ServicingCompleteTaskProgress As String = ""

        Public Shared ServicingCompleteTaskRegistered As Boolean = False

        Public Const TimeTriggeredTaskName As String = "TimeTriggeredTask"

        Public Shared TimeTriggeredTaskProgress As String = ""

        Public Shared TimeTriggeredTaskRegistered As Boolean = False

        Public Const ApplicationTriggerTaskName As String = "ApplicationTriggerTask"

        Public Shared ApplicationTriggerTaskProgress As String = ""

        Public Shared ApplicationTriggerTaskResult As String = ""

        Public Shared ApplicationTriggerTaskRegistered As Boolean = False

        Public Const GroupedBackgroundTaskName As String = "GroupedBackgroundTask"

        Public Const BackgroundTaskGroupId As String = "3F2504E0-5F89-41D3-9A0C-0405E82C3333"

        Public Const BackgroundTaskGroupFriendlyName As String = "Background Task Group"

        Public Shared GroupedBackgroundTaskProgress As String = ""

        Public Shared GroupedBackgroundTaskRegistered As Boolean = False

        Public Shared TaskStatuses As PropertySet = New PropertySet()

        ''' <summary>
        ''' Register a background task with the specified taskEntryPoint, name, trigger,
        ''' and condition (optional).
        ''' </summary>
        ''' <param name="taskEntryPoint">Task entry point for the background task.</param>
        ''' <param name="name">A name for the background task.</param>
        ''' <param name="trigger">The trigger for the background task.</param>
        ''' <param name="condition">An optional conditional event that must be true for the task to fire.</param>
        Public Shared Function RegisterBackgroundTask(taskEntryPoint As String, name As String, trigger As IBackgroundTrigger, condition As IBackgroundCondition, Optional group As BackgroundTaskRegistrationGroup = Nothing) As BackgroundTaskRegistration
            If TaskRequiresBackgroundAccess(name) Then
                ' If the user denies access, the task will not run.
                Dim requestTask = BackgroundExecutionManager.RequestAccessAsync()
            End If

            Dim builder = New BackgroundTaskBuilder()
            builder.Name = name

            If taskEntryPoint IsNot Nothing Then
                ' If you leave the TaskEntryPoint at its default value, then the task runs
                ' inside the main process from OnBackgroundActivated rather than as a separate process.
                builder.TaskEntryPoint = taskEntryPoint
            End If

            builder.SetTrigger(trigger)
            If condition IsNot Nothing Then
                builder.AddCondition(condition)
                builder.CancelOnConditionLoss = True
            End If

            If group IsNot Nothing Then
                builder.TaskGroup = group
            End If

            Dim task As BackgroundTaskRegistration = builder.Register()
            UpdateBackgroundTaskRegistrationStatus(name, True)
            '
            ' Remove previous completion status.
            '
            TaskStatuses.Remove(name)
            Return task
        End Function

        ''' <summary>
        ''' Unregister background tasks with specified name.
        ''' </summary>
        ''' <param name="name">Name of the background task to unregister.</param>
        Public Shared Sub UnregisterBackgroundTasks(name As String, Optional group As BackgroundTaskRegistrationGroup = Nothing)

            If group IsNot Nothing Then
                For Each cur In group.AllTasks
                    If cur.Value.Name = name Then
                        cur.Value.Unregister(True)
                    End If
                Next
            Else
                For Each cur In BackgroundTaskRegistration.AllTasks
                    If cur.Value.Name = name Then
                        cur.Value.Unregister(True)
                    End If
                Next
            End If

            UpdateBackgroundTaskRegistrationStatus(name, False)
        End Sub

        ''' <summary>
        ''' Retrieve a registered background task group. If no group is registered with the given id,
        ''' then create a new one and return it.
        ''' </summary>
        ''' <param name="id"></param>
        ''' <param name="groupName"></param>
        ''' <returns>The task group associated with the given id</returns>
        Public Shared Function GetTaskGroup(id As String, groupName As String) As BackgroundTaskRegistrationGroup
            Dim group = BackgroundTaskRegistration.GetTaskGroup(id)

            If group Is Nothing Then
                group = New BackgroundTaskRegistrationGroup(id, groupName)
            End If

            Return group
        End Function

        ''' <summary>
        ''' Store the registration status of a background task with a given name.
        ''' </summary>
        ''' <param name="name">Name of background task to store registration status for.</param>
        ''' <param name="registered">TRUE if registered, FALSE if unregistered.</param>
        Public Shared Sub UpdateBackgroundTaskRegistrationStatus(name As String, registered As Boolean)
            Select name
                Case SampleBackgroundTaskName
                    SampleBackgroundTaskRegistered = registered
                Case SampleBackgroundTaskWithConditionName
                    SampleBackgroundTaskWithConditionRegistered = registered
                Case ServicingCompleteTaskName
                    ServicingCompleteTaskRegistered = registered
                Case TimeTriggeredTaskName
                    TimeTriggeredTaskRegistered = registered
                Case ApplicationTriggerTaskName
                    ApplicationTriggerTaskRegistered = registered
                Case GroupedBackgroundTaskName
                    GroupedBackgroundTaskRegistered = registered
            End Select
        End Sub

        ''' <summary>
        ''' Get the registration / completion status of the background task with
        ''' given name.
        ''' </summary>
        ''' <param name="name">Name of background task to retreive registration status.</param>
        Public Shared Function GetBackgroundTaskStatus(name As String) As String
            Dim registered = False
            Select name
                Case SampleBackgroundTaskName
                    registered = SampleBackgroundTaskRegistered
                Case SampleBackgroundTaskWithConditionName
                    registered = SampleBackgroundTaskWithConditionRegistered
                Case ServicingCompleteTaskName
                    registered = ServicingCompleteTaskRegistered
                Case TimeTriggeredTaskName
                    registered = TimeTriggeredTaskRegistered
                Case ApplicationTriggerTaskName
                    registered = ApplicationTriggerTaskRegistered
                Case GroupedBackgroundTaskName
                    registered = GroupedBackgroundTaskRegistered
            End Select

            Dim status = If(registered, "Registered", "Unregistered")
            Dim taskStatus As Object = Nothing
            If TaskStatuses.TryGetValue(name, taskStatus) Then
                status &= " - " & taskStatus.ToString()
            End If

            Return status
        End Function

        ''' <summary>
        ''' Determine if task with given name requires background access.
        ''' </summary>
        ''' <param name="name">Name of background task to query background access requirement.</param>
        Public Shared Function TaskRequiresBackgroundAccess(name As String) As Boolean
            If (name = TimeTriggeredTaskName) OrElse (name = ApplicationTriggerTaskName) Then
                Return True
            Else
                Return False
            End If
        End Function
    End Class
End Namespace

Namespace Global.SDKTemplate
    Partial Class App
        Inherits Application

        Protected Overrides Sub OnBackgroundActivated(args As BackgroundActivatedEventArgs)
            BackgroundActivity.Start(args.TaskInstance)
        End Sub

        Private Sub Construct()
            Dim group = BackgroundTaskSample.GetTaskGroup(BackgroundTaskSample.BackgroundTaskGroupId, BackgroundTaskSample.BackgroundTaskGroupFriendlyName)
            AddHandler group.BackgroundActivated, AddressOf BackgroundActivity.Start
        End Sub
    End Class
End Namespace
