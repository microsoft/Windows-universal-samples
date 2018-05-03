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
#include "MainPage.xaml.h"
#include "SampleConfiguration.h"
#include "DeviceHelpers.h"

using namespace Concurrency;
using namespace Platform;
using namespace SDKTemplate;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "DataReceived Event", "SDKTemplate.Scenario1_BasicFunctionality" },
    { "Release/Retain Functionality", "SDKTemplate.Scenario2_MultipleScanners" },
    { "Active Symbologies", "SDKTemplate.Scenario3_ActiveSymbologies" },
    { "Symbology Attributes", "SDKTemplate.Scenario4_SymbologyAttributes" },
    { "Displaying a Barcode Preview", "SDKTemplate.Scenario5_DisplayingBarcodePreview" }
};

task<BarcodeScanner^> DeviceHelpers::GetFirstBarcodeScannerAsync(PosConnectionTypes connectionTypes)
{
    return DeviceHelpers::GetFirstDeviceAsync(BarcodeScanner::GetDeviceSelector(connectionTypes),
        [](String^ id) { return create_task(BarcodeScanner::FromIdAsync(id)); });
}
