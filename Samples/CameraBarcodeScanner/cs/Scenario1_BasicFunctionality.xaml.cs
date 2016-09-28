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
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.Enumeration;
using Windows.Devices.PointOfService;
using Windows.Storage.Streams;
using Windows.Media.Capture;
using Windows.UI.Core;
using Windows.System.Display;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_BasicFunctionality : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        BarcodeScanner scanner = null;
        ClaimedBarcodeScanner claimedScanner = null;

        public Scenario1_BasicFunctionality()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Start device watcher.
        /// </summary>
        private void StartWatcher()
        {
            if (posDeviceWatcher == null)
            {
                posDeviceWatcher = new PosDeviceWatcher(BarcodeScanner.GetDeviceSelector(), Dispatcher);
                posDeviceWatcher.Start();
            }
        }

        /// <summary>
        /// Stop device watcher.
        /// </summary>
        private void StopWatcher()
        {
            if (posDeviceWatcher != null)
            {
                posDeviceWatcher.Stop();
                posDeviceWatcher = null;
            }
        }

        /// <summary>
        /// Creates the default barcode scanner.
        /// </summary>
        /// <returns>true if barcode scanner is created. Otherwise returns false</returns>
        private async Task<bool> CreateDefaultScannerObject()
        {
            if (scanner == null)
            {
                rootPage.NotifyUser("Creating barcode scanner object.", NotifyType.StatusMessage);

                if (posDeviceWatcher.FoundDeviceList != null && posDeviceWatcher.FoundDeviceList.Count > 0)
                {
                    scanner = await BarcodeScanner.FromIdAsync(posDeviceWatcher.FoundDeviceList[0].Id);

                    if (scanner == null)
                    {
                        rootPage.NotifyUser("Failed to create barcode scanner object.", NotifyType.ErrorMessage);
                        return false;
                    }
                }
                else
                {
                    rootPage.NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType.ErrorMessage);
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        /// This method claims the barcode scanner 
        /// </summary>
        /// <returns>true if claim is successful. Otherwise returns false</returns>
        private async Task<bool> ClaimScanner()
        {
            if (claimedScanner == null)
            {
                // claim the barcode scanner
                claimedScanner = await scanner.ClaimScannerAsync();

                // enable the claimed barcode scanner
                if (claimedScanner == null)
                {
                    rootPage.NotifyUser("Claim barcode scanner failed.", NotifyType.ErrorMessage);
                    return false;
                }
            }
            return true;
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
            StartWatcher();
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
            StopWatcher();
        }

        /// <summary>
        /// Event Handler for Start Scan Button Click.
        /// Sets up the barcode scanner to be ready to receive the data events from the scan.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ScenarioStartScanButton_Click(object sender, RoutedEventArgs e)
        {
            rootPage.NotifyUser("Creating barcode scanner object.", NotifyType.StatusMessage);

            // create the barcode scanner. 
            if (await CreateDefaultScannerObject())
            {
                // after successful creation, claim the scanner for exclusive use and enable it so that data reveived events are received.
                if (await ClaimScanner())
                {

                    // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can 
                    // claim ownsership of the barcode scanner.
                    claimedScanner.ReleaseDeviceRequested += claimedScanner_ReleaseDeviceRequested;

                    // after successfully claiming, attach the datareceived event handler.
                    claimedScanner.DataReceived += claimedScanner_DataReceived;

                    // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and 
                    // send the ScanDataLabel and ScanDataType in the DataReceived event
                    claimedScanner.IsDecodeDataEnabled = true;

                    // enable the scanner.
                    // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event 
                    // if the claimedScanner has not beed Enabled
                    await claimedScanner.EnableAsync();

                    // reset the button state
                    ScenarioEndScanButton.IsEnabled = true;
                    ScenarioStartSWTriggerButton.IsEnabled = true;
                    ScenarioStopSWTriggerButton.IsEnabled = true;
                    ScenarioStartScanButton.IsEnabled = false;

                    try
                    {
                        // Use the VideoDeviceId from scanner to open the capture stream
                        await InitializeCameraAsync();
                    }
                    catch (Exception)
                    {
                        Debug.WriteLine("Could not initialize Camera Async");
                    }
                    rootPage.NotifyUser("Ready to scan. Device ID: " + claimedScanner.DeviceId, NotifyType.StatusMessage);
                    StopWatcher();
                }
            }
        }

        /// <summary>
        /// Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
        async void claimedScanner_ReleaseDeviceRequested(object sender, ClaimedBarcodeScanner e)
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                // always retain the device
                e.RetainDevice();

                rootPage.NotifyUser("Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType.StatusMessage);
            });
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
                    case "Ean13":
                        // The UPC, EAN8, EAN13, and 2 of 5 families encode the digits 0..9
                        // which are then sent to the app in a UTF8 string (like "01234")
                        // This is not an exhaustive list of symbologies that can be converted to a string
                    case "Qr":
                        // Need to distinguish the scan data among Numeric, Alphanumeric, Binary/Byte, and Kanji/Kana.
                        // But as current barcode scanner could not distinguish the data is in Binary/Byte format or in rest 3 formats.
                        // So to read the decoded data as string.
                        result = string.Format("{0}", System.Text.Encoding.UTF8.GetString(data.ToArray()));
                        break;
                    default:
                        // Some other unsupport symbologies and just leave it as binary data
                        result = string.Format("Decoded data unavailable. Raw label data: {0}", GetDataString(data));
                        break;
                }
            }

            return result;
        }

        /// <summary>
        /// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
        async void claimedScanner_DataReceived(ClaimedBarcodeScanner sender, BarcodeScannerDataReceivedEventArgs args)
        {
            // need to update the UI data on the dispatcher thread.
            // update the UI with the data received from the scan.
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                // read the scan decoded data from the buffer and convert to string.
                ScenarioOutputScanDataLabel.Text = GetDataLabelString(args.Report.ScanDataLabel, args.Report.ScanDataType);

                // read the raw data from the buffer
                ScenarioOutputScanData.Text = GetDataString(args.Report.ScanData);

                ScenarioOutputScanDataType.Text = BarcodeSymbologies.GetName(args.Report.ScanDataType);
            });
        }

        /// <summary>
        /// Reset the Scenario state
        /// </summary>
        private async void ResetTheScenarioState()
        {
            if (claimedScanner != null)
            {
                // Detach the event handlers
                claimedScanner.DataReceived -= claimedScanner_DataReceived;
                claimedScanner.ReleaseDeviceRequested -= claimedScanner_ReleaseDeviceRequested;
                // Release the Barcode Scanner and set to null
                claimedScanner.Dispose();
                claimedScanner = null;
            }

            scanner = null;

            await CleanupCameraAsync();

            // Reset the strings in the UI
            rootPage.NotifyUser("Click the start scanning button to begin.", NotifyType.StatusMessage);
            this.ScenarioOutputScanData.Text = "No data";
            this.ScenarioOutputScanDataLabel.Text = "No data";
            this.ScenarioOutputScanDataType.Text = "No data";

            // reset the button state
            ScenarioEndScanButton.IsEnabled = false;
            ScenarioStartSWTriggerButton.IsEnabled = false;
            ScenarioStopSWTriggerButton.IsEnabled = false;
            ScenarioStartScanButton.IsEnabled = true;
        }

        /// <summary>
        /// Event handler for End Scan Button Click. 
        /// Releases the Barcode Scanner and resets the text in the UI
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioEndScanButton_Click(object sender, RoutedEventArgs e)
        {
            // reset the scenario state
            this.ResetTheScenarioState();
            StartWatcher();
        }

        /// <summary>
        /// Attempts to find the first available camera capture device
        /// </summary>
         /// <returns></returns>
        private static async Task<DeviceInformation> FindCameraDeviceAsync()
        {
            // Get available devices for capturing pictures
            var allVideoDevices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);

            // Return the first device found
            return (allVideoDevices.Count > 0)? allVideoDevices[0] : null;
        }

        /// <summary>
        /// Initialize camera capture device
        private async Task InitializeCameraAsync()
        {
            if (_mediaCapture == null)
            {
                // Attempt to get the first available camera 
                var cameraDevice = await FindCameraDeviceAsync();

                if (cameraDevice == null)
                {
                    Debug.WriteLine("No camera device found!");
                    return;
                }

                // Create MediaCapture and its settings
                _mediaCapture = new MediaCapture();
                var settings = new MediaCaptureInitializationSettings { VideoDeviceId = scanner.VideoDeviceId };

                // Initialize MediaCapture
                try
                {
                    await _mediaCapture.InitializeAsync(settings);
                    _isInitialized = true;
                }
                catch (UnauthorizedAccessException)
                {
                    Debug.WriteLine("The app was denied access to the camera");
                }

                // If initialization succeeded, start the preview
                if (_isInitialized)
                {
                    await StartPreviewAsync();
                }
            }
        }

        private async void ScenarioStartSWTriggerButton_Click(object sender, RoutedEventArgs e)
        {
            if (scanner.Capabilities.IsSoftwareTriggerSupported)
            {
                await claimedScanner.StartSoftwareTriggerAsync();
            }
            // reset the trigger buttons' state
            ScenarioStartSWTriggerButton.IsEnabled = false;
            ScenarioStopSWTriggerButton.IsEnabled = true;
            rootPage.NotifyUser("Start Software Trigger", NotifyType.StatusMessage);
        }

        private async void ScenarioStopSWTriggerButton_Click(object sender, RoutedEventArgs e)
        {
            if (scanner.Capabilities.IsSoftwareTriggerSupported)
            {
                await claimedScanner.StopSoftwareTriggerAsync();
            }
            // reset the trigger buttons' state
            ScenarioStartSWTriggerButton.IsEnabled = true;
            ScenarioStopSWTriggerButton.IsEnabled = false;
            rootPage.NotifyUser("Stop Software Trigger", NotifyType.StatusMessage);
        }

        private async Task StartPreviewAsync()
        {
            // Prevent the device from sleeping while the preview is running
            _displayRequest.RequestActive();

            // Set the preview source in the UI and mirror it if necessary
            PreviewControl.Source = _mediaCapture;

            // Start the preview
            await _mediaCapture.StartPreviewAsync();
            _isPreviewing = true;
        }

        /// <summary>
        /// Stops the preview and deactivates a display request, to allow the screen to go into power saving modes
        /// </summary>
        /// <returns></returns>
        private async Task StopPreviewAsync()
        {
            // Stop the preview
            _isPreviewing = false;
            await _mediaCapture.StopPreviewAsync();

            // Use the dispatcher because this method is sometimes called from non-UI threads
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Cleanup the UI
                PreviewControl.Source = null;

                // Allow the device screen to sleep now that the preview is stopped
                _displayRequest.RequestRelease();
            });
        }


        private async Task CleanupCameraAsync()
        {
            Debug.WriteLine("CleanupCameraAsync");

            if (_isInitialized)
            {
                if (_isPreviewing)
                {
                    // The call to stop the preview is included here for completeness, but can be
                    // safely removed if a call to MediaCapture.Dispose() is being made later,
                    // as the preview will be automatically stopped at that point
                    await StopPreviewAsync();
                }

                _isInitialized = false;
            }

            if (_mediaCapture != null)
            {
                _mediaCapture.Dispose();
                _mediaCapture = null;
            }
        }

        private readonly DisplayRequest _displayRequest = new DisplayRequest();
        private MediaCapture _mediaCapture;
        private bool _isInitialized;
        private bool _isPreviewing;
        private PosDeviceWatcher posDeviceWatcher = null;
    }
}
