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
#include "Scenario2_MultipleScanners.xaml.h"
#include "MainPage.xaml.h"
#include <Windows.h>

using namespace SDKTemplate;

using namespace std;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238


Scenario2_MultipleScanners::Scenario2_MultipleScanners()
{
    InitializeComponent();
    rootPage = MainPage::Current;
}

/// <summary>
/// Creates the default barcode scanner.
/// </summary>
task<void> Scenario2_MultipleScanners::CreateDefaultScannerObject(BarcodeScannerInstance instance)
{
    return create_task(BarcodeScanner::GetDefaultAsync()).then([this, instance](BarcodeScanner^ _scanner)
    {
        if (_scanner == nullptr)
        {
            rootPage->NotifyUser("Barcode scanner not found. Please connect a barcode scanner.", NotifyType::ErrorMessage);
            return;
        }

        switch (instance)
        {
        case BarcodeScannerInstance::Instance1:
            scannerInstance1 = _scanner;
            break;
        case BarcodeScannerInstance::Instance2:
            scannerInstance2 = _scanner;
            break;
        default:
            return;
        }

    });
}

/// <summary>
/// This method claims the barcode scanner
/// </summary>
task<void> Scenario2_MultipleScanners::ClaimScanner(BarcodeScannerInstance instance)
{
    // claim the barcode scanner

    BarcodeScanner^ scanner = nullptr;
    Platform::String^ strInstance;
    switch (instance)
    {
    case BarcodeScannerInstance::Instance1:
        scanner = this->scannerInstance1;
        strInstance = "Instance 1";
        break;
    case BarcodeScannerInstance::Instance2:
        scanner = this->scannerInstance2;
        strInstance = "Instance 2";
        break;
    default:
        break;
    }

    return create_task(scanner->ClaimScannerAsync()).then([this, instance, strInstance](ClaimedBarcodeScanner^ claimedBarcodeScanner)
    {
        if (claimedBarcodeScanner == nullptr)
            rootPage->NotifyUser(strInstance + " claim barcode scanner failed.", NotifyType::ErrorMessage);

        switch (instance)
        {
        case BarcodeScannerInstance::Instance1:
            this->claimedBarcodeScannerInstance1 = claimedBarcodeScanner;
            break;
        case BarcodeScannerInstance::Instance2:
            this->claimedBarcodeScannerInstance2 = claimedBarcodeScanner;
            break;
        default:
            delete claimedBarcodeScanner;
            claimedBarcodeScanner = nullptr;
            break;
        }

    });
}

/// <summary>
/// This method enables the barcode scanner.
/// </summary>
task<void> Scenario2_MultipleScanners::EnableScanner(BarcodeScannerInstance instance)
{
    ClaimedBarcodeScanner^ claimedBarcodeScanner = nullptr;
    Platform::String^ strInstance;
    switch (instance)
    {
    case BarcodeScannerInstance::Instance1:
        claimedBarcodeScanner = this->claimedBarcodeScannerInstance1;
        strInstance = "Instance 1";
        break;
    case BarcodeScannerInstance::Instance2:
        claimedBarcodeScanner = this->claimedBarcodeScannerInstance2;
        strInstance = "Instance 2";
        break;
    default:
        break;
    }
    // enable the barcode scanner
    return create_task(claimedBarcodeScanner->EnableAsync()).then([this, strInstance, claimedBarcodeScanner](void)
    {
        rootPage->NotifyUser(strInstance + " ready to scan. Device ID: " + claimedBarcodeScanner->DeviceId, NotifyType::StatusMessage);
    });

}

