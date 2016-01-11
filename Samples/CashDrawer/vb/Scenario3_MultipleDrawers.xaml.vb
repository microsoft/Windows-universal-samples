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
    Public NotInheritable Partial Class Scenario3_MultipleDrawers
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim cashDrawerInstance1 As CashDrawer = Nothing

        Dim cashDrawerInstance2 As CashDrawer = Nothing

        Dim claimedCashDrawerInstance1 As ClaimedCashDrawer = Nothing

        Dim claimedCashDrawerInstance2 As ClaimedCashDrawer = Nothing

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Enum CashDrawerInstance
            Instance1
            Instance2
        End Enum

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
        ''' Creates the default cash drawer.
        ''' </summary>
        ''' <param name="instance">Specifies the cash drawer instance that should be used.</param>
        ''' <returns>True if the cash drawer was created, false otherwise.</returns>
        Private Async Function CreateDefaultCashDrawerObject(instance As CashDrawerInstance) As Task(Of Boolean)
            rootPage.NotifyUser("Creating cash drawer object.", NotifyType.StatusMessage)
            Dim tempDrawer As CashDrawer = Nothing
            tempDrawer = Await CashDrawer.GetDefaultAsync()
            If tempDrawer Is Nothing Then
                rootPage.NotifyUser("Cash drawer not found. Please connect a cash drawer.", NotifyType.ErrorMessage)
                Return False
            End If

            Select instance
                Case CashDrawerInstance.Instance1
                    cashDrawerInstance1 = tempDrawer
                     Case CashDrawerInstance.Instance2
                    cashDrawerInstance2 = tempDrawer
                     Case Else
                    Return False
            End Select

            Return True
        End Function

        ''' <summary>
        ''' Attempt to claim the connected cash drawer.
        ''' </summary>
        ''' <param name="instance">Specifies the cash drawer instance that should be used.</param>
        ''' <returns>True if the cash drawer was successfully claimed, false otherwise.</returns>
        Private Async Function ClaimCashDrawer(instance As CashDrawerInstance) As Task(Of Boolean)
            Dim claimAsyncStatus As Boolean = False
            Select instance
                Case CashDrawerInstance.Instance1
                    claimedCashDrawerInstance1 = Await cashDrawerInstance1.ClaimDrawerAsync()
                    If claimedCashDrawerInstance1 IsNot Nothing Then
                        rootPage.NotifyUser("Successfully claimed instance 1.", NotifyType.StatusMessage)
                        claimAsyncStatus = True
                    Else
                        rootPage.NotifyUser("Failed to claim instance 1.", NotifyType.ErrorMessage)
                    End If

                     Case CashDrawerInstance.Instance2
                    claimedCashDrawerInstance2 = Await cashDrawerInstance2.ClaimDrawerAsync()
                    If claimedCashDrawerInstance2 IsNot Nothing Then
                        rootPage.NotifyUser("Successfully claimed instance 2.", NotifyType.StatusMessage)
                        claimAsyncStatus = True
                    Else
                        rootPage.NotifyUser("Failed to claim instance 2.", NotifyType.ErrorMessage)
                    End If

                     Case Else
                     End Select

            Return claimAsyncStatus
        End Function

        ''' <summary>
        ''' Event callback for claim instance 1 button.
        ''' </summary>
        ''' <param name="sender">Button that was clicked.</param>
        ''' <param name="e">Event data associated with click event.</param>
        Private Async Sub ClaimButton1_Click(sender As Object, e As RoutedEventArgs)
            If Await CreateDefaultCashDrawerObject(CashDrawerInstance.Instance1) Then
                If Await ClaimCashDrawer(CashDrawerInstance.Instance1) Then
                    AddHandler claimedCashDrawerInstance1.ReleaseDeviceRequested, AddressOf claimedCashDrawerInstance1_ReleaseDeviceRequested
                    SetClaimedUI(CashDrawerInstance.Instance1)
                Else
                    cashDrawerInstance2 = Nothing
                End If
            End If
        End Sub

        ''' <summary>
        ''' Event callback for claim instance 2 button.
        ''' </summary>
        ''' <param name="sender">Button that was clicked.</param>
        ''' <param name="e">Event data associated with click event.</param>
        Private Async Sub ClaimButton2_Click(sender As Object, e As RoutedEventArgs)
            If Await CreateDefaultCashDrawerObject(CashDrawerInstance.Instance2) Then
                If Await ClaimCashDrawer(CashDrawerInstance.Instance2) Then
                    AddHandler claimedCashDrawerInstance2.ReleaseDeviceRequested, AddressOf claimedCashDrawerInstance2_ReleaseDeviceRequested
                    SetClaimedUI(CashDrawerInstance.Instance2)
                Else
                    cashDrawerInstance2 = Nothing
                End If
            End If
        End Sub

        ''' <summary>
        ''' Event callback for release instance 1 button.
        ''' </summary>
        ''' <param name="sender">Button that was clicked.</param>
        ''' <param name="e">Event data associated with click event.</param>
        Private Sub ReleaseButton1_Click(sender As Object, e As RoutedEventArgs)
            If claimedCashDrawerInstance1 IsNot Nothing Then
                claimedCashDrawerInstance1.Dispose()
                claimedCashDrawerInstance1 = Nothing
                SetReleasedUI(CashDrawerInstance.Instance1)
                rootPage.NotifyUser("Claimed instance 1 was released.", NotifyType.StatusMessage)
            End If
        End Sub

        ''' <summary>
        ''' Event callback for release instance 2 button.
        ''' </summary>
        ''' <param name="sender">Button that was clicked.</param>
        ''' <param name="e">Event data associated with click event.</param>
        Private Sub ReleaseButton2_Click(sender As Object, e As RoutedEventArgs)
            If claimedCashDrawerInstance2 IsNot Nothing Then
                claimedCashDrawerInstance2.Dispose()
                claimedCashDrawerInstance2 = Nothing
                SetReleasedUI(CashDrawerInstance.Instance2)
                rootPage.NotifyUser("Claimed instance 2 was released.", NotifyType.StatusMessage)
            End If
        End Sub

        ''' <summary>
        ''' Event callback for a release device request for instance 1.
        ''' </summary>
        ''' <param name="sender">The drawer receiving the release device request.</param>
        ''' <param name="e">Unused for ReleaseDeviceRequested events.</param>
        Private Async Sub claimedCashDrawerInstance1_ReleaseDeviceRequested(sender As ClaimedCashDrawer, e As Object)
            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                rootPage.NotifyUser("Release instance 1 requested.", NotifyType.StatusMessage)
                If RetainCheckBox1.IsChecked = True Then
                    If Await claimedCashDrawerInstance1.RetainDeviceAsync() = False Then
                        rootPage.NotifyUser("Cash drawer instance 1 retain failed.", NotifyType.ErrorMessage)
                    End If
                Else
                    claimedCashDrawerInstance1.Dispose()
                    claimedCashDrawerInstance1 = Nothing
                    SetReleasedUI(CashDrawerInstance.Instance1)
                End If
            End Sub)
        End Sub

        ''' <summary>
        ''' Event callback for a release device request for instance 2.
        ''' </summary>
        ''' <param name="sender">The drawer receiving the release device request.</param>
        ''' <param name="e">Unused for ReleaseDeviceRequested events.</param>
        Private Async Sub claimedCashDrawerInstance2_ReleaseDeviceRequested(drawer As ClaimedCashDrawer, e As Object)
            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Async Sub()
                rootPage.NotifyUser("Release instance 2 requested.", NotifyType.StatusMessage)
                If RetainCheckBox2.IsChecked = True Then
                    If Await claimedCashDrawerInstance2.RetainDeviceAsync() = False Then
                        rootPage.NotifyUser("Cash drawer instance 2 retain failed.", NotifyType.ErrorMessage)
                    End If
                Else
                    claimedCashDrawerInstance2.Dispose()
                    claimedCashDrawerInstance2 = Nothing
                    SetReleasedUI(CashDrawerInstance.Instance2)
                End If
            End Sub)
        End Sub

        ''' <summary>
        ''' Resets the display elements to original state
        ''' </summary>
        Private Sub ResetScenarioState()
            cashDrawerInstance1 = Nothing
            cashDrawerInstance2 = Nothing
            If claimedCashDrawerInstance1 IsNot Nothing Then
                claimedCashDrawerInstance1.Dispose()
                claimedCashDrawerInstance1 = Nothing
            End If

            If claimedCashDrawerInstance2 IsNot Nothing Then
                claimedCashDrawerInstance2.Dispose()
                claimedCashDrawerInstance2 = Nothing
            End If

            ClaimButton1.IsEnabled = True
            ClaimButton2.IsEnabled = True
            ReleaseButton1.IsEnabled = False
            ReleaseButton2.IsEnabled = False
            RetainCheckBox1.IsChecked = True
            RetainCheckBox2.IsChecked = True
            rootPage.NotifyUser("Click a claim button to begin.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Sets the UI elements to a state corresponding to the specified instace being claimed.
        ''' </summary>
        ''' <param name="instance">Corresponds to instance that has been claimed.</param>
        Private Sub SetClaimedUI(instance As CashDrawerInstance)
            Select instance
                Case CashDrawerInstance.Instance1
                    ClaimButton1.IsEnabled = False
                    ClaimButton2.IsEnabled = True
                    ReleaseButton1.IsEnabled = True
                    ReleaseButton2.IsEnabled = False
                     Case CashDrawerInstance.Instance2
                    ClaimButton1.IsEnabled = True
                    ClaimButton2.IsEnabled = False
                    ReleaseButton1.IsEnabled = False
                    ReleaseButton2.IsEnabled = True
                     Case Else
                     End Select
        End Sub

        ''' <summary>
        ''' Sets the UI elements to a state corresponding to the specified instace being released.
        ''' </summary>
        ''' <param name="instance">Corresponds to instance that has been released.</param>
        Private Sub SetReleasedUI(instance As CashDrawerInstance)
            Select instance
                Case CashDrawerInstance.Instance1
                    ClaimButton1.IsEnabled = True
                    ReleaseButton1.IsEnabled = False
                     Case CashDrawerInstance.Instance2
                    ClaimButton2.IsEnabled = True
                    ReleaseButton2.IsEnabled = False
                     Case Else
                     End Select
        End Sub
    End Class
End Namespace
