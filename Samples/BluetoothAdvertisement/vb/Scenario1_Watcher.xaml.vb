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
Imports Windows.Devices.Bluetooth.Advertisement
Imports SDKTemplate

Namespace Global.BluetoothAdvertisement

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario1_Watcher
        Inherits Page

        ' The Bluetooth LE advertisement watcher class is used to control and customize Bluetooth LE scanning.
        Private watcher As BluetoothLEAdvertisementWatcher

        ' A pointer back to the main page is required to display status messages.
        Private rootPage As MainPage

        Public Sub New()
            Me.InitializeComponent()
            watcher = New BluetoothLEAdvertisementWatcher()
            ' Begin of watcher configuration. Configure the advertisement filter to look for the data advertised by the publisher 
            ' in Scenario 2 or 4. You need to run Scenario 2 on another Windows platform within proximity of this one for Scenario 1 to 
            ' take effect. The APIs shown in this Scenario are designed to operate only if the App is in the foreground. For background
            ' watcher operation, please refer to Scenario 3.
            ' Please comment out this following section (watcher configuration) if you want to remove all filters. By not specifying
            ' any filters, all advertisements received will be notified to the App through the event handler. You should comment out the following
            ' section if you do not have another Windows platform to run Scenario 2 alongside Scenario 1 or if you want to scan for 
            ' all LE advertisements around you.
            ' For determining the filter restrictions programatically across APIs, use the following properties:
            '      MinSamplingInterval, MaxSamplingInterval, MinOutOfRangeTimeout, MaxOutOfRangeTimeout
            ' Part 1A: Configuring the advertisement filter to watch for a particular advertisement payload
            ' First, let create a manufacturer data section we wanted to match for. These are the same as the one 
            ' created in Scenario 2 and 4.
            Dim manufacturerData = New BluetoothLEManufacturerData()
            manufacturerData.CompanyId = &HFFFE
            ' Finally set the data payload within the manufacturer-specific section
            ' Here, use a 16-bit UUID: 0x1234 -> {0x34, 0x12} (little-endian)
            Dim writer = New DataWriter()
            writer.WriteUInt16(&H1234)
            manufacturerData.Data = writer.DetachBuffer()
            watcher.AdvertisementFilter.Advertisement.ManufacturerData.Add(manufacturerData)
            watcher.SignalStrengthFilter.InRangeThresholdInDBm = -70
            watcher.SignalStrengthFilter.OutOfRangeThresholdInDBm = -75
            watcher.SignalStrengthFilter.OutOfRangeTimeout = TimeSpan.FromMilliseconds(2000)
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
            AddHandler watcher.Received, AddressOf OnAdvertisementReceived
            AddHandler watcher.Stopped, AddressOf OnAdvertisementWatcherStopped
            AddHandler App.Current.Suspending, AddressOf App_Suspending
            AddHandler App.Current.Resuming, AddressOf App_Resuming
            rootPage.NotifyUser("Press Run to start watcher.", NotifyType.StatusMessage)
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
            watcher.Stop()
            RemoveHandler watcher.Received, AddressOf OnAdvertisementReceived
            RemoveHandler watcher.Stopped, AddressOf OnAdvertisementWatcherStopped
            rootPage.NotifyUser("Navigating away. Watcher stopped.", NotifyType.StatusMessage)
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
            watcher.Stop()
            RemoveHandler watcher.Received, AddressOf OnAdvertisementReceived
            RemoveHandler watcher.Stopped, AddressOf OnAdvertisementWatcherStopped
            rootPage.NotifyUser("App suspending. Watcher stopped.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked when application execution is being resumed.
        ''' </summary>
        ''' <param name="sender">The source of the resume request.</param>
        ''' <param name="e"></param>
        Private Sub App_Resuming(sender As Object, e As Object)
            AddHandler watcher.Received, AddressOf OnAdvertisementReceived
            AddHandler watcher.Stopped, AddressOf OnAdvertisementWatcherStopped
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the Run button is pressed.
        ''' </summary>
        ''' <param name="sender">Instance that triggered the event.</param>
        ''' <param name="e">Event data describing the conditions that led to the event.</param>
        Private Sub RunButton_Click(sender As Object, e As RoutedEventArgs)
            watcher.Start()
            rootPage.NotifyUser("Watcher started.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the Stop button is pressed.
        ''' </summary>
        ''' <param name="sender">Instance that triggered the event.</param>
        ''' <param name="e">Event data describing the conditions that led to the event.</param>
        Private Sub StopButton_Click(sender As Object, e As RoutedEventArgs)
            watcher.Stop()
            rootPage.NotifyUser("Watcher stopped.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when an advertisement is received.
        ''' </summary>
        ''' <param name="watcher">Instance of watcher that triggered the event.</param>
        ''' <param name="eventArgs">Event data containing information about the advertisement event.</param>
        Private Async Sub OnAdvertisementReceived(watcher As BluetoothLEAdvertisementWatcher, eventArgs As BluetoothLEAdvertisementReceivedEventArgs)
            ' We can obtain various information about the advertisement we just received by accessing 
            ' the properties of the EventArgs class
            ' The timestamp of the event
            Dim timestamp As DateTimeOffset = eventArgs.Timestamp
            ' The type of advertisement
            Dim advertisementType As BluetoothLEAdvertisementType = eventArgs.AdvertisementType
            ' The received signal strength indicator (RSSI)
            Dim rssi As Int16 = eventArgs.RawSignalStrengthInDBm
            ' The local name of the advertising device contained within the payload, if any
            Dim localName As String = eventArgs.Advertisement.LocalName
            ' Check if there are any manufacturer-specific sections.
            ' If there is, print the raw data of the first manufacturer section (if there are multiple).
            Dim manufacturerDataString As String = ""
            Dim manufacturerSections = eventArgs.Advertisement.ManufacturerData
            If manufacturerSections.Count > 0 Then
                ' Only print the first one of the list
                Dim manufacturerData = manufacturerSections(0)
                Dim data = New Byte() {}
                Using reader = DataReader.FromBuffer(manufacturerData.Data)
                    reader.ReadBytes(data)
                End Using

                manufacturerDataString = String.Format("0x{0}: {1}", manufacturerData.CompanyId.ToString("X"), BitConverter.ToString(data))
            End If

            Await Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                ReceivedAdvertisementListBox.Items.Add(String.Format("[{0}]: type={1}, rssi={2}, name={3}, manufacturerData=[{4}]", timestamp.ToString("hh\:mm\:ss\.fff"), advertisementType.ToString(), rssi.ToString(), localName, manufacturerDataString))
            End Sub)
        End Sub

        ''' <summary>
        ''' Invoked as an event handler when the watcher is stopped or aborted.
        ''' </summary>
        ''' <param name="watcher">Instance of watcher that triggered the event.</param>
        ''' <param name="eventArgs">Event data containing information about why the watcher stopped or aborted.</param>
        Private Async Sub OnAdvertisementWatcherStopped(watcher As BluetoothLEAdvertisementWatcher, eventArgs As BluetoothLEAdvertisementWatcherStoppedEventArgs)
            Await Me.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser(String.Format("Watcher stopped or aborted: {0}", eventArgs.Error.ToString()), NotifyType.StatusMessage)
            End Sub)
        End Sub
    End Class
End Namespace
