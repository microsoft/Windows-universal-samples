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
Imports Windows.Storage.Streams
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation
Imports Windows.Devices.Bluetooth
Imports Windows.Devices.Bluetooth.Advertisement
Imports SDKTemplate

Namespace Global.BluetoothAdvertisement

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario2_Publisher
        Inherits Page

        ' The Bluetooth LE advertisement publisher class is used to control and customize Bluetooth LE advertising.
        Private publisher As BluetoothLEAdvertisementPublisher

        ' A pointer back to the main page is required to display status messages.
        Private rootPage As MainPage

        Public Sub New()
            Me.InitializeComponent()
            publisher = New BluetoothLEAdvertisementPublisher()
            ' We need to add some payload to the advertisement. A publisher without any payload
            ' or with invalid ones cannot be started. We only need to configure the payload once
            ' for any publisher.
            ' Add a manufacturer-specific section:
            ' First, let create a manufacturer data section
            Dim manufacturerData = New BluetoothLEManufacturerData()
            manufacturerData.CompanyId = &HFFFE
            ' Finally set the data payload within the manufacturer-specific section
            ' Here, use a 16-bit UUID: 0x1234 -> {0x34, 0x12} (little-endian)
            Dim writer = New DataWriter()
            Dim uuidData As UInt16 = &H1234
            writer.WriteUInt16(uuidData)
            manufacturerData.Data = writer.DetachBuffer()
            publisher.Advertisement.ManufacturerData.Add(manufacturerData)
            PublisherPayloadBlock.Text = String.Format("Published payload information: CompanyId=0x{0}, ManufacturerData=0x{1}", manufacturerData.CompanyId.ToString("X"), uuidData.ToString("X"))
            PublisherStatusBlock.Text = String.Format("Published Status: {0}, Error: {1}", publisher.Status, BluetoothError.Success)
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
            AddHandler publisher.StatusChanged, AddressOf OnPublisherStatusChanged
            AddHandler App.Current.Suspending, AddressOf App_Suspending
            AddHandler App.Current.Resuming, AddressOf App_Resuming
            rootPage.NotifyUser("Press Run to start publisher.", NotifyType.StatusMessage)
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
            publisher.Stop()
            RemoveHandler publisher.StatusChanged, AddressOf OnPublisherStatusChanged
            rootPage.NotifyUser("Navigating away. Publisher stopped.", NotifyType.StatusMessage)
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
            publisher.Stop()
            RemoveHandler publisher.StatusChanged, AddressOf OnPublisherStatusChanged
            rootPage.NotifyUser("App suspending. Publisher stopped.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked when application execution is being resumed.
        ''' </summary>
        ''' <param name="sender">The source of the resume request.</param>
        ''' <param name="e"></param>
        Private Sub App_Resuming(sender As Object, e As Object)
            AddHandler publisher.StatusChanged, AddressOf OnPublisherStatusChanged
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the Run button is pressed.
        ''' </summary>
        ''' <param name="sender">Instance that triggered the event.</param>
        ''' <param name="e">Event data describing the conditions that led to the event.</param>
        Private Sub RunButton_Click(sender As Object, e As RoutedEventArgs)
            publisher.Start()
            rootPage.NotifyUser("Publisher started.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the Stop button is pressed.
        ''' </summary>
        ''' <param name="sender">Instance that triggered the event.</param>
        ''' <param name="e">Event data describing the conditions that led to the event.</param>
        Private Sub StopButton_Click(sender As Object, e As RoutedEventArgs)
            publisher.Stop()
            rootPage.NotifyUser("Publisher stopped.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the status of the publisher changes.
        ''' </summary>
        ''' <param name="publisher">Instance of publisher that triggered the event.</param>
        ''' <param name="eventArgs">Event data containing information about the publisher status change event.</param>
        Private Async Sub OnPublisherStatusChanged(publisher As BluetoothLEAdvertisementPublisher, eventArgs As BluetoothLEAdvertisementPublisherStatusChangedEventArgs)
            ' This event handler can be used to monitor the status of the publisher.
            ' We can catch errors if the publisher is aborted by the system
            Dim status As BluetoothLEAdvertisementPublisherStatus = eventArgs.Status
            Dim [error] As BluetoothError = eventArgs.Error
            Await Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                PublisherStatusBlock.Text = String.Format("Published Status: {0}, Error: {1}", status.ToString(), [error].ToString())
            End Sub)
        End Sub
    End Class
End Namespace