/// <summary>
/// Sets the UI state to match the current active Instance.
/// </summary>
void Scenario2_MultipleScanners::SetUI(BarcodeScannerInstance instance)
{
    Instance1Border->BorderBrush = ref new SolidColorBrush(Colors::Gray);
    Instance2Border->BorderBrush = ref new SolidColorBrush(Colors::Gray);

    if (instance == BarcodeScannerInstance::Instance2)
    {
        ScenarioStartScanningInstance2->IsEnabled = false;
        ScenarioStartScanningInstance1->IsEnabled = true;
        ScenarioEndScanningInstance2->IsEnabled = true;
        ScenarioEndScanningInstance1->IsEnabled = false;
        Instance2Border->BorderBrush = ref new SolidColorBrush(Colors::DarkBlue);

    }
    else if (instance == BarcodeScannerInstance::Instance1)
    {
        ScenarioStartScanningInstance1->IsEnabled = false;
        ScenarioStartScanningInstance2->IsEnabled = true;
        ScenarioEndScanningInstance1->IsEnabled = true;
        ScenarioEndScanningInstance2->IsEnabled = false;
        Instance1Border->BorderBrush = ref new SolidColorBrush(Colors::DarkBlue);
    }
}


/// <summary>
/// Resets the UI text blocks to initial state
/// </summary>
void Scenario2_MultipleScanners::ResetUI()
{
    Instance1Border->BorderBrush = ref new SolidColorBrush(Colors::Gray);
    Instance2Border->BorderBrush = ref new SolidColorBrush(Colors::Gray);

    ScanDataType1->Text = "No data";
    ScanData1->Text = "No data";
    DataLabel1->Text = "No data";
    ScanDataType2->Text = "No data";
    ScanData2->Text = "No data";
    DataLabel2->Text = "No data";

    ScenarioStartScanningInstance1->IsEnabled = true;
    ScenarioStartScanningInstance2->IsEnabled = true;
    ScenarioEndScanningInstance1->IsEnabled = false;
    ScenarioEndScanningInstance2->IsEnabled = false;
}

/// <summary>
/// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner instance 1
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
void Scenario2_MultipleScanners::OnDataReceivedInstance1(Windows::Devices::PointOfService::ClaimedBarcodeScanner ^sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs ^args)
{
    // read the data from the buffer and convert to string.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        DataLabel1->Text = GetDataLabelString(args->Report->ScanDataLabel, args->Report->ScanDataType);

        ScanData1->Text = GetDataString(args->Report->ScanData);

        ScanDataType1->Text = BarcodeSymbologies::GetName(args->Report->ScanDataType);
        
        rootPage->NotifyUser("Instance 1 received data from the barcode scanner.", NotifyType::StatusMessage);
    }));
}


/// <summary>
/// Event handler for the DataReceived event fired when a barcode is scanned by the barcode scanner instance 2
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the BarcodeScannerReport which contains the data obtained in the scan</param>
void Scenario2_MultipleScanners::OnDataReceivedInstance2(Windows::Devices::PointOfService::ClaimedBarcodeScanner ^sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs ^args)
{
    // read the data from the buffer and convert to string.
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        DataLabel2->Text = GetDataLabelString(args->Report->ScanDataLabel, args->Report->ScanDataType);

        ScanData2->Text = GetDataString(args->Report->ScanData);

        ScanDataType2->Text = BarcodeSymbologies::GetName(args->Report->ScanDataType);

        rootPage->NotifyUser("Instance 2 received data from the barcode scanner.", NotifyType::StatusMessage);
    }));
}


/// <summary>
/// Event handler for the Release Device Requested event fired when barcode scanner instance1 receives Claim request from another application
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
void Scenario2_MultipleScanners::OnReleaseDeviceRequestedInstance1(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner ^args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        // If instance1 retain is selected, we need to retain it, and this release device request will be ignored.
        if (Retain1->IsChecked->Value)
        {
            args->RetainDevice();
        }
        else
        {
            // release the claimed scanner instance1
            claimedBarcodeScannerInstance1->DataReceived::remove(dataReceivedTokenInstance1);
            claimedBarcodeScannerInstance1->ReleaseDeviceRequested::remove(releaseDeviceRequestedTokenInstance1);
            delete claimedBarcodeScannerInstance1;
            claimedBarcodeScannerInstance1 = nullptr;
        }
    }));
}

