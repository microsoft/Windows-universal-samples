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
#include "Scenario3_MultipleClaims.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;

Scenario3_MultipleClaims::Scenario3_MultipleClaims()
{
    InitializeComponent();
}

void Scenario3_MultipleClaims::ClaimSamePrinter_Click()
{
    ClaimResultText->Text = "";
    rootPage->NotifyUser("", NotifyType::StatusMessage);


    if (rootPage->Printer == nullptr)
    {
        rootPage->NotifyUser("Use scenario 1 to find a printer first.", NotifyType::ErrorMessage);
        return;
    }

    create_task(PosPrinter::FromIdAsync(rootPage->Printer->DeviceId)).then([this](PosPrinter^ printer)
    {
        if (printer == nullptr)
        {
            rootPage->NotifyUser("Cannot create same printer as scenario 1.", NotifyType::ErrorMessage);
        }
        else
        {
            create_task(printer->ClaimPrinterAsync()).then([this, printer](ClaimedPosPrinter^ claimedPrinter)
            {
                if (claimedPrinter != nullptr)
                {
                    ClaimResultText->Text = "Claimed the printer.";

                    // This scenario doesn't do anything with the printer aside from claim it.
                    // Release our claim by closing the claimed printer.
                    delete claimedPrinter;
                }
                else
                {
                    ClaimResultText->Text = "Did not claim the printer.";
                }
                // Close the duplicate printer.
                delete printer;
            });
        }
    });
}