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
#include "Scenario3_ActiveSymbologies.xaml.h"

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace SDKTemplate;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_ActiveSymbologies::Scenario3_ActiveSymbologies() : rootPage(MainPage::Current)
{
    InitializeComponent();

    listOfSymbologies = ref new Vector<SymbologyListEntry^>();
    SymbologyListSource->Source = listOfSymbologies;
}

// <summary>
/// Setup the barcode scanner to be ready to receive the data events from the scan.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_ActiveSymbologies::ScenarioStartScanButton_Click(Object^ sender, RoutedEventArgs^ e)
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

                // claim the scanner for exclusive use.
                return create_task(scanner->ClaimScannerAsync());
            }).then([this](ClaimedBarcodeScanner^ _claimedScanner)
            {
                claimedScanner = _claimedScanner;
                if (claimedScanner)
                {
                    // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can 
                    // claim ownership of the barcode scanner.
                    releaseDeviceRequestedToken = claimedScanner->ReleaseDeviceRequested += ref new EventHandler<ClaimedBarcodeScanner^>(this, &Scenario3_ActiveSymbologies::OnReleaseDeviceRequested);

                    // after successfully claiming, attach the datareceived event handler.
                    dataReceivedToken = claimedScanner->DataReceived += ref new TypedEventHandler<ClaimedBarcodeScanner^, BarcodeScannerDataReceivedEventArgs^>(this, &Scenario3_ActiveSymbologies::OnDataReceived);

                    // Ask for the raw data from the barcode scanner to be decoded and
                    // send the ScanDataLabel and ScanDataType in the DataReceived event
                    claimedScanner->IsDecodeDataEnabled = true;

                    // Enable the scanner so it raises DataReceived events.
                    // Do this after adding the DataReceived event handler.
                    create_task(claimedScanner->EnableAsync()).then([this]()
                    {
                        rootPage->NotifyUser("Ready to scan. Device ID: " + claimedScanner->DeviceId, NotifyType::StatusMessage);

                        // reset the button state
                        ScenarioEndScanButton->IsEnabled = true;
                        SetActiveSymbologiesButton->IsEnabled = true;
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
void Scenario3_ActiveSymbologies::OnReleaseDeviceRequested(Object^ sender, ClaimedBarcodeScanner^ args)
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
void Scenario3_ActiveSymbologies::OnDataReceived(ClaimedBarcodeScanner^ sender, BarcodeScannerDataReceivedEventArgs^ args)
{
    // read the data from the buffer and convert to string.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, args]()
    {
        ScenarioOutputScanDataLabel->Text = GetDataLabelString(args->Report->ScanDataLabel, args->Report->ScanDataType);
        ScenarioOutputScanData->Text = GetDataString(args->Report->ScanData);
        ScenarioOutputScanDataType->Text = BarcodeSymbologies::GetName(args->Report->ScanDataType);
    }));
}

void Scenario3_ActiveSymbologies::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

void Scenario3_ActiveSymbologies::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Event handler for End Scan Button Click. 
/// Releases the Barcode Scanner and resets the text in the UI
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario3_ActiveSymbologies::ScenarioEndScanButton_Click(Object^ sender, RoutedEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Reset the Scenario state
/// </summary>
void Scenario3_ActiveSymbologies::ResetTheScenarioState()
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

    // Reset the strings in the UI
    rootPage->NotifyUser("Click the start scanning button to begin.", NotifyType::StatusMessage);

    ScenarioOutputScanData->Text = "No data";
    ScenarioOutputScanDataLabel->Text = "No data";
    ScenarioOutputScanDataType->Text = "No data";

    // reset the button state
    SetActiveSymbologiesButton->IsEnabled = false;
    ScenarioEndScanButton->IsEnabled = false;
    ScenarioStartScanButton->IsEnabled = true;

    // reset symbology list
    listOfSymbologies->Clear();
}

void Scenario3_ActiveSymbologies::SetActiveSymbologies_Click(Object^ sender, RoutedEventArgs^ e)
{
    if (claimedScanner != nullptr)
    {
        auto symbologyList = ref new Vector<uint32_t>();

        for (SymbologyListEntry^ symbologyListEntry : listOfSymbologies)
        {
            if (symbologyListEntry->IsEnabled)
            {
                symbologyList->Append(symbologyListEntry->Id);
            }
        }

        claimedScanner->SetActiveSymbologiesAsync(symbologyList);
    }
}
