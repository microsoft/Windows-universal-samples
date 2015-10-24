//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_MultipleScanners : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        BarcodeScanner scannerInstance1 = null;
        BarcodeScanner scannerInstance2 = null;
        ClaimedBarcodeScanner claimedBarcodeScannerInstance1 = null;
        ClaimedBarcodeScanner claimedBarcodeScannerInstance2 = null;


        public Scenario2_MultipleScanners()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Enumerator for current active Instance.
        /// </summary>
        private enum BarcodeScannerInstance
        {
            Instance1,
            Instance2
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ResetTheScenarioState();
            base.OnNavigatedTo(e);
        }

        /// <summary>
        /// Invoked when this page is no longer displayed.
        /// </summary>
        /// <param name="e">Event data that describes how this page was exited.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ResetTheScenarioState();
            base.OnNavigatedFrom(e);
        }

        /// <summary>
        /// This is the click handler for the 'ScenarioStartScanningInstance1' button. It initiates creation of scanner instance 1.
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// 
        private async void ButtonStartScanningInstance1_Click(object sender, RoutedEventArgs e)
        {
            //Get the handle to the default scanner          
            if (await CreateDefaultScannerObjectAsync(BarcodeScannerInstance.Instance1))
            {
                //Claim the scanner
                if (await ClaimBarcodeScannerAsync(BarcodeScannerInstance.Instance1))
                {
                    //add the event handlers
                    claimedBarcodeScannerInstance1.ReleaseDeviceRequested += claimedBarcodeScannerInstance1_ReleaseDeviceRequested;
                    claimedBarcodeScannerInstance1.DataReceived += claimedBarcodeScannerInstance1_DataReceived;
                    claimedBarcodeScannerInstance1.IsDecodeDataEnabled = true;

                    //Enable the Scanner
                    if (await EnableBarcodeScannerAsync(BarcodeScannerInstance.Instance1))
                    {
                        //Set the UI state
                        ResetUI();
                        SetUI(BarcodeScannerInstance.Instance1);
                    }
                }
                else
                {
                    scannerInstance1 = null;
                }
            }
        }

        /// <summary>
        /// This method is called upon when a claim request is made on instance 1. If a retain request was placed on the device it rejects the new claim.
        /// </summary>
        /// <param name="instance"></param>
        /// <returns></returns>
        async void claimedBarcodeScannerInstance1_ReleaseDeviceRequested(object sender, ClaimedBarcodeScanner e)
        {
            await MainPage.Current.Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    //check if the instance wants to retain the device
                    if (Retain1.IsChecked == true)
                    {
                        try
                        {
                            //Retain the device
                            claimedBarcodeScannerInstance1.RetainDevice();
                        }
                        catch (Exception exception)
                        {
                            rootPage.NotifyUser("Retain instance 1 failed: " + exception.Message, NotifyType.ErrorMessage);
                        }
                    }
                    //Release the device
                    else
                    {
                        claimedBarcodeScannerInstance1.Dispose();
                        claimedBarcodeScannerInstance1 = null;
                    }
                }
            );
        }


        /// <summary>
        /// This is the click handler for the 'ScenarioStartScanningInstance2' button. Initiates creation of scanner instance 2
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// 
        private async void ButtonStartScanningInstance2_Click(object sender, RoutedEventArgs e)
        {
            //Get the handle to the default scanner          
            if (await CreateDefaultScannerObjectAsync(BarcodeScannerInstance.Instance2))
            {
                //Claim the scanner
                if (await ClaimBarcodeScannerAsync(BarcodeScannerInstance.Instance2))
                {
                    //set the handlers
                    claimedBarcodeScannerInstance2.ReleaseDeviceRequested += claimedBarcodeScannerInstance2_ReleaseDeviceRequested;
                    claimedBarcodeScannerInstance2.DataReceived += claimedBarcodeScannerInstance2_DataReceived;

                    //enable the scanner to decode the scanned data
                    claimedBarcodeScannerInstance2.IsDecodeDataEnabled = true;

                    //Enable the Scanner
                    if (await EnableBarcodeScannerAsync(BarcodeScannerInstance.Instance2))
                    {
                        //Set the UI state
                        ResetUI();
                        SetUI(BarcodeScannerInstance.Instance2);
                    }
                }
                else
                {
                    scannerInstance2 = null;
                }
            }
        }

        /// <summary>
        /// This method is called upon when a claim request is made on instance 2. If a retain request was placed on the device it rejects the new claim.
        /// </summary>
        /// <param name="instance"></param>
        /// <returns></returns>
        async void claimedBarcodeScannerInstance2_ReleaseDeviceRequested(object sender, ClaimedBarcodeScanner e)
        {
            await MainPage.Current.Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    //check if the instance wants to retain the device
                    if (Retain2.IsChecked == true)
                    {
                        try
                        {
                            //Retain the device
                            claimedBarcodeScannerInstance2.RetainDevice();
                        }
                        catch (Exception exception)
                        {
                            rootPage.NotifyUser("Retain instance 1 failed: " + exception.Message, NotifyType.ErrorMessage);
                        }
                    }
                    //Release the device
                    else
                    {
                        claimedBarcodeScannerInstance2.Dispose();
                        claimedBarcodeScannerInstance2 = null;
                    }

                }
            );
        }

        /// <summary>
        /// This is the click handler for the 'ScenarioEndScanningInstance1' button.  
        /// Initiates the disposal of scanner instance 1.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// 
        private void ButtonEndScanningInstance1_Click(object sender, RoutedEventArgs e)
        {
            //remove the event handlers
            claimedBarcodeScannerInstance1.DataReceived -= claimedBarcodeScannerInstance1_DataReceived;
            claimedBarcodeScannerInstance1.ReleaseDeviceRequested -= claimedBarcodeScannerInstance1_ReleaseDeviceRequested;

            //dispose the instance
            claimedBarcodeScannerInstance1.Dispose();
            claimedBarcodeScannerInstance1 = null;
            scannerInstance1 = null;

            //reset the UI

            ResetUI();

            rootPage.NotifyUser("Click a start scanning button to begin.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// This is the click handler for the 'ScenarioEndScanningInstance2' button.  
        /// Initiates the disposal fo scanner instance 2.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// 
        private void ButtonEndScanningInstance2_Click(object sender, RoutedEventArgs e)
        {
            //remove the event handlers
            claimedBarcodeScannerInstance2.DataReceived -= claimedBarcodeScannerInstance2_DataReceived;
            claimedBarcodeScannerInstance2.ReleaseDeviceRequested -= claimedBarcodeScannerInstance2_ReleaseDeviceRequested;

            //dispose the instance
            claimedBarcodeScannerInstance2.Dispose();
            claimedBarcodeScannerInstance2 = null;
            scannerInstance2 = null;

            //reset the UI
            ResetUI();

            rootPage.NotifyUser("Click a start scanning button to begin.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// This method returns the first available Barcode Scanner. To enumerate and find a particular device use the device enumeration code.
        /// </summary>
        /// <returns>a boolean value based on whether it found a compatible scanner connected</returns>
        private async Task<bool> CreateDefaultScannerObjectAsync(BarcodeScannerInstance instance)
        {
            BarcodeScanner scanner = null;
            scanner = await BarcodeScanner.GetDefaultAsync();

            if (scanner == null)
            {
                rootPage.NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType.ErrorMessage);
                return false;
            }

            switch (instance)
            {
                case BarcodeScannerInstance.Instance1:
                    scannerInstance1 = scanner;
                    break;
                case BarcodeScannerInstance.Instance2:
                    scannerInstance2 = scanner;
                    break;
                default:
                    return false;
            }

            return true;
        }

        /// <summary>
        /// This method claims the connected scanner.
        /// </summary>
        /// <returns>a boolean based on whether it was able to claim the scanner.</returns>
        private async Task<bool> ClaimBarcodeScannerAsync(BarcodeScannerInstance instance)
        {
            bool bClaimAsyncStatus = false;
            //select the instance to claim
            switch (instance)
            {
                case BarcodeScannerInstance.Instance1:

                    claimedBarcodeScannerInstance1 = await scannerInstance1.ClaimScannerAsync();
                    if (claimedBarcodeScannerInstance1 == null)
                        rootPage.NotifyUser("Instance 1 claim barcode scanner failed.", NotifyType.ErrorMessage);
                    else
                        bClaimAsyncStatus = true;
                    break;

                case BarcodeScannerInstance.Instance2:

                    claimedBarcodeScannerInstance2 = await scannerInstance2.ClaimScannerAsync();
                    if (claimedBarcodeScannerInstance2 == null)
                        rootPage.NotifyUser("Instance 2 claim barcode scanner failed.", NotifyType.ErrorMessage);
                    else
                        bClaimAsyncStatus = true;
                    break;

                default:
                    return bClaimAsyncStatus;
            }
            return bClaimAsyncStatus;
        }

        /// <summary>
        /// This method enables the connected scanner.
        /// </summary>
        /// <returns>a boolean based on whether it was able to enable the scanner.</returns>
        private async Task<bool> EnableBarcodeScannerAsync(BarcodeScannerInstance instance)
        {
            switch (instance)
            {
                case BarcodeScannerInstance.Instance1:
                    await claimedBarcodeScannerInstance1.EnableAsync();
                    rootPage.NotifyUser("Instance 1 ready to scan. Device ID: " + claimedBarcodeScannerInstance1.DeviceId, NotifyType.StatusMessage);
                    break;
                case BarcodeScannerInstance.Instance2:
                    await claimedBarcodeScannerInstance2.EnableAsync();
                    rootPage.NotifyUser("Instance 2 ready to scan. Device ID: " + claimedBarcodeScannerInstance2.DeviceId, NotifyType.StatusMessage);
                    break;
                default:
                    return false;
            }
            return true;
        }

        /// <summary>
        /// Reset the Scenario state
        /// </summary>
        private void ResetTheScenarioState()
        {
            if (claimedBarcodeScannerInstance1 != null)
            {
                claimedBarcodeScannerInstance1.Dispose();
                claimedBarcodeScannerInstance1 = null;
            }

            scannerInstance1 = null;

            if (claimedBarcodeScannerInstance2 != null)
            {
                claimedBarcodeScannerInstance2.Dispose();
                claimedBarcodeScannerInstance2 = null;
            }

            scannerInstance2 = null;

            ResetUI();
        }
        
        /// <summary>
        /// Resets the display Elements to original state
        /// </summary>
        private void ResetUI()
        {
            Instance1Border.BorderBrush = new SolidColorBrush(Colors.Gray);
            Instance2Border.BorderBrush = new SolidColorBrush(Colors.Gray);

            ScanDataType1.Text = String.Format("No data");
            ScanData1.Text = String.Format("No data");
            DataLabel1.Text = String.Format("No data");
            ScanDataType2.Text = String.Format("No data");
            ScanData2.Text = String.Format("No data");
            DataLabel2.Text = String.Format("No data");

            ScenarioStartScanningInstance1.IsEnabled = true;
            ScenarioStartScanningInstance2.IsEnabled = true;
            ScenarioEndScanningInstance1.IsEnabled = false;
            ScenarioEndScanningInstance2.IsEnabled = false;
        }

        /// <summary>
        /// Sets the UI elements to a state corresponding to the current active Instance.
        /// </summary>
        /// <param name="instance">Corresponds to the current active instance</param>
        private async void SetUI(BarcodeScannerInstance instance)
        {
            Instance1Border.BorderBrush = new SolidColorBrush(Colors.Gray);
            Instance2Border.BorderBrush = new SolidColorBrush(Colors.Gray);

            switch (instance)
            {

                case BarcodeScannerInstance.Instance1:
                    await rootPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                        () =>
                        {
                            ScenarioStartScanningInstance1.IsEnabled = false;
                            ScenarioStartScanningInstance2.IsEnabled = true;
                            ScenarioEndScanningInstance1.IsEnabled = true;
                            ScenarioEndScanningInstance2.IsEnabled = false;
                            Instance1Border.BorderBrush = new SolidColorBrush(Colors.DarkBlue);
                        }
                    );

                    break;

                case BarcodeScannerInstance.Instance2:
                    await rootPage.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        ScenarioStartScanningInstance1.IsEnabled = true;
                        ScenarioStartScanningInstance2.IsEnabled = false;
                        ScenarioEndScanningInstance1.IsEnabled = false;
                        ScenarioEndScanningInstance2.IsEnabled = true;
                        Instance2Border.BorderBrush = new SolidColorBrush(Colors.DarkBlue);
                    }
                );
                    break;

                default:
                    break;
            }
        }

        string GetDataString(IBuffer data)
        {
            StringBuilder result = new StringBuilder();

            if (data == null)
            {
                result.Append("No data");
            }
            else
            {
                // Just to show that we have the raw data, we'll print the value of the bytes.
                // Arbitrarily limit the number of bytes printed to 20 so the UI isn't overloaded.
                const uint MAX_BYTES_TO_PRINT = 20;
                uint bytesToPrint = Math.Min(data.Length, MAX_BYTES_TO_PRINT);

                DataReader reader = DataReader.FromBuffer(data);
                byte[] dataBytes = new byte[bytesToPrint];
                reader.ReadBytes(dataBytes);

                for (uint byteIndex = 0; byteIndex < bytesToPrint; ++byteIndex)
                {
                    result.AppendFormat("{0:X2} ", dataBytes[byteIndex]);
                }

                if (bytesToPrint < data.Length)
                {
                    result.Append("...");
                }
            }

            return result.ToString();
        }

        string GetDataLabelString(IBuffer data, uint scanDataType)
        {
            string result = null;
            // Only certain data types contain encoded text.
            //   To keep this simple, we'll just decode a few of them.
            if (data == null)
            {
                result = "No data";
            }
            else
            {
                switch (BarcodeSymbologies.GetName(scanDataType))
                {
                    case "Upca":
                    case "UpcaAdd2":
                    case "UpcaAdd5":
                    case "Upce":
                    case "UpceAdd2":
                    case "UpceAdd5":
                    case "Ean8":
                    case "TfStd":
                        // The UPC, EAN8, and 2 of 5 families encode the digits 0..9
                        // which are then sent to the app in a UTF8 string (like "01234")

                        // This is not an exhaustive list of symbologies that can be converted to a string

                        DataReader reader = DataReader.FromBuffer(data);
                        result = reader.ReadString(data.Length);
                        break;
                    default:
                        // Some other symbologies (typically 2-D symbologies) contain binary data that
                        //  should not be converted to text.
                        result = string.Format("Decoded data unavailable. Raw label data: {0}", GetDataString(data));
                        break;
                }
            }

            return result;
        }

        /// <summary>
        /// This is an event handler for the claimed scanner Instance 1 when it scans and recieves data
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void claimedBarcodeScannerInstance1_DataReceived(ClaimedBarcodeScanner sender, BarcodeScannerDataReceivedEventArgs args)
        {
            // Grab the data from the IBuffers
            string scanData = String.Empty;
            string scanDataLabel = String.Empty;

            if (args.Report.ScanData != null)
            {
                scanData = GetDataString(args.Report.ScanData);
            }

            if (args.Report.ScanDataLabel != null)
            {
                scanDataLabel = GetDataLabelString(args.Report.ScanDataLabel, args.Report.ScanDataType);
            }

            await MainPage.Current.Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    ScanDataType1.Text = String.Format("{0}", BarcodeSymbologies.GetName(args.Report.ScanDataType));

                    // DataLabel
                    DataLabel1.Text = String.Format("{0}", scanDataLabel);

                    // Data
                    ScanData1.Text = String.Format("{0}", scanData);

                    rootPage.NotifyUser("Instance 1 received data from the barcode scanner.", NotifyType.StatusMessage);
                }
            );
        }

        /// <summary>
        /// This is an event handler for the claimed scanner Instance 2 when it scans and recieves data
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void claimedBarcodeScannerInstance2_DataReceived(ClaimedBarcodeScanner sender, BarcodeScannerDataReceivedEventArgs args)
        {
            // Grab the data from the IBuffers
            string scanData = String.Empty;
            string scanDataLabel = String.Empty;

            if (args.Report.ScanData != null)
            {
                scanData = GetDataString(args.Report.ScanData);
            }

            if (args.Report.ScanDataLabel != null)
            {
                scanDataLabel = GetDataLabelString(args.Report.ScanDataLabel, args.Report.ScanDataType);
            }

            await MainPage.Current.Dispatcher.RunAsync(
                Windows.UI.Core.CoreDispatcherPriority.Normal,
                () =>
                {
                    // Symbology
                    ScanDataType2.Text = String.Format("{0}", BarcodeSymbologies.GetName(args.Report.ScanDataType));

                    // DataLabel
                    DataLabel2.Text = String.Format("{0}", scanDataLabel);

                    // Data
                    ScanData2.Text = String.Format("{0}", scanData);

                    rootPage.NotifyUser("Instance 2 received data from the barcode scanner.", NotifyType.StatusMessage);
                }
            );
        }
    }

}
