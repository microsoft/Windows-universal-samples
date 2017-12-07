' Copyright (c) Microsoft. All rights reserved.
Imports System
Imports System.Threading.Tasks
Imports Windows.ApplicationModel.Background
Imports Windows.Devices.Sensors
Imports Windows.Foundation
Imports Windows.Storage
Imports Windows.UI.Core
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario1_DeviceUse
        Inherits Page

        ' A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        ' as NotifyUser()
        Private rootPage As MainPage = MainPage.Current

        Private Accelerometer As Accelerometer

        Private _deviceUseTrigger As DeviceUseTrigger

        ' Used to register the device use background task
        Private _deviceUseBackgroundTaskRegistration As BackgroundTaskRegistration

        ' Used for refreshing the number of samples received when the app is visible
        Private _refreshTimer As DispatcherTimer

        Public Sub New()
            Me.InitializeComponent()
            Accelerometer = Accelerometer.GetDefault()
            If Nothing IsNot Accelerometer Then
                _deviceUseTrigger = New DeviceUseTrigger()
                _refreshTimer = New DispatcherTimer() With {.Interval = New TimeSpan(0, 0, 1)}
                AddHandler _refreshTimer.Tick, AddressOf RefreshTimer_Tick
            Else
                rootPage.NotifyUser("No accelerometer found", NotifyType.StatusMessage)
            End If
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached. The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            ' If the background task is active, start the refresh timer and activate the "Disable" button.
            ' The "IsBackgroundTaskActive" state is set by the background task.
            Dim isBackgroundTaskActive As Boolean = ApplicationData.Current.LocalSettings.Values.ContainsKey("IsBackgroundTaskActive") AndAlso CType(ApplicationData.Current.LocalSettings.Values("IsBackgroundTaskActive"), Boolean)
            ScenarioEnableButton.IsEnabled = Not isBackgroundTaskActive
            ScenarioDisableButton.IsEnabled = isBackgroundTaskActive
            If isBackgroundTaskActive Then
                _refreshTimer.Start()
            End If

            ApplicationData.Current.LocalSettings.Values("IsAppVisible") = True
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
            RemoveHandler Window.Current.VisibilityChanged, New WindowVisibilityChangedEventHandler(AddressOf VisibilityChanged)
            ApplicationData.Current.LocalSettings.Values("IsAppVisible") = False
            MyBase.OnNavigatingFrom(e)
        End Sub

        ''' <summary>
        ''' This is the event handler for VisibilityChanged events.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e">
        ''' Event data that can be examined for the current visibility state.
        ''' </param>
        Private Sub VisibilityChanged(sender As Object, e As VisibilityChangedEventArgs)
            If ScenarioDisableButton.IsEnabled Then
                ApplicationData.Current.LocalSettings.Values("IsAppVisible") = e.Visible
                If e.Visible Then
                    _refreshTimer.Start()
                Else
                    _refreshTimer.Stop()
                End If
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Enable' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub ScenarioEnable(sender As Object, e As RoutedEventArgs)
            AddHandler Window.Current.VisibilityChanged, New WindowVisibilityChangedEventHandler(AddressOf VisibilityChanged)
            If Nothing IsNot Accelerometer Then
                ' Make sure this app is allowed to run background tasks.
                ' RequestAccessAsync must be called on the UI thread.
                Dim accessStatus As BackgroundAccessStatus = Await BackgroundExecutionManager.RequestAccessAsync()
                If(BackgroundAccessStatus.AlwaysAllowed = accessStatus) OrElse (BackgroundAccessStatus.AllowedSubjectToSystemPolicy = accessStatus) Then
                    If Await StartSensorBackgroundTaskAsync(Accelerometer.DeviceId) Then
                        _refreshTimer.Start()
                        ScenarioEnableButton.IsEnabled = False
                        ScenarioDisableButton.IsEnabled = True
                    End If
                Else
                    rootPage.NotifyUser("Background tasks may be disabled for this app", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("No accelerometer found", NotifyType.StatusMessage)
            End If
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'Disable' button.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ScenarioDisable(sender As Object, e As RoutedEventArgs)
            RemoveHandler Window.Current.VisibilityChanged, New WindowVisibilityChangedEventHandler(AddressOf VisibilityChanged)
            ScenarioEnableButton.IsEnabled = True
            ScenarioDisableButton.IsEnabled = False
            _refreshTimer.Stop()
            If Nothing IsNot _deviceUseBackgroundTaskRegistration Then
                _deviceUseBackgroundTaskRegistration.Unregister(True)
                _deviceUseBackgroundTaskRegistration = Nothing
            Else
                FindAndCancelExistingBackgroundTask()
            End If

            rootPage.NotifyUser("Background task was canceled", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Starts the sensor background task.
        ''' </summary>
        ''' <param name="deviceId">Device Id for the sensor to be used by the task.</param>
        ''' <param name="e"></param>
        ''' <returns>True if the task is started successfully.</returns>
        Private Async Function StartSensorBackgroundTaskAsync(deviceId As String) As Task(Of Boolean)
            Dim started As Boolean = False
            FindAndCancelExistingBackgroundTask()
            ' Register the background task.
            Dim backgroundTaskBuilder = New BackgroundTaskBuilder() With {.Name = SampleConstants.Scenario1_DeviceUse_TaskName, .TaskEntryPoint = SampleConstants.Scenario1_DeviceUse_TaskEntryPoint}
            backgroundTaskBuilder.SetTrigger(_deviceUseTrigger)
            _deviceUseBackgroundTaskRegistration = backgroundTaskBuilder.Register()
            AddHandler _deviceUseBackgroundTaskRegistration.Completed, New BackgroundTaskCompletedEventHandler(AddressOf OnBackgroundTaskCompleted)
            Try
                ' Request a DeviceUse task to use the accelerometer.
                Dim deviceTriggerResult As DeviceTriggerResult = Await _deviceUseTrigger.RequestAsync(deviceId)
                Select Case deviceTriggerResult
                    Case DeviceTriggerResult.Allowed
                        rootPage.NotifyUser("Background task started", NotifyType.StatusMessage)
                        started = True
                    Case DeviceTriggerResult.LowBattery
                        rootPage.NotifyUser("Insufficient battery to run the background task", NotifyType.ErrorMessage)
                    Case DeviceTriggerResult.DeniedBySystem
                        rootPage.NotifyUser("The system has denied the background task request", NotifyType.ErrorMessage)
                    Case Else
                        rootPage.NotifyUser("Could not start the background task: " & deviceTriggerResult.ToString(), NotifyType.ErrorMessage)
                End Select
            Catch ex As InvalidOperationException
                rootPage.NotifyUser("A previous background task is still running, please wait for it to exit", NotifyType.ErrorMessage)
                FindAndCancelExistingBackgroundTask()
            End Try

            Return started
        End Function

        ''' <summary>
        ''' Finds a previously registered background task for this scenario and cancels it (if present)
        ''' </summary>
        Private Sub FindAndCancelExistingBackgroundTask()
            For Each backgroundTask As IBackgroundTaskRegistration In BackgroundTaskRegistration.AllTasks.Values
                If SampleConstants.Scenario1_DeviceUse_TaskName = backgroundTask.Name Then
                    Dim registration = CType(backgroundTask, BackgroundTaskRegistration)
                    registration.Unregister(True)
                    Exit For
                End If
            Next
        End Sub

        ''' <summary>
        ''' This is the tick handler for the Refresh timer.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub RefreshTimer_Tick(sender As Object, e As Object)
            If ApplicationData.Current.LocalSettings.Values.ContainsKey("SampleCount") Then
                Dim sampleCount As ULong = CType(ApplicationData.Current.LocalSettings.Values("SampleCount"), ULong)
                ScenarioOutput_SampleCount.Text = sampleCount.ToString(System.Globalization.CultureInfo.CurrentCulture)
            Else
                ScenarioOutput_SampleCount.Text = "No data"
            End If
        End Sub

        ''' <summary>
        ''' This is the background task completion handler.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub OnBackgroundTaskCompleted(sender As BackgroundTaskRegistration, e As BackgroundTaskCompletedEventArgs)
            Await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, Sub()
                Try
                    e.CheckResult()
                    If ApplicationData.Current.LocalSettings.Values.ContainsKey("TaskCancelationReason") Then
                        Dim cancelationReason As String = CType(ApplicationData.Current.LocalSettings.Values("TaskCancelationReason"), String)
                        rootPage.NotifyUser("Background task was stopped, reason: " & cancelationReason, NotifyType.StatusMessage)
                    End If
                Catch ex As Exception
                    rootPage.NotifyUser("Exception in background task: " & ex.Message, NotifyType.ErrorMessage)
                End Try

                _refreshTimer.Stop()
            End Sub)
            If Nothing IsNot _deviceUseBackgroundTaskRegistration Then
                _deviceUseBackgroundTaskRegistration.Unregister(False)
                _deviceUseBackgroundTaskRegistration = Nothing
            End If
        End Sub
    End Class
End Namespace
