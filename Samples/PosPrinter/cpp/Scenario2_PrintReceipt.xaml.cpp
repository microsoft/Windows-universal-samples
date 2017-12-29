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
#include "Scenario2_PrintReceipt.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_PrintReceipt::Scenario2_PrintReceipt()
{
    InitializeComponent();
}

bool Scenario2_PrintReceipt::IsPrinterClaimed()
{
    if (rootPage->ClaimedPrinter != nullptr)
    {
        rootPage->NotifyUser("", NotifyType::StatusMessage);
        return true;
    }
    else
    {
        rootPage->NotifyUser("Use scenario 1 to find, claim, and enable a receipt printer.", NotifyType::ErrorMessage);
        return false;
    }
}

task<bool> Scenario2_PrintReceipt::ExecuteJobAndReportResultAsync(ReceiptPrintJob^ job)
{
    return create_task(job->ExecuteAsync()).then([this](bool success)
    {
        if (success)
        {
            rootPage->NotifyUser("Printing complete.", NotifyType::StatusMessage);
        }
        else
        {
            String^ reason;
            ClaimedReceiptPrinter^ receipt = rootPage->ClaimedPrinter->Receipt;
            if (receipt->IsCartridgeEmpty)
            {
                reason = "Printer is out of ink. Please replace cartridge.";
            }
            else if (receipt->IsCartridgeRemoved)
            {
                reason = "Printer cartridge is missing. Please replace cartridge.";
            }
            else if (receipt->IsCoverOpen)
            {
                reason = "Printer cover is open. Please close it.";
            }
            else if (receipt->IsHeadCleaning)
            {
                reason = "Printer is currently cleaning the cartridge. Please wait until cleaning has completed.";
            }
            else if (receipt->IsPaperEmpty)
            {
                reason = "Printer is out of paper. Please insert a new roll.";
            }
            else
            {
                reason = "Unable to print.";
            }
            rootPage->NotifyUser(reason, NotifyType::ErrorMessage);
        }
        return success;
    });
}

#pragma region Print a single line of text
void Scenario2_PrintReceipt::PrintLine_Click()
{
    if (!IsPrinterClaimed())
    {
        return;
    }

    ReceiptPrintJob^ job = rootPage->ClaimedPrinter->Receipt->CreateJob();
    job->PrintLine(PrintLineTextBox->Text);

    ExecuteJobAndReportResultAsync(job);
}
#pragma endregion

#pragma region Print a sample receipt
void Scenario2_PrintReceipt::PrintReceipt_Click()
{
    if (!IsPrinterClaimed())
    {
        return;
    }

    String^ receiptString =
        "======================\n" +
        "|   Sample Header    |\n" +
        "======================\n" +
        "Item             Price\n" +
        "----------------------\n" +
        "Books            10.40\n" +
        "Games             9.60\n" +
        "----------------------\n" +
        "Total            20.00\n";

    rootPage->NotifyUser("Printing receipt...", NotifyType::StatusMessage);

    // The job consists of two receipts, one for the merchant and one
    // for the customer.
    ReceiptPrintJob^ job = rootPage->ClaimedPrinter->Receipt->CreateJob();
    PrintLineFeedAndCutPaper(job, receiptString + GetMerchantFooter());
    PrintLineFeedAndCutPaper(job, receiptString + GetCustomerFooter());

    ExecuteJobAndReportResultAsync(job);
}

Platform::String^ Scenario2_PrintReceipt::GetMerchantFooter()
{
    return
        "\n" +
        "______________________\n" +
        "Signature\n" +
        "\n" +
        "Merchant Copy\n";
}

Platform::String^ Scenario2_PrintReceipt::GetCustomerFooter()
{
    return
        "\n" +
        "Customer Copy\n";
}

// Cut the paper after printing enough blank lines to clear the paper cutter.
void Scenario2_PrintReceipt::PrintLineFeedAndCutPaper(ReceiptPrintJob^ job, String^ receipt)
{
    // Passing a multi-line string to the Print method results in
    // smoother paper feeding than sending multiple single-line strings
    // to PrintLine.
    String^ feedString = "";
    for (unsigned int n = 0; n < rootPage->ClaimedPrinter->Receipt->LinesToPaperCut; n++)
    {
        feedString += "\n";
    }
    job->Print(receipt + feedString);
    if (rootPage->Printer->Capabilities->Receipt->CanCutPaper)
    {
        job->CutPaper();
    }
}
#pragma endregion

#pragma region Print a bitmap
void Scenario2_PrintReceipt::PrintBitmap_Click()
{
    if (!IsPrinterClaimed())
    {
        return;
    }

    LoadLogoBitmapAsync().then([this](BitmapFrame^ logoFrame)
    {
        ReceiptPrintJob^ job = rootPage->ClaimedPrinter->Receipt->CreateJob();
        rootPage->ClaimedPrinter->Receipt->IsLetterQuality = true;
        job->PrintBitmap(logoFrame, PosPrinterAlignment::Center);
        return ExecuteJobAndReportResultAsync(job);
    });
}

task<BitmapFrame^> Scenario2_PrintReceipt::LoadLogoBitmapAsync()
{
    Uri^ uri = ref new Uri("ms-appx:///Assets/coffee-logo.png");
    return create_task(StorageFile::GetFileFromApplicationUriAsync(uri)).then([](StorageFile^ file)
    {
        return create_task(file->OpenReadAsync());
    }).then([](IRandomAccessStream^ readStream)
    {
        return create_task(BitmapDecoder::CreateAsync(readStream));
    }).then([](BitmapDecoder^ decoder)
    {
        return create_task(decoder->GetFrameAsync(0));
    });
}
#pragma endregion

#pragma region Print a barcode
void Scenario2_PrintReceipt::PrintBarcode_Click()
{
    if (!IsPrinterClaimed())
    {
        return;
    }

    rootPage->ClaimedPrinter->Receipt->IsLetterQuality = true;
    ReceiptPrintJob^ job = rootPage->ClaimedPrinter->Receipt->CreateJob();
    job->PrintBarcode(BarcodeText->Text, BarcodeSymbologies::Upca, 60, 3, PosPrinterBarcodeTextPosition::Below, PosPrinterAlignment::Center);
    ExecuteJobAndReportResultAsync(job);
}
#pragma endregion
