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
    { "Receipt Printer", "SDKTemplate.Scenario1_ReceiptPrinter" },
    { "Receipt Printer Error Handling", "SDKTemplate.Scenario2_ErrorHandling" },
    { "Multiple Receipt Printers", "SDKTemplate.Scenario3_MultipleReceipt" }
};

task<PosPrinter^> DeviceHelpers::GetFirstReceiptPrinterAsync(PosConnectionTypes connectionTypes)
{
    return DeviceHelpers::GetFirstDeviceAsync(PosPrinter::GetDeviceSelector(connectionTypes),
        [](String^ id)
    {
        return create_task(PosPrinter::FromIdAsync(id)).then([](PosPrinter^ printer)
        {
            if (printer && printer->Capabilities->Receipt->IsPrinterPresent)
            {
                return printer;
            }
            else
            {
                // Close the unwanted printer.
                delete printer;
                return static_cast<PosPrinter^>(nullptr);
            }
        });
    });
}
