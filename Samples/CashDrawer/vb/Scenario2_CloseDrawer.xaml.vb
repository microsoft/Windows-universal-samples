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
Imports System.Threading.Tasks
Imports Windows.Devices.PointOfService
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario2_CloseDrawer
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim drawer As CashDrawer = Nothing

        Dim claimedDrawer As ClaimedCashDrawer = Nothing

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            ResetScenarioState()
            MyBase.OnNavigatedTo(e)
        End Sub

        ''' <summary>
        ''' Invoked before the page is unloaded and is no longer the current source of a Frame.
        ''' </summary>
        ''' <param name="e">Event data describing the navigation that was requested.</param>
        Protected Overrides Sub OnNavigatedFrom(e As NavigationEventArgs)
            ResetScenarioState()
            MyBase.OnNavigatedFrom(e)
        End Sub

        ''' <summary>
        ''' Event handler for Initialize Drawer button.
        ''' Claims and enables the default cash drawer.
        ''' </summary>
        ''' <param name="sender">Button that was clicked.</param>
        ''' <param name="e">Event data associated with click event.</param>
        Private Async Sub InitDrawerButton_Click(sender As Object, e As RoutedEventArgs)
            If Await CreateDefaultCashDrawerObject() Then
                If Await ClaimCashDrawer() Then
                    If Await EnableCashDrawer() Then
                        AddHandler drawer.StatusUpdated, AddressOf drawer_StatusUpdated
                        InitDrawerButton.IsEnabled = False
                        DrawerWaitButton.IsEnabled = True
                        UpdateStatusOutput(drawer.Status.StatusKind)
                        rootPage.NotifyUser("Successfully enabled cash drawer. Device ID: " & claimedDrawer.DeviceId, NotifyType.StatusMessage)
                    Else
                        rootPage.NotifyUser("Failed to enable cash drawer.", NotifyType.ErrorMessage)
                    End If
                Else
                    rootPage.NotifyUser("Failed to claim cash drawer.", NotifyType.ErrorMessage)
                End If
            Else
                rootPage.NotifyUser("Cash drawer not found. Please connect a cash drawer.", NotifyType.ErrorMessage)
            End If
        End Sub

        ''' <summary>
        ''' Set up an alarm and wait for the drawer to close.
        ''' </summary>
        ''' <param name="sender">Button that was clicked.</param>
        ''' <param name="e">Event data associated with click event.</param>
        Private Async Sub WaitForDrawerCloseButton_Click(sender As Object, e As RoutedEventArgs)
            If claimedDrawer Is Nothing Then
                rootPage.NotifyUser("Drawer must be initialized.", NotifyType.ErrorMessage)
                Return
            End If

            Dim alarm = claimedDrawer.CloseAlarm
            If alarm Is Nothing Then
                rootPage.NotifyUser("Failed to create drawer alarm.", NotifyType.ErrorMessage)
                Return
            End If

            alarm.AlarmTimeout = New TimeSpan(0, 0, 30)
            alarm.BeepDelay = New TimeSpan(0, 0, 3)
            alarm.BeepDuration = New TimeSpan(0, 0, 1)
            alarm.BeepFrequency = 700
            AddHandler alarm.AlarmTimeoutExpired, AddressOf drawer_AlarmExpired
            rootPage.NotifyUser("Waiting for drawer to close.", NotifyType.StatusMessage)
            If Await alarm.StartAsync() Then
                rootPage.NotifyUser("Successfully waited for drawer close.", NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("Failed to wait for drawer close.", NotifyType.ErrorMessage)
            End If
        End Sub

        ''' <summary>
        ''' Creates the default cash drawer.
        ''' </summary>
        ''' <returns>True if the cash drawer was created, false otherwise.</returns>
        Private Async Function CreateDefaultCashDrawerObject() As Task(Of Boolean)
            rootPage.NotifyUser("Creating cash drawer object.", NotifyType.StatusMessage)
            If drawer Is Nothing Then
                drawer = Await CashDrawer.GetDefaultAsync()
                If drawer Is Nothing Then
                    Return False
                End If
            End If

            Return True
        End Function

        ''' <summary>
        ''' Attempt to claim the connected cash drawer.
        ''' </summary>
        ''' <returns>True if the cash drawer was successfully claimed, false otherwise.</returns>
        Private Async Function ClaimCashDrawer() As Task(Of Boolean)
            If drawer Is Nothing Then
                Return False
            End If

            If claimedDrawer Is Nothing Then
                claimedDrawer = Await drawer.ClaimDrawerAsync()
                If claimedDrawer Is Nothing Then
                    Return False
                End If
            End If

            Return True
        End Function

        ''' <summary>
        ''' Attempt to enabled the claimed cash drawer.
        ''' </summary>
        ''' <returns>True if the cash drawer was successfully enabled, false otherwise.</returns>        
        Private Async Function EnableCashDrawer() As Task(Of Boolean)
            If claimedDrawer Is Nothing Then
                Return False
            End If

            If claimedDrawer.IsEnabled Then
                Return True
            End If

            Return Await claimedDrawer.EnableAsync()
        End Function

        ''' <summary>
        ''' Event callback for device status updates.
        ''' </summary>
        ''' <param name="drawer">CashDrawer object sending the status update event.</param>
        ''' <param name="e">Event data associated with the status update.</param>
        Private Async Sub drawer_StatusUpdated(drawer As CashDrawer, e As CashDrawerStatusUpdatedEventArgs)
            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                UpdateStatusOutput(e.Status.StatusKind)
                rootPage.NotifyUser("Status updated event: " & e.Status.StatusKind.ToString(), NotifyType.StatusMessage)
            End Sub)
        End Sub

        ''' <summary>
        ''' Event callback for the alarm expiring.
        ''' </summary>
        ''' <param name="alarm">CashDrawerCloseAlarm that has expired.</param>
        ''' <param name="sender">Unused by AlarmTimeoutExpired events.</param>
        Private Async Sub drawer_AlarmExpired(alarm As CashDrawerCloseAlarm, sender As Object)
            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Alarm expired.  Still waiting for drawer to be closed.", NotifyType.StatusMessage)
            End Sub)
        End Sub

        ''' <summary>
        ''' Reset the scenario to its initial state.
        ''' </summary>
        Private Sub ResetScenarioState()
            drawer = Nothing
            If claimedDrawer IsNot Nothing Then
                claimedDrawer.Dispose()
                claimedDrawer = Nothing
            End If

            UpdateStatusOutput(CashDrawerStatusKind.Offline)
            InitDrawerButton.IsEnabled = True
            DrawerWaitButton.IsEnabled = False
            rootPage.NotifyUser("Click the init drawer button to begin.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Update the cash drawer text block.
        ''' </summary>
        ''' <param name="status">Cash drawer status to be displayed.</param>
        Private Sub UpdateStatusOutput(status As CashDrawerStatusKind)
            DrawerStatusBlock.Text = status.ToString()
        End Sub
    End Class
End Namespace
