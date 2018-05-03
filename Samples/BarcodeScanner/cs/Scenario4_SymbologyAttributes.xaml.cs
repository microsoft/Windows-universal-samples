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
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using Windows.Devices.PointOfService;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario4_SymbologyAttributes : Page
    {
        MainPage rootPage = MainPage.Current;
        BarcodeScanner scanner = null;
        ClaimedBarcodeScanner claimedScanner = null;

        ObservableCollection<SymbologyListEntry> listOfSymbologies = null;
        BarcodeSymbologyAttributes symbologyAttributes = null;

        public Scenario4_SymbologyAttributes()
        {
            this.InitializeComponent();

            listOfSymbologies = new ObservableCollection<SymbologyListEntry>();
            SymbologyListSource.Source = listOfSymbologies;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ResetTheScenarioState();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            ResetTheScenarioState();
        }

        /// <summary>
        /// Event Handler for Start Scan Button Click.
        /// Sets up the barcode scanner to be ready to receive the data events from the scan.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ScenarioStartScanButton_Click(object sender, RoutedEventArgs e)
        {
            ScenarioStartScanButton.IsEnabled = false;

            rootPage.NotifyUser("Acquiring barcode scanner object.", NotifyType.StatusMessage);

            // create the barcode scanner.
            scanner = await DeviceHelpers.GetFirstBarcodeScannerAsync();

            if (scanner != null)
            {
                // claim the scanner for exclusive use and enable it so that data received events are received.
                claimedScanner = await scanner.ClaimScannerAsync();
                if (claimedScanner != null)
                {
                    // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can
                    // claim ownership of the barcode scanner.
                    claimedScanner.ReleaseDeviceRequested += claimedScanner_ReleaseDeviceRequested;

                    // after successfully claiming, attach the datareceived event handler.
                    claimedScanner.DataReceived += claimedScanner_DataReceived;

                    // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and
                    // send the ScanDataLabel and ScanDataType in the DataReceived event
                    claimedScanner.IsDecodeDataEnabled = true;

                    // enable the scanner.
                    // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event
                    // if the claimedScanner has not been Enabled
                    await claimedScanner.EnableAsync();

                    // after successful claim, list supported symbologies
                    IReadOnlyList<uint> supportedSymbologies = await scanner.GetSupportedSymbologiesAsync();
                    foreach (uint symbology in supportedSymbologies)
                    {
                        listOfSymbologies.Add(new SymbologyListEntry(symbology));
                    }

                    // reset the button state
                    ScenarioEndScanButton.IsEnabled = true;

                    rootPage.NotifyUser("Ready to scan. Device ID: " + claimedScanner.DeviceId, NotifyType.StatusMessage);
                }
                else
                {
                    scanner.Dispose();
                    scanner = null;
                    ScenarioStartScanButton.IsEnabled = true;
                    rootPage.NotifyUser("Claim barcode scanner failed.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                ScenarioStartScanButton.IsEnabled = true;
                rootPage.NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
        void claimedScanner_ReleaseDeviceRequested(object sender, ClaimedBarcodeScanner e)
        {
            // always retain the device
            e.RetainDevice();
            rootPage.NotifyUser("Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType.StatusMessage);
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
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // read the data from the buffer and convert to string.
                ScenarioOutputScanDataLabel.Text = DataHelpers.GetDataLabelString(args.Report.ScanDataLabel, args.Report.ScanDataType);
                ScenarioOutputScanData.Text = DataHelpers.GetDataString(args.Report.ScanData);
                ScenarioOutputScanDataType.Text = BarcodeSymbologies.GetName(args.Report.ScanDataType);
            });
        }

        /// <summary>
        /// Reset the Scenario state
        /// </summary>
        private void ResetTheScenarioState()
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

            if (scanner != null)
            {
                scanner.Dispose();
                scanner = null;
            }

            symbologyAttributes = null;

            // Reset the UI if we are still the current page.
            if (Frame.Content == this)
            {
                rootPage.NotifyUser("Click the start scanning button to begin.", NotifyType.StatusMessage);
                this.ScenarioOutputScanData.Text = "No data";
                this.ScenarioOutputScanDataLabel.Text = "No data";
                this.ScenarioOutputScanDataType.Text = "No data";

                // reset the button state
                ScenarioEndScanButton.IsEnabled = false;
                ScenarioStartScanButton.IsEnabled = true;
                SetSymbologyAttributesButton.IsEnabled = false;
                EnableCheckDigit.IsEnabled = false;
                TransmitCheckDigit.IsEnabled = false;
                SetDecodeRangeLimits.IsEnabled = false;

                // reset symbology list
                listOfSymbologies.Clear();
            }
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
        }

        /// <summary>
        /// Event handler for Symbology listbox selection changed.
        /// Get symbology attributes and populate attribute UI components
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private async void SymbologySelection_Changed(object sender, SelectionChangedEventArgs args)
        {
            if (claimedScanner != null)
            {
                SymbologyListEntry symbologyListEntry = (SymbologyListEntry)SymbologyListBox.SelectedItem;
                if (symbologyListEntry != null)
                {
                    SetSymbologyAttributesButton.IsEnabled = false;
                    try
                    {
                        symbologyAttributes = await claimedScanner.GetSymbologyAttributesAsync(symbologyListEntry.Id);
                    }
                    catch (Exception)
                    {
                        symbologyAttributes = null;
                    }

                    if (symbologyAttributes != null)
                    {
                        SetSymbologyAttributesButton.IsEnabled = true;

                        // initialize attributes UIs
                        EnableCheckDigit.IsEnabled = symbologyAttributes.IsCheckDigitValidationSupported;
                        EnableCheckDigit.IsChecked = symbologyAttributes.IsCheckDigitValidationEnabled;
                        TransmitCheckDigit.IsEnabled = symbologyAttributes.IsCheckDigitTransmissionSupported;
                        TransmitCheckDigit.IsChecked = symbologyAttributes.IsCheckDigitTransmissionEnabled;
                        SetDecodeRangeLimits.IsEnabled = symbologyAttributes.IsDecodeLengthSupported;
                        bool decodeLengthEnabled = (symbologyAttributes.DecodeLengthKind == BarcodeSymbologyDecodeLengthKind.Range);
                        SetDecodeRangeLimits.IsChecked = decodeLengthEnabled;
                        if (decodeLengthEnabled)
                        {
                            MinimumDecodeLength.Value = Math.Min(symbologyAttributes.DecodeLength1, symbologyAttributes.DecodeLength2);
                            MaximumDecodeLength.Value = Math.Max(symbologyAttributes.DecodeLength1, symbologyAttributes.DecodeLength2);
                        }
                    }
                    else
                    {
                        rootPage.NotifyUser("Symbology attributes are not available.", NotifyType.ErrorMessage);
                        EnableCheckDigit.IsEnabled = false;
                        TransmitCheckDigit.IsEnabled = false;
                        SetDecodeRangeLimits.IsEnabled = false;
                    }
                }
            }
        }

        private async void SetSymbologyAttributes_Click(object sender, RoutedEventArgs e)
        {
            if ((claimedScanner != null) && (symbologyAttributes != null))
            {
                // populate attributes
                if (symbologyAttributes.IsCheckDigitValidationSupported)
                {
                    symbologyAttributes.IsCheckDigitValidationEnabled = EnableCheckDigit.IsChecked.Value;
                }
                if (symbologyAttributes.IsCheckDigitTransmissionSupported)
                {
                    symbologyAttributes.IsCheckDigitTransmissionEnabled = TransmitCheckDigit.IsChecked.Value;
                }
                if (symbologyAttributes.IsDecodeLengthSupported)
                {
                    if (SetDecodeRangeLimits.IsChecked.Value)
                    {
                        symbologyAttributes.DecodeLengthKind = BarcodeSymbologyDecodeLengthKind.Range;
                        symbologyAttributes.DecodeLength1 = (uint)MinimumDecodeLength.Value;
                        symbologyAttributes.DecodeLength2 = (uint)MaximumDecodeLength.Value;
                    }
                    else
                    {
                        symbologyAttributes.DecodeLengthKind = BarcodeSymbologyDecodeLengthKind.AnyLength;
                    }
                }

                SymbologyListEntry symbologyListEntry = (SymbologyListEntry)SymbologyListBox.SelectedItem;
                if (symbologyListEntry != null)
                {
                    bool attributesSet = false;

                    try
                    {
                        attributesSet = await claimedScanner.SetSymbologyAttributesAsync(symbologyListEntry.Id, symbologyAttributes);
                    }
                    catch (Exception)
                    {
                        // Scanner could not set the attributes.
                    }

                    if (attributesSet)
                    {
                        rootPage.NotifyUser("Attributes set for symbology '" + symbologyListEntry.Name + "'", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("Attributes could not be set for symbology '" + symbologyListEntry.Name + "'.", NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    rootPage.NotifyUser("Select a symbology from the list.", NotifyType.ErrorMessage);
                }
            }
        }
    }
}