/// <summary>
/// Event handler for the Release Device Requested event fired when barcode scanner instance 2 receives Claim request from another application
/// </summary>
/// <param name="sender"></param>
/// <param name="args"> Contains the ClamiedBarcodeScanner that is sending this request</param>
void Scenario2_MultipleScanners::OnReleaseDeviceRequestedInstance2(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner ^args)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
        [this, args]()
    {
        // If instance1 retain is selected, we need to retain it, and this release device request will be ignored.
        if (Retain2->IsChecked->Value)
        {
            args->RetainDevice();
        }
        else
        {
            // release the claimed scanner instance1
            claimedBarcodeScannerInstance2->DataReceived::remove(dataReceivedTokenInstance2);
            claimedBarcodeScannerInstance2->ReleaseDeviceRequested::remove(releaseDeviceRequestedTokenInstance2);
            delete claimedBarcodeScannerInstance2;
            claimedBarcodeScannerInstance2 = nullptr;
        }
    }));
}

// <summary>
/// Setup the barcode scanner Instance 1 to be ready to receive the data events from the scan.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_MultipleScanners::ButtonStartScanningInstance1_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(CreateDefaultScannerObject(BarcodeScannerInstance::Instance1)).then([this](void)
    {
        if (scannerInstance1 != nullptr)
        {
            //after successful creation, claim the scanner for exclusive use and enable it so that data received events are received.
            create_task(ClaimScanner(BarcodeScannerInstance::Instance1)).then([this](void)
            {
                if (claimedBarcodeScannerInstance1 != nullptr)
                {
                    // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and 
                    // send the ScanDataLabel and ScanDataType in the DataReceived event
                    claimedBarcodeScannerInstance1->IsDecodeDataEnabled = true;

                    create_task(EnableScanner(BarcodeScannerInstance::Instance1)).then([this](void)
                    {

                        // after successfully claiming and enabling, attach the datareceived event handler.
                        // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event 
                        // if the activeClaimedBarcodeScanner has not been Enabled

                        dataReceivedTokenInstance1 = claimedBarcodeScannerInstance1->DataReceived::add(ref new TypedEventHandler<ClaimedBarcodeScanner^, BarcodeScannerDataReceivedEventArgs^>(this, &Scenario2_MultipleScanners::OnDataReceivedInstance1));
                        releaseDeviceRequestedTokenInstance1 = claimedBarcodeScannerInstance1->ReleaseDeviceRequested::add(ref new EventHandler<ClaimedBarcodeScanner^>(this, &Scenario2_MultipleScanners::OnReleaseDeviceRequestedInstance1));

                        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
                            [this]()
                        {
                            ResetUI();
                            SetUI(BarcodeScannerInstance::Instance1);
                        }));
                    });
                }
                else
                {
                    scannerInstance1 = nullptr;
                }
            });
        }
    });
}

// <summary>
/// Setup the barcode scanner Instance 2 to be ready to receive the data events from the scan.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_MultipleScanners::ButtonStartScanningInstance2_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    create_task(CreateDefaultScannerObject(BarcodeScannerInstance::Instance2)).then([this](void)
    {
        if (scannerInstance2 != nullptr)
        {
            //after successful creation, claim the scanner for exclusive use and enable it so that data received events are received.
            create_task(ClaimScanner(BarcodeScannerInstance::Instance2)).then([this](void)
            {
                if (claimedBarcodeScannerInstance2 != nullptr)
                {
                    // Ask the API to decode the data by default. By setting this, API will decode the raw data from the barcode scanner and 
                    // send the ScanDataLabel and ScanDataType in the DataReceived event
                    claimedBarcodeScannerInstance2->IsDecodeDataEnabled = true;

                    create_task(EnableScanner(BarcodeScannerInstance::Instance2)).then([this](void)
                    {

                        // after successfully claiming and enabling, attach the datareceived event handler.
                        // Note: If the scanner is not enabled (i.e. EnableAsync not called), attaching the event handler will not be any useful because the API will not fire the event 
                        // if the activeClaimedBarcodeScanner has not been Enabled

                        dataReceivedTokenInstance2 = claimedBarcodeScannerInstance2->DataReceived::add(ref new TypedEventHandler<ClaimedBarcodeScanner^, BarcodeScannerDataReceivedEventArgs^>(this, &Scenario2_MultipleScanners::OnDataReceivedInstance2));
                        releaseDeviceRequestedTokenInstance2 = claimedBarcodeScannerInstance2->ReleaseDeviceRequested::add(ref new EventHandler<ClaimedBarcodeScanner^>(this, &Scenario2_MultipleScanners::OnReleaseDeviceRequestedInstance2));

                        Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler(
                            [this]()
                        {
                            ResetUI();
                            SetUI(BarcodeScannerInstance::Instance2);
                        }));
                    });
                }
                else
                {
                    scannerInstance1 = nullptr;
                }
            });
        }
    });
}

