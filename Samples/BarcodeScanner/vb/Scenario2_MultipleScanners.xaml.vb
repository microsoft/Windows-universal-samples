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
Imports Windows.Devices.PointOfService
Imports Windows.Storage.Streams
Imports Windows.UI
Imports Windows.UI.Xaml
Imports Windows.UI.Xaml.Controls
Imports Windows.UI.Xaml.Media
Imports Windows.UI.Xaml.Navigation

Namespace Global.SDKTemplate

    ''' <summary>
    ''' An empty page that can be used on its own or navigated to within a Frame.
    ''' </summary>
    Public NotInheritable Partial Class Scenario2_MultipleScanners
        Inherits Page

        Dim rootPage As MainPage = MainPage.Current

        Dim scannerInstance1 As BarcodeScanner = Nothing

        Dim scannerInstance2 As BarcodeScanner = Nothing

        Dim claimedBarcodeScannerInstance1 As ClaimedBarcodeScanner = Nothing

        Dim claimedBarcodeScannerInstance2 As ClaimedBarcodeScanner = Nothing

        Public Sub New()
            Me.InitializeComponent()
        End Sub

        Private Enum BarcodeScannerInstance
            Instance1
            Instance2
        End Enum

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
        ''' This is the click handler for the 'ScenarioStartScanningInstance1' button. It initiates creation of scanner instance 1.
        ''' 
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' 
        Private Async Sub ButtonStartScanningInstance1_Click(sender As Object, e As RoutedEventArgs)
            If Await CreateDefaultScannerObjectAsync(BarcodeScannerInstance.Instance1) Then
                If Await ClaimBarcodeScannerAsync(BarcodeScannerInstance.Instance1) Then
                    AddHandler claimedBarcodeScannerInstance1.ReleaseDeviceRequested, AddressOf claimedBarcodeScannerInstance1_ReleaseDeviceRequested
                    AddHandler claimedBarcodeScannerInstance1.DataReceived, AddressOf claimedBarcodeScannerInstance1_DataReceived
                    claimedBarcodeScannerInstance1.IsDecodeDataEnabled = True
                    If Await EnableBarcodeScannerAsync(BarcodeScannerInstance.Instance1) Then
                        ResetUI()
                        SetUI(BarcodeScannerInstance.Instance1)
                    End If
                Else
                    scannerInstance1 = Nothing
                End If
            End If
        End Sub

        ''' <summary>
        ''' This method is called upon when a claim request is made on instance 1. If a retain request was placed on the device it rejects the new claim.
        ''' </summary>
        ''' <param name="instance"></param>
        ''' <returns></returns>
        Async Sub claimedBarcodeScannerInstance1_ReleaseDeviceRequested(sender As Object, e As ClaimedBarcodeScanner)
            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                If Retain1.IsChecked = True Then
                    Try
                        claimedBarcodeScannerInstance1.RetainDevice()
                    Catch exception As Exception
                        rootPage.NotifyUser("Retain instance 1 failed: " & exception.Message, NotifyType.ErrorMessage)
                    End Try
                Else
                    claimedBarcodeScannerInstance1.Dispose()
                    claimedBarcodeScannerInstance1 = Nothing
                End If
            End Sub)
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'ScenarioStartScanningInstance2' button. Initiates creation of scanner instance 2
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' 
        Private Async Sub ButtonStartScanningInstance2_Click(sender As Object, e As RoutedEventArgs)
            If Await CreateDefaultScannerObjectAsync(BarcodeScannerInstance.Instance2) Then
                If Await ClaimBarcodeScannerAsync(BarcodeScannerInstance.Instance2) Then
                    AddHandler claimedBarcodeScannerInstance2.ReleaseDeviceRequested, AddressOf claimedBarcodeScannerInstance2_ReleaseDeviceRequested
                    AddHandler claimedBarcodeScannerInstance2.DataReceived, AddressOf claimedBarcodeScannerInstance2_DataReceived
                    claimedBarcodeScannerInstance2.IsDecodeDataEnabled = True
                    If Await EnableBarcodeScannerAsync(BarcodeScannerInstance.Instance2) Then
                        ResetUI()
                        SetUI(BarcodeScannerInstance.Instance2)
                    End If
                Else
                    scannerInstance2 = Nothing
                End If
            End If
        End Sub

        ''' <summary>
        ''' This method is called upon when a claim request is made on instance 2. If a retain request was placed on the device it rejects the new claim.
        ''' </summary>
        ''' <param name="instance"></param>
        ''' <returns></returns>
        Async Sub claimedBarcodeScannerInstance2_ReleaseDeviceRequested(sender As Object, e As ClaimedBarcodeScanner)
            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                If Retain2.IsChecked = True Then
                    Try
                        claimedBarcodeScannerInstance2.RetainDevice()
                    Catch exception As Exception
                        rootPage.NotifyUser("Retain instance 1 failed: " & exception.Message, NotifyType.ErrorMessage)
                    End Try
                Else
                    claimedBarcodeScannerInstance2.Dispose()
                    claimedBarcodeScannerInstance2 = Nothing
                End If
            End Sub)
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'ScenarioEndScanningInstance1' button.  
        ''' Initiates the disposal of scanner instance 1.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' 
        Private Sub ButtonEndScanningInstance1_Click(sender As Object, e As RoutedEventArgs)
            RemoveHandler claimedBarcodeScannerInstance1.DataReceived, AddressOf claimedBarcodeScannerInstance1_DataReceived
            RemoveHandler claimedBarcodeScannerInstance1.ReleaseDeviceRequested, AddressOf claimedBarcodeScannerInstance1_ReleaseDeviceRequested
            claimedBarcodeScannerInstance1.Dispose()
            claimedBarcodeScannerInstance1 = Nothing
            scannerInstance1 = Nothing
            ResetUI()
            rootPage.NotifyUser("Click a start scanning button to begin.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' This is the click handler for the 'ScenarioEndScanningInstance2' button.  
        ''' Initiates the disposal fo scanner instance 2.
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' 
        Private Sub ButtonEndScanningInstance2_Click(sender As Object, e As RoutedEventArgs)
            RemoveHandler claimedBarcodeScannerInstance2.DataReceived, AddressOf claimedBarcodeScannerInstance2_DataReceived
            RemoveHandler claimedBarcodeScannerInstance2.ReleaseDeviceRequested, AddressOf claimedBarcodeScannerInstance2_ReleaseDeviceRequested
            claimedBarcodeScannerInstance2.Dispose()
            claimedBarcodeScannerInstance2 = Nothing
            scannerInstance2 = Nothing
            ResetUI()
            rootPage.NotifyUser("Click a start scanning button to begin.", NotifyType.StatusMessage)
        End Sub

        ''' <summary>
        ''' This method returns the first available Barcode Scanner. To enumerate and find a particular device use the device enumeration code.
        ''' </summary>
        ''' <returns>a boolean value based on whether it found a compatible scanner connected</returns>
        Private Async Function CreateDefaultScannerObjectAsync(instance As BarcodeScannerInstance) As Task(Of Boolean)
            Dim scanner As BarcodeScanner = Nothing
            scanner = Await BarcodeScanner.GetDefaultAsync()
            If scanner Is Nothing Then
                rootPage.NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType.ErrorMessage)
                Return False
            End If

            Select instance
                Case BarcodeScannerInstance.Instance1
                    scannerInstance1 = scanner
                     Case BarcodeScannerInstance.Instance2
                    scannerInstance2 = scanner
                     Case Else
                    Return False
            End Select

            Return True
        End Function

        ''' <summary>
        ''' This method claims the connected scanner.
        ''' </summary>
        ''' <returns>a boolean based on whether it was able to claim the scanner.</returns>
        Private Async Function ClaimBarcodeScannerAsync(instance As BarcodeScannerInstance) As Task(Of Boolean)
            Dim bClaimAsyncStatus As Boolean = False
            Select instance
                Case BarcodeScannerInstance.Instance1
                    claimedBarcodeScannerInstance1 = Await scannerInstance1.ClaimScannerAsync()
                    If claimedBarcodeScannerInstance1 Is Nothing Then
                        rootPage.NotifyUser("Instance 1 claim barcode scanner failed.", NotifyType.ErrorMessage)
                    Else
                        bClaimAsyncStatus = True
                    End If

                     Case BarcodeScannerInstance.Instance2
                    claimedBarcodeScannerInstance2 = Await scannerInstance2.ClaimScannerAsync()
                    If claimedBarcodeScannerInstance2 Is Nothing Then
                        rootPage.NotifyUser("Instance 2 claim barcode scanner failed.", NotifyType.ErrorMessage)
                    Else
                        bClaimAsyncStatus = True
                    End If

                     Case Else
                    Return bClaimAsyncStatus
            End Select

            Return bClaimAsyncStatus
        End Function

        ''' <summary>
        ''' This method enables the connected scanner.
        ''' </summary>
        ''' <returns>a boolean based on whether it was able to enable the scanner.</returns>
        Private Async Function EnableBarcodeScannerAsync(instance As BarcodeScannerInstance) As Task(Of Boolean)
            Select instance
                Case BarcodeScannerInstance.Instance1
                    Await claimedBarcodeScannerInstance1.EnableAsync()
                    rootPage.NotifyUser("Instance 1 ready to scan. Device ID: " & claimedBarcodeScannerInstance1.DeviceId, NotifyType.StatusMessage)
                     Case BarcodeScannerInstance.Instance2
                    Await claimedBarcodeScannerInstance2.EnableAsync()
                    rootPage.NotifyUser("Instance 2 ready to scan. Device ID: " & claimedBarcodeScannerInstance2.DeviceId, NotifyType.StatusMessage)
                     Case Else
                    Return False
            End Select

            Return True
        End Function

        ''' <summary>
        ''' Reset the Scenario state
        ''' </summary>
        Private Sub ResetTheScenarioState()
            If claimedBarcodeScannerInstance1 IsNot Nothing Then
                claimedBarcodeScannerInstance1.Dispose()
                claimedBarcodeScannerInstance1 = Nothing
            End If

            scannerInstance1 = Nothing
            If claimedBarcodeScannerInstance2 IsNot Nothing Then
                claimedBarcodeScannerInstance2.Dispose()
                claimedBarcodeScannerInstance2 = Nothing
            End If

            scannerInstance2 = Nothing
            ResetUI()
        End Sub

        ''' <summary>
        ''' Resets the display Elements to original state
        ''' </summary>
        Private Sub ResetUI()
            Instance1Border.BorderBrush = New SolidColorBrush(Colors.Gray)
            Instance2Border.BorderBrush = New SolidColorBrush(Colors.Gray)
            ScanDataType1.Text = String.Format("No data")
            ScanData1.Text = String.Format("No data")
            DataLabel1.Text = String.Format("No data")
            ScanDataType2.Text = String.Format("No data")
            ScanData2.Text = String.Format("No data")
            DataLabel2.Text = String.Format("No data")
            ScenarioStartScanningInstance1.IsEnabled = True
            ScenarioStartScanningInstance2.IsEnabled = True
            ScenarioEndScanningInstance1.IsEnabled = False
            ScenarioEndScanningInstance2.IsEnabled = False
        End Sub

        ''' <summary>
        ''' Sets the UI elements to a state corresponding to the current active Instance.
        ''' </summary>
        ''' <param name="instance">Corresponds to the current active instance</param>
        Private Async Sub SetUI(instance As BarcodeScannerInstance)
            Instance1Border.BorderBrush = New SolidColorBrush(Colors.Gray)
            Instance2Border.BorderBrush = New SolidColorBrush(Colors.Gray)
            Select instance
                Case BarcodeScannerInstance.Instance1
                    Await rootPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                        ScenarioStartScanningInstance1.IsEnabled = False
                        ScenarioStartScanningInstance2.IsEnabled = True
                        ScenarioEndScanningInstance1.IsEnabled = True
                        ScenarioEndScanningInstance2.IsEnabled = False
                        Instance1Border.BorderBrush = New SolidColorBrush(Colors.DarkBlue)
                    End Sub)
                     Case BarcodeScannerInstance.Instance2
                    Await rootPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                        ScenarioStartScanningInstance1.IsEnabled = True
                        ScenarioStartScanningInstance2.IsEnabled = False
                        ScenarioEndScanningInstance1.IsEnabled = False
                        ScenarioEndScanningInstance2.IsEnabled = True
                        Instance2Border.BorderBrush = New SolidColorBrush(Colors.DarkBlue)
                    End Sub)
                     Case Else
                     End Select
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
        ''' This is an event handler for the claimed scanner Instance 1 when it scans and recieves data
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Private Async Sub claimedBarcodeScannerInstance1_DataReceived(sender As ClaimedBarcodeScanner, args As BarcodeScannerDataReceivedEventArgs)
            ' Grab the data from the IBuffers
            Dim scanData As String = String.Empty
            Dim scanDataLabel As String = String.Empty
            If args.Report.ScanData IsNot Nothing Then
                scanData = GetDataString(args.Report.ScanData)
            End If

            If args.Report.ScanDataLabel IsNot Nothing Then
                scanDataLabel = GetDataLabelString(args.Report.ScanDataLabel, args.Report.ScanDataType)
            End If

            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                ScanDataType1.Text = String.Format("{0}", BarcodeSymbologies.GetName(args.Report.ScanDataType))
                DataLabel1.Text = String.Format("{0}", scanDataLabel)
                ScanData1.Text = String.Format("{0}", scanData)
                rootPage.NotifyUser("Instance 1 received data from the barcode scanner.", NotifyType.StatusMessage)
            End Sub)
        End Sub

        ''' <summary>
        ''' This is an event handler for the claimed scanner Instance 2 when it scans and recieves data
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="args"></param>
        Private Async Sub claimedBarcodeScannerInstance2_DataReceived(sender As ClaimedBarcodeScanner, args As BarcodeScannerDataReceivedEventArgs)
            ' Grab the data from the IBuffers
            Dim scanData As String = String.Empty
            Dim scanDataLabel As String = String.Empty
            If args.Report.ScanData IsNot Nothing Then
                scanData = GetDataString(args.Report.ScanData)
            End If

            If args.Report.ScanDataLabel IsNot Nothing Then
                scanDataLabel = GetDataLabelString(args.Report.ScanDataLabel, args.Report.ScanDataType)
            End If

            Await MainPage.Current.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, Sub()
                ScanDataType2.Text = String.Format("{0}", BarcodeSymbologies.GetName(args.Report.ScanDataType))
                DataLabel2.Text = String.Format("{0}", scanDataLabel)
                ScanData2.Text = String.Format("{0}", scanData)
                rootPage.NotifyUser("Instance 2 received data from the barcode scanner.", NotifyType.StatusMessage)
            End Sub)
        End Sub
    End Class
End Namespace
