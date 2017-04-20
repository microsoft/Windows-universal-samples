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
#include "pch.h"
#include "Scenario4_SymbologyAttributes.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario4_SymbologyAttributes::Scenario4_SymbologyAttributes() : rootPage(MainPage::Current)
{
    InitializeComponent();

    listOfSymbologies = ref new Vector<SymbologyListEntry^>();
    SymbologyListSource->Source = listOfSymbologies;
}

void Scenario4_SymbologyAttributes::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

void Scenario4_SymbologyAttributes::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

// <summary>
/// Setup the barcode scanner to be ready to receive the data events from the scan.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_SymbologyAttributes::ScenarioStartScanButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    ScenarioStartScanButton->IsEnabled = false;

    rootPage->NotifyUser("Acquiring barcode scanner object...", NotifyType::StatusMessage);

    // create the barcode scanner. 
    create_task(DeviceHelpers::GetFirstBarcodeScannerAsync()).then([this](BarcodeScanner^ newScanner)
    {
        scanner = newScanner;

        if (scanner != nullptr)
        {
            // after successful creation, list supported symbologies
            create_task(scanner->GetSupportedSymbologiesAsync()).then([this](IVectorView<uint32_t>^ supportedSymbologies)
            {
                for (uint32_t symbology : supportedSymbologies)
                {
                    this->listOfSymbologies->Append(ref new SymbologyListEntry(symbology));
                }

                // Claim the scanner for exclusive use.
                return create_task(scanner->ClaimScannerAsync());
            }).then([this](ClaimedBarcodeScanner^ _claimedScanner)
            {
                claimedScanner = _claimedScanner;
                if (claimedScanner)
                {
                    // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can 
                    // claim ownership of the barcode scanner.
                    releaseDeviceRequestedToken = claimedScanner->ReleaseDeviceRequested += ref new EventHandler<ClaimedBarcodeScanner^>(this, &Scenario4_SymbologyAttributes::OnReleaseDeviceRequested);

                    // after successfully claiming, attach the datareceived event handler.
                    dataReceivedToken = claimedScanner->DataReceived += ref new TypedEventHandler<ClaimedBarcodeScanner^, BarcodeScannerDataReceivedEventArgs^>(this, &Scenario4_SymbologyAttributes::OnDataReceived);

                    // Ask for the raw data from the barcode scanner to be decoded and
                    // send the ScanDataLabel and ScanDataType in the DataReceived event
                    claimedScanner->IsDecodeDataEnabled = true;

                    // enable the scanner.
                    // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event 
                    // if the claimedScanner has not been Enabled
                    create_task(claimedScanner->EnableAsync()).then([this](void)
                    {
                        rootPage->NotifyUser("Ready to scan. Device ID: " + claimedScanner->DeviceId, NotifyType::StatusMessage);

                        // reset the button state
                        ScenarioEndScanButton->IsEnabled = true;
                        SetSymbologyAttributesButton->IsEnabled = true;
                    });
                }
                else
                {
                    delete scanner;
                    scanner = nullptr;
                    ScenarioStartScanButton->IsEnabled = true;
                    rootPage->NotifyUser("Claim barcode scanner failed.", NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType::ErrorMessage);
            ScenarioStartScanButton->IsEnabled = true;
        }
    }, concurrency::task_continuation_context::use_current());

}

/// <summary>
/// Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
void Scenario4_SymbologyAttributes::OnReleaseDeviceRequested(Object^ sender, ClaimedBarcodeScanner^ args)
{
    // let us retain the device always. If it is not retained, this exclusive claim will be lost.
    args->RetainDevice();

    rootPage->NotifyUser("Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType::StatusMessage);
}

/// <summary>
/// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner 
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
void Scenario4_SymbologyAttributes::OnDataReceived(ClaimedBarcodeScanner^ sender, BarcodeScannerDataReceivedEventArgs^ args)
{
    // read the data from the buffer and convert to string.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        ScenarioOutputScanDataLabel->Text = GetDataLabelString(args->Report->ScanDataLabel, args->Report->ScanDataType);
        ScenarioOutputScanData->Text = GetDataString(args->Report->ScanData);
        ScenarioOutputScanDataType->Text = BarcodeSymbologies::GetName(args->Report->ScanDataType);
    }));
}

/// <summary>
/// Event handler for End Scan Button Click. 
/// Releases the Barcode Scanner and resets the text in the UI
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario4_SymbologyAttributes::ScenarioEndScanButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Reset the Scenario state
/// </summary>
void Scenario4_SymbologyAttributes::ResetTheScenarioState()
{
    if (claimedScanner != nullptr)
    {
        // Detach the event handlers
        claimedScanner->DataReceived -= dataReceivedToken;
        claimedScanner->ReleaseDeviceRequested -= releaseDeviceRequestedToken;

        // release the Barcode Scanner and set to null
        delete claimedScanner;
        claimedScanner = nullptr;
    }

    if (scanner != nullptr)
    {
        delete scanner;
        scanner = nullptr;
    }

    symbologyAttributes = nullptr;

    // Reset the strings in the UI
    rootPage->NotifyUser("Click the start scanning button to begin.", NotifyType::StatusMessage);

    ScenarioOutputScanData->Text = "No data";
    ScenarioOutputScanDataLabel->Text = "No data";
    ScenarioOutputScanDataType->Text = "No data";

    // reset the button state
    ScenarioEndScanButton->IsEnabled = false;
    ScenarioStartScanButton->IsEnabled = true;
    SetSymbologyAttributesButton->IsEnabled = false;
    EnableCheckDigit->IsEnabled = false;
    TransmitCheckDigit->IsEnabled = false;
    SetDecodeRangeLimits->IsEnabled = false;

    // reset symbology list
    listOfSymbologies->Clear();
}

