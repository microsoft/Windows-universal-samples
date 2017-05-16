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
Imports SDKTemplate
Imports Windows.ApplicationModel.Background
Imports Windows.Storage
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Partial Public NotInheritable Class GroupedBackgroundTask
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim group As BackgroundTaskRegistrationGroup = Nothing

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)

            group = BackgroundTaskSample.GetTaskGroup(BackgroundTaskSample.BackgroundTaskGroupId, BackgroundTaskSample.BackgroundTaskGroupFriendlyName)

            For Each task In group.AllTasks
                If task.Value.Name = BackgroundTaskSample.GroupedBackgroundTaskName Then
                    AttachProgressAndCompletedHandlers(task.Value)
                    BackgroundTaskSample.UpdateBackgroundTaskRegistrationStatus(BackgroundTaskSample.GroupedBackgroundTaskName, True)
                    Exit For
                End If
            Next

            UpdateUI()
        End Sub

        ''' <summary>
        ''' Register a Grouped Background Task.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub RegisterGroupedBackgroundTask(sender As Object, e As RoutedEventArgs)
            Dim task = BackgroundTaskSample.RegisterBackgroundTask(Nothing, BackgroundTaskSample.GroupedBackgroundTaskName, New SystemTrigger(SystemTriggerType.TimeZoneChange, False), Nothing, group)
            AttachProgressAndCompletedHandlers(task)
            UpdateUI()
        End Sub

        ''' <summary>
        ''' Unregister a Grouped Background Task.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub UnregisterGroupedTask(sender As Object, e As RoutedEventArgs)
            BackgroundTaskSample.UnregisterBackgroundTasks(BackgroundTaskSample.GroupedBackgroundTaskName, group)
            UpdateUI()
        End Sub

        ''' <summary>
        ''' Unregister all Background Tasks that are not in groups.
        ''' This will not impact tasks registered with groups.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub UnregisterUngroupedTasks(sender As Object, e As RoutedEventArgs)
            For Each cur In BackgroundTaskRegistration.AllTasks
                cur.Value.Unregister(True)
                BackgroundTaskSample.UpdateBackgroundTaskRegistrationStatus(cur.Value.Name, False)
            Next
        End Sub

        ''' <summary>
        ''' Attach progress and completed handers to a background task.
        ''' </summary>
        ''' <param name="task">The task to attach progress and completed handlers to.</param>
        Private Sub AttachProgressAndCompletedHandlers(task As IBackgroundTaskRegistration)
            AddHandler task.Progress, New BackgroundTaskProgressEventHandler(AddressOf OnProgress)
            AddHandler task.Completed, New BackgroundTaskCompletedEventHandler(AddressOf OnCompleted)
        End Sub

        ''' <summary>
        ''' Handle background task progress.
        ''' </summary>
        ''' <param name="task">The task that is reporting progress.</param>
        ''' <param name="e">Arguments of the progress report.</param>
        Private Sub OnProgress(task As IBackgroundTaskRegistration, args As BackgroundTaskProgressEventArgs)
            Dim ignored = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                                                                                 Dim progress = "Progress: " & args.Progress & "%"
                                                                                 BackgroundTaskSample.GroupedBackgroundTaskProgress = progress
                                                                                 UpdateUI()
                                                                             End Sub)
        End Sub

        ''' <summary>
        ''' Handle background task completion.
        ''' </summary>
        ''' <param name="task">The task that is reporting completion.</param>
        ''' <param name="e">Arguments of the completion report.</param>
        Private Sub OnCompleted(task As IBackgroundTaskRegistration, args As BackgroundTaskCompletedEventArgs)
            UpdateUI()
        End Sub

        ''' <summary>
        ''' Update the scenario UI.
        ''' </summary>
        Private Async Sub UpdateUI()
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                                                                         RegisterButton.IsEnabled = Not BackgroundTaskSample.GroupedBackgroundTaskRegistered
                                                                         UnregisterGroupedButton.IsEnabled = BackgroundTaskSample.GroupedBackgroundTaskRegistered
                                                                         Progress.Text = BackgroundTaskSample.GroupedBackgroundTaskProgress
                                                                         Status.Text = BackgroundTaskSample.GetBackgroundTaskStatus(BackgroundTaskSample.GroupedBackgroundTaskName)
                                                                     End Sub)
        End Sub
    End Class
End Namespace
