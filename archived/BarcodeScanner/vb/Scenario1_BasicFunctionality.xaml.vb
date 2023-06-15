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
Imports System.Text
Imports System.Threading.Tasks
Imports Windows.Devices.Enumeration
Imports Windows.Devices.PointOfService
Imports Windows.Storage.Streams
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario1_BasicFunctionality
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim scanner As BarcodeScanner = Nothing

        Dim claimedScanner As ClaimedBarcodeScanner = Nothing

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        ''' <summary>
        ''' Creates the default barcode scanner.
        ''' </summary>
        ''' <returns>true if barcode scanner is created. Otherwise returns false</returns>
        Private Async Function CreateDefaultScannerObject() As Task(Of Boolean)
            If scanner Is Nothing Then
                rootPage.NotifyUser("Creating barcode scanner object.", NotifyType.StatusMessage)
                Dim deviceCollection As DeviceInformationCollection = Await DeviceInformation.FindAllAsync(BarcodeScanner.GetDeviceSelector())
                If deviceCollection IsNot Nothing AndAlso deviceCollection.Count > 0 Then
                    scanner = Await BarcodeScanner.FromIdAsync(deviceCollection(0).Id)
                    If scanner Is Nothing Then
                        rootPage.NotifyUser("Failed to create barcode scanner object.", NotifyType.ErrorMessage)
                        Return False
                    End If
                Else
                    rootPage.NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType.ErrorMessage)
                    Return False
                End If
            End If

            Return True
        End Function

        ''' <summary>
        ''' This method claims the barcode scanner 
        ''' </summary>
        ''' <returns>true if claim is successful. Otherwise returns false</returns>
        Private Async Function ClaimScanner() As Task(Of Boolean)
            If claimedScanner Is Nothing Then
                claimedScanner = Await scanner.ClaimScannerAsync()
                If claimedScanner Is Nothing Then
                    rootPage.NotifyUser("Claim barcode scanner failed.", NotifyType.ErrorMessage)
                    Return False
                End If
            End If

            Return True
        End Function

        ''' <summary>
        ''' Invoked when this page is about to be displayed in a Frame.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was reached.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedTo(e As NavigationEventArgs)
            ResetTheScenarioState()
            MyBase.OnNavigatedTo(e)
        End Sub

        ''' <summary>
        ''' Invoked when this page is no longer displayed.
        ''' </summary>
        ''' <param name="e">Event data that describes how this page was exited.  The Parameter
        ''' property is typically used to configure the page.</param>
        Protected Overrides Sub OnNavigatedFrom(e As NavigationEventArgs)
            ResetTheScenarioState()
            MyBase.OnNavigatedFrom(e)
        End Sub

        ''' <summary>
        ''' Event Handler for Start Scan Button Click.
        ''' Sets up the barcode scanner to be ready to receive the data events from the scan.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Async Sub ScenarioStartScanButton_Click(sender As Object, e As RoutedEventArgs)
            rootPage.NotifyUser("Creating barcode scanner object.", NotifyType.StatusMessage)
            If Await CreateDefaultScannerObject() Then
                If Await ClaimScanner() Then
                    AddHandler claimedScanner.ReleaseDeviceRequested, AddressOf claimedScanner_ReleaseDeviceRequested
                    AddHandler claimedScanner.DataReceived, AddressOf claimedScanner_DataReceived
                    claimedScanner.IsDecodeDataEnabled = True
                    Await claimedScanner.EnableAsync()
                    ScenarioEndScanButton.IsEnabled = True
                    ScenarioStartScanButton.IsEnabled = False
                    rootPage.NotifyUser("Ready to scan. Device ID: " & claimedScanner.DeviceId, NotifyType.StatusMessage)
                End If
            End If
        End Sub

        ''' <summary>
        ''' Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
        Async Sub claimedScanner_ReleaseDeviceRequested(sender As Object, e As ClaimedBarcodeScanner)
            e.RetainDevice()
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                rootPage.NotifyUser("Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType.StatusMessage)
            End Sub)
        End Sub

        Function GetDataString(data As IBuffer) As String
            Dim result As StringBuilder = New StringBuilder()
            If data Is Nothing Then
                result.Append("No data")
            Else
                ' Just to show that we have the raw data, we'll print the value of the bytes.
                ' Arbitrarily limit the number of bytes printed to 20 so the UI isn't overloaded.
                Const MAX_BYTES_TO_PRINT As UInteger = 20
                Dim bytesToPrint As UInteger = Math.Min(data.Length, MAX_BYTES_TO_PRINT)
                Dim reader As DataReader = DataReader.FromBuffer(data)
                Dim dataBytes As Byte() = New Byte() {}
                reader.ReadBytes(dataBytes)
                For byteIndex = 0 To bytesToPrint - 1
                    result.AppendFormat("{0:X2} ", dataBytes(byteIndex))
                Next

                If bytesToPrint < data.Length Then
                    result.Append("...")
                End If
            End If

            Return result.ToString()
        End Function

        Function GetDataLabelString(data As IBuffer, scanDataType As UInteger) As String
            Dim result As String = Nothing
            If data Is Nothing Then
                result = "No data"
            Else
                Select BarcodeSymbologies.GetName(scanDataType)
                    Case "Upca", "UpcaAdd2", "UpcaAdd5", "Upce", "UpceAdd2", "UpceAdd5", "Ean8", "TfStd"
                        ' The UPC, EAN8, and 2 of 5 families encode the digits 0..9
                        ' which are then sent to the app in a UTF8 string (like "01234")
                        ' This is not an exhaustive list of symbologies that can be converted to a string
                        Dim reader As DataReader = DataReader.FromBuffer(data)
                        result = reader.ReadString(data.Length)
                         Case Else
                        result = String.Format("Decoded data unavailable. Raw label data: {0}", GetDataString(data))
                         End Select
            End If

            Return result
        End Function

        ''' <summary>
        ''' Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner 
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
        Async Sub claimedScanner_DataReceived(sender As ClaimedBarcodeScanner, args As BarcodeScannerDataReceivedEventArgs)
            Await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                ScenarioOutputScanDataLabel.Text = GetDataLabelString(args.Report.ScanDataLabel, args.Report.ScanDataType)
                ScenarioOutputScanData.Text = GetDataString(args.Report.ScanData)
                ScenarioOutputScanDataType.Text = BarcodeSymbologies.GetName(args.Report.ScanDataType)
            End Sub)
        End Sub

        ''' <summary>
        ''' Reset the Scenario state
        ''' </summary>
        Private Sub ResetTheScenarioState()
            If claimedScanner IsNot Nothing Then
                RemoveHandler claimedScanner.DataReceived, AddressOf claimedScanner_DataReceived
                RemoveHandler claimedScanner.ReleaseDeviceRequested, AddressOf claimedScanner_ReleaseDeviceRequested
                claimedScanner.Dispose()
                claimedScanner = Nothing
            End If

            scanner = Nothing
            rootPage.NotifyUser("Click the start scanning button to begin.", NotifyType.StatusMessage)
            Me.ScenarioOutputScanData.Text = "No data"
            Me.ScenarioOutputScanDataLabel.Text = "No data"
            Me.ScenarioOutputScanDataType.Text = "No data"
            ScenarioEndScanButton.IsEnabled = False
            ScenarioStartScanButton.IsEnabled = True
        End Sub

        ''' <summary>
        ''' Event handler for End Scan Button Click. 
        ''' Releases the Barcode Scanner and resets the text in the UI
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        Private Sub ScenarioEndScanButton_Click(sender As Object, e As RoutedEventArgs)
            Me.ResetTheScenarioState()
        End Sub
    End Class
End Namespace