/// <summary>
/// Event handler for Symbology listbox selection changed. 
/// Get symbology attributes and populate attribute UI components
/// </summary>
/// <param name="sender"></param>
/// <param name="args"></param>
void Scenario4_SymbologyAttributes::SymbologySelection_Changed(Object^ sender, SelectionChangedEventArgs^ e)
{
    if (claimedScanner != nullptr)
    {
        SymbologyListEntry^ symbologyListEntry = safe_cast<SymbologyListEntry^>(SymbologyListBox->SelectedItem);
        if (symbologyListEntry != nullptr)
        {
            SetSymbologyAttributesButton->IsEnabled = false;
            create_task(claimedScanner->GetSymbologyAttributesAsync(symbologyListEntry->Id)).then([this](task<BarcodeSymbologyAttributes^> previousTask)
            {
                try
                {
                    symbologyAttributes = previousTask.get();
                }
                catch (Exception^)
                {
                    symbologyAttributes = nullptr;
                }

                if (symbologyAttributes != nullptr)
                {
                    SetSymbologyAttributesButton->IsEnabled = true;

                    // initialize attributes UIs
                    EnableCheckDigit->IsEnabled = symbologyAttributes->IsCheckDigitValidationSupported;
                    EnableCheckDigit->IsChecked = symbologyAttributes->IsCheckDigitValidationEnabled;
                    TransmitCheckDigit->IsEnabled = symbologyAttributes->IsCheckDigitTransmissionSupported;
                    TransmitCheckDigit->IsChecked = symbologyAttributes->IsCheckDigitTransmissionEnabled;
                    SetDecodeRangeLimits->IsEnabled = symbologyAttributes->IsDecodeLengthSupported;
                    bool decodeLengthEnabled = (symbologyAttributes->DecodeLengthKind == BarcodeSymbologyDecodeLengthKind::Range);
                    SetDecodeRangeLimits->IsChecked = decodeLengthEnabled;
                    if (decodeLengthEnabled)
                    {
                        MinimumDecodeLength->Value = min(symbologyAttributes->DecodeLength1, symbologyAttributes->DecodeLength2);
                        MaximumDecodeLength->Value = max(symbologyAttributes->DecodeLength1, symbologyAttributes->DecodeLength2);
                    }
                }
                else
                {
                    rootPage->NotifyUser("Symbology attributes are not available.", NotifyType::ErrorMessage);
                    EnableCheckDigit->IsEnabled = false;
                    TransmitCheckDigit->IsEnabled = false;
                    SetDecodeRangeLimits->IsEnabled = false;
                }
            });
        }
    }
}

void Scenario4_SymbologyAttributes::SetSymbologyAttributes_Click(Object^ sender, RoutedEventArgs^ e)
{
    if ((claimedScanner != nullptr) && (symbologyAttributes != nullptr))
    {
        SetSymbologyAttributesButton->IsEnabled = false;

        // populate attributes
        if (symbologyAttributes->IsCheckDigitValidationSupported)
        {
            symbologyAttributes->IsCheckDigitValidationEnabled = EnableCheckDigit->IsChecked->Value;
        }
        if (symbologyAttributes->IsCheckDigitTransmissionSupported)
        {
            symbologyAttributes->IsCheckDigitTransmissionEnabled = TransmitCheckDigit->IsChecked->Value;
        }
        if (symbologyAttributes->IsDecodeLengthSupported)
        {
            if (SetDecodeRangeLimits->IsChecked->Value)
            {
                symbologyAttributes->DecodeLengthKind = BarcodeSymbologyDecodeLengthKind::Range;
                symbologyAttributes->DecodeLength1 = (unsigned int)MinimumDecodeLength->Value;
                symbologyAttributes->DecodeLength2 = (unsigned int)MaximumDecodeLength->Value;
            }
            else
            {
                symbologyAttributes->DecodeLengthKind = BarcodeSymbologyDecodeLengthKind::AnyLength;
            }
        }

        SymbologyListEntry^ symbologyListEntry = safe_cast<SymbologyListEntry^>(SymbologyListBox->SelectedItem);
        if (symbologyListEntry != nullptr)
        {
            create_task(claimedScanner->SetSymbologyAttributesAsync(symbologyListEntry->Id, symbologyAttributes)).then([this, symbologyListEntry](task<bool> previousTask)
            {
                bool attributesSet = false;
                try
                {
                    attributesSet = previousTask.get();
                }
                catch (Exception^)
                {
                    // Scanner could not set the attributes.
                }

                if (attributesSet)
                {
                    rootPage->NotifyUser("Attributes set for symbology '" + symbologyListEntry->Name + "'", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("Attributes could not be set for symbology '" + symbologyListEntry->Name + "'.", NotifyType::ErrorMessage);
                }
            });
        }
        else
        {
            rootPage->NotifyUser("Select a symbology from the list.", NotifyType::ErrorMessage);
        }
        SetSymbologyAttributesButton->IsEnabled = true;
    }
}
