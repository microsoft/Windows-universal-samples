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
Imports Windows.Storage
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports SDKTemplate

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class SampleBackgroundTask
        Inherits Page

        ' A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        ' as NotifyUser()
        Private rootPage As MainPage = MainPage.Current

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
                If task.Value.Name = BackgroundTaskSample.SampleBackgroundTaskName Then
                    AttachProgressAndCompletedHandlers(task.Value)
                    BackgroundTaskSample.UpdateBackgroundTaskRegistrationStatus(BackgroundTaskSample.SampleBackgroundTaskName, True)
                    Exit For
                End If
            Next

            UpdateUI()
        End Sub

        ''' <summary>
        ''' Register a SampleBackgroundTask.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub RegisterBackgroundTask(sender As Object, e As RoutedEventArgs)
            Dim task = BackgroundTaskSample.RegisterBackgroundTask(BackgroundTaskSample.SampleBackgroundTaskEntryPoint, BackgroundTaskSample.SampleBackgroundTaskName, New SystemTrigger(SystemTriggerType.TimeZoneChange, False), Nothing)
            AttachProgressAndCompletedHandlers(task)
            UpdateUI()
        End Sub

        ''' <summary>
        ''' Unregister a SampleBackgroundTask.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub UnregisterBackgroundTask(sender As Object, e As RoutedEventArgs)
            BackgroundTaskSample.UnregisterBackgroundTasks(BackgroundTaskSample.SampleBackgroundTaskName)
            UpdateUI()
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
                BackgroundTaskSample.SampleBackgroundTaskProgress = progress
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
                RegisterButton.IsEnabled = Not BackgroundTaskSample.SampleBackgroundTaskRegistered
                UnregisterButton.IsEnabled = BackgroundTaskSample.SampleBackgroundTaskRegistered
                Progress.Text = BackgroundTaskSample.SampleBackgroundTaskProgress
                Status.Text = BackgroundTaskSample.GetBackgroundTaskStatus(BackgroundTaskSample.SampleBackgroundTaskName)
            End Sub)
        End Sub
    End Class
End Namespace