/// <summary>
/// Event handler for End Instance 1
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_MultipleScanners::ButtonEndScanningInstance1_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // release the claimed scanner instance1
    if (claimedBarcodeScannerInstance1 != nullptr)
    {
        claimedBarcodeScannerInstance1->DataReceived::remove(dataReceivedTokenInstance1);
        claimedBarcodeScannerInstance1->ReleaseDeviceRequested::remove(releaseDeviceRequestedTokenInstance1);
        delete claimedBarcodeScannerInstance1;
        claimedBarcodeScannerInstance1 = nullptr;
    }

    if (scannerInstance1 != nullptr)
    {
        delete scannerInstance1;
        scannerInstance1 = nullptr;
    }

    rootPage->NotifyUser("Click the start scanning button to begin.", NotifyType::StatusMessage);

    ResetUI();
}

/// <summary>
/// Event handler for End Instance 2
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario2_MultipleScanners::ButtonEndScanningInstance2_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    if (claimedBarcodeScannerInstance2 != nullptr)
    {
        // release the claimed scanner instance1
        claimedBarcodeScannerInstance2->DataReceived::remove(dataReceivedTokenInstance2);
        claimedBarcodeScannerInstance2->ReleaseDeviceRequested::remove(releaseDeviceRequestedTokenInstance2);
        delete claimedBarcodeScannerInstance2;
        claimedBarcodeScannerInstance2 = nullptr;
    }

    if (scannerInstance2 != nullptr)
    {
        delete scannerInstance2;
        scannerInstance2 = nullptr;
    }

    rootPage->NotifyUser("Click the start scanning button to begin.", NotifyType::StatusMessage);

    ResetUI();
}

void Scenario2_MultipleScanners::ResetTheScenarioState()
{
    if (claimedBarcodeScannerInstance1 != nullptr)
    {
        claimedBarcodeScannerInstance1->DataReceived::remove(dataReceivedTokenInstance2);
        claimedBarcodeScannerInstance1->ReleaseDeviceRequested::remove(releaseDeviceRequestedTokenInstance2);
        delete claimedBarcodeScannerInstance1;
        claimedBarcodeScannerInstance1 = nullptr;
    }
    if (claimedBarcodeScannerInstance2 != nullptr)
    {
        claimedBarcodeScannerInstance2->DataReceived::remove(dataReceivedTokenInstance2);
        claimedBarcodeScannerInstance2->ReleaseDeviceRequested::remove(releaseDeviceRequestedTokenInstance2);
        delete claimedBarcodeScannerInstance2;
        claimedBarcodeScannerInstance2 = nullptr;
    }
    if (scannerInstance1 != nullptr)
    {
        delete scannerInstance1;
        scannerInstance1 = nullptr;
    }
    if (scannerInstance2 != nullptr)
    {
        delete scannerInstance2;
        scannerInstance2 = nullptr;
    }

    rootPage->NotifyUser("Click the start scanning button to begin.", NotifyType::StatusMessage);

    ResetUI();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario2_MultipleScanners::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedTo(e);
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2_MultipleScanners::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
    Page::OnNavigatedFrom(e);
}
