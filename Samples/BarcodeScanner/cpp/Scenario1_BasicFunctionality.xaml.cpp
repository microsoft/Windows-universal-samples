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
#include "Scenario1_BasicFunctionality.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_BasicFunctionality::Scenario1_BasicFunctionality() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

/// <summary>
/// Creates the default barcode scanner.
/// </summary>
task<void> Scenario1_BasicFunctionality::CreateDefaultScannerObject()
{
    return create_task(DeviceInformation::FindAllAsync(BarcodeScanner::GetDeviceSelector())).then([this](DeviceInformationCollection^ deviceCollection)
    {
        if (deviceCollection == nullptr || deviceCollection->Size == 0)
        {
            rootPage->NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType::ErrorMessage);
            return task_from_result();
        }

        DeviceInformation^ scannerInfo = deviceCollection->GetAt(0);
        return create_task(BarcodeScanner::FromIdAsync(scannerInfo->Id)).then([this](BarcodeScanner^ _scanner)
        {
            this->scanner = _scanner;
            if (this->scanner == nullptr)
            {
                rootPage->NotifyUser("Failed to create barcode scanner object.", NotifyType::ErrorMessage);
            }
        });
    });
}

/// <summary>
/// This method claims the barcode scanner
/// </summary>
task<void> Scenario1_BasicFunctionality::ClaimScanner()
{
    // claim the barcode scanner
    return create_task(scanner->ClaimScannerAsync()).then([this](ClaimedBarcodeScanner^ _claimedScanner)
    {
        this->claimedScanner = _claimedScanner;
        // enable the claimed barcode scanner
        if (claimedScanner == nullptr)
        {
            rootPage->NotifyUser("Claim barcode scanner failed.", NotifyType::ErrorMessage);
        }
    });
}

// <summary>
/// Setup the barcode scanner to be ready to receive the data events from the scan.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_BasicFunctionality::ScenarioStartScanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Creating barcode scanner object.", NotifyType::StatusMessage);

    // create the barcode scanner. 
    create_task(CreateDefaultScannerObject()).then([this](void)
    {
        if (scanner != nullptr)
        {
            // after successful creation, claim the scanner for exclusive use and enable it so that data reveived events are received.
            create_task(ClaimScanner()).then([this](void)
            {
                if (claimedScanner)
                {
                    // It is always a good idea to have a release device requested event handler. If this event is not handled, there are chances of another app can 
                    // claim ownsership of the barcode scanner.
                    releaseDeviceRequestedToken = claimedScanner->ReleaseDeviceRequested::add(ref new EventHandler<ClaimedBarcodeScanner^>(this, &Scenario1_BasicFunctionality::OnReleaseDeviceRequested));

                    // after successfully claiming, attach the datareceived event handler.
                    dataReceivedToken = claimedScanner->DataReceived::add(ref new TypedEventHandler<ClaimedBarcodeScanner^, BarcodeScannerDataReceivedEventArgs^>(this, &Scenario1_BasicFunctionality::OnDataReceived));

                    // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and 
                    // send the ScanDataLabel and ScanDataType in the DataReceived event
                    claimedScanner->IsDecodeDataEnabled = true;

                    // enable the scanner.
                    // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event 
                    // if the claimedScanner has not beed Enabled
                    create_task(claimedScanner->EnableAsync()).then([this](void)
                    {
                        rootPage->NotifyUser("Ready to scan. Device ID: " + claimedScanner->DeviceId, NotifyType::StatusMessage);

                        // reset the button state
                        ScenarioEndScanButton->IsEnabled = true;
                        ScenarioStartScanButton->IsEnabled = false;
                    });
                }
            });
        }
    });

}

/// <summary>
/// Event handler for the Release Device Requested event fired when barcode scanner receives Claim request from another application
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
void Scenario1_BasicFunctionality::OnReleaseDeviceRequested(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner ^args)
{
    // let us retain the device always. If it is not retained, this exclusive claim will be lost.
    args->RetainDevice();

    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        rootPage->NotifyUser("Event ReleaseDeviceRequested received. Retaining the barcode scanner.", NotifyType::StatusMessage);
    }));
}

/// <summary>
/// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner 
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
void Scenario1_BasicFunctionality::OnDataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner ^sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs ^args)
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
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_BasicFunctionality::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedTo(e);
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario1_BasicFunctionality::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedFrom(e);
}

/// <summary>
/// Event handler for End Scan Button Click. 
/// Releases the Barcode Scanner and resets the text in the UI
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario1_BasicFunctionality::ScenarioEndScanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Reset the Scenario state
/// </summary>
void Scenario1_BasicFunctionality::ResetTheScenarioState()
{
    if (claimedScanner != nullptr)
    {
        // Detach the event handlers
        claimedScanner->DataReceived::remove(dataReceivedToken);
        claimedScanner->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken);

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
    ScenarioEndScanButton->IsEnabled = false;
    ScenarioStartScanButton->IsEnabled = true;
}
