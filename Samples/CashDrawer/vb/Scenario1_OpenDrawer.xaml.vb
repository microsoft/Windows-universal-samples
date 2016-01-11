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
    Public NotInheritable Partial Class Scenario1_OpenDrawer
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
                        InitDrawerButton.IsEnabled = False
                        OpenDrawerButton.IsEnabled = True
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
        ''' Open the cash drawer.
        ''' </summary>
        ''' <param name="sender">Button that was clicked.</param>
        ''' <param name="e">Event data associated with click event.</param>
        Private Async Sub OpenDrawerButton_Click(sender As Object, e As RoutedEventArgs)
            If Await OpenCashDrawer() Then
                rootPage.NotifyUser("Cash drawer open succeeded.", NotifyType.StatusMessage)
            Else
                rootPage.NotifyUser("Cash drawer open failed.", NotifyType.ErrorMessage)
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
        ''' Attempt to open the claimed cash drawer.
        ''' </summary>
        ''' <returns>True if the cash drawer was successfully opened, false otherwise.</returns>
        Private Async Function OpenCashDrawer() As Task(Of Boolean)
            If claimedDrawer Is Nothing OrElse Not claimedDrawer.IsEnabled Then
                Return False
            End If

            Return Await claimedDrawer.OpenDrawerAsync()
        End Function

        ''' <summary>
        ''' Reset the scenario to its initial state.
        ''' </summary>
        Private Sub ResetScenarioState()
            drawer = Nothing
            If claimedDrawer IsNot Nothing Then
                claimedDrawer.Dispose()
                claimedDrawer = Nothing
            End If

            InitDrawerButton.IsEnabled = True
            OpenDrawerButton.IsEnabled = False
            rootPage.NotifyUser("Click the init drawer button to begin.", NotifyType.StatusMessage)
        End Sub
    End Class
End Namespace
