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
#include "Scenario1_ReceiptPrinter.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Concurrency;
using namespace Windows::Devices::PointOfService;
using namespace Windows::UI::Core;
using namespace Windows::Devices::Enumeration;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario1_ReceiptPrinter::Scenario1_ReceiptPrinter() : rootPage(MainPage::Current)
{
    InitializeComponent();
    ResetTheScenarioState();
}

//
//Creates multiple tasks, first to find a receipt printer, then to claim the printer and then to enable and add releasedevicerequested event handler.
//
void Scenario1_ReceiptPrinter::FindClaimEnable_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    create_task(FindReceiptPrinter()).then([this]()
    {
        if (printer != nullptr)
        {
            create_task(ClaimPrinter()).then([this](void)
            {
                if (claimedPrinter)
                {
                    create_task(EnableAsync()).then([this](void)
                    {
                        releaseDeviceRequestedToken = claimedPrinter->ReleaseDeviceRequested::add
                            (ref new TypedEventHandler<ClaimedPosPrinter^, PosPrinterReleaseDeviceRequestedEventArgs^ >(this, &Scenario1_ReceiptPrinter::ClaimedPrinter_ReleaseDeviceRequested));
                    });
                }
            });
        }
    });
}

//
//Default checkbox selection makes it an important transaction, hence we do not release claim when we get a release devicerequested event.
//
void Scenario1_ReceiptPrinter::ClaimedPrinter_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedInstance, PosPrinterReleaseDeviceRequestedEventArgs ^args)
{
    if (IsAnImportantTransaction)
    {
        create_task(claimedInstance->RetainDeviceAsync()).then([this](bool success)
        {
            if (success)
            {
                rootPage->NotifyUser("Retained Device", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Could not retain device.", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        ResetTheScenarioState();
    }
}

//
//Prints the line that is in the textbox.
//
void Scenario1_ReceiptPrinter::PrintLine_Click(Platform::Object ^sender, RoutedEventArgs e)
{
    if (claimedPrinter != nullptr)
    {
        ReceiptPrintJob^ job = claimedPrinter->Receipt->CreateJob();
        job->PrintLine(txtPrintLine->Text);

        create_task(job->ExecuteAsync()).then([this](bool status)
        {
            if (status)
            {
                rootPage->NotifyUser("Printed line", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Could not Print line", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Could not Print line. No claimed printer found.", NotifyType::ErrorMessage);
    }

}

void Scenario1_ReceiptPrinter::PrintReceipt_Click(Platform::Object ^sender, RoutedEventArgs e)
{
    rootPage->NotifyUser("Trying to Print Receipt", NotifyType::StatusMessage);
    PrintReceipt();

}

void Scenario1_ReceiptPrinter::EndScenario_Click(Platform::Object ^sender, RoutedEventArgs e)
{
    ResetTheScenarioState();
    rootPage->NotifyUser("Disconnected Printer", NotifyType::StatusMessage);
}

//
//Actual claim method task that claims the printer asynchronously
//
task<void> Scenario1_ReceiptPrinter::ClaimPrinter()
{
    if (claimedPrinter == nullptr)
    {
        return create_task(printer->ClaimPrinterAsync()).then([this](ClaimedPosPrinter^ _claimedPrinter)
        {
            this->claimedPrinter = _claimedPrinter;
            if (claimedPrinter != nullptr)
            {
                rootPage->NotifyUser("Claimed Printer", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Claim Printer failed", NotifyType::ErrorMessage);
            }
        });
    }

    return task_from_result();
}

task<void> Scenario1_ReceiptPrinter::EnableAsync()
{
    if (claimedPrinter)
    {
        return create_task(claimedPrinter->EnableAsync()).then([this](bool)
        {
            rootPage->NotifyUser("Enabled printer.", NotifyType::StatusMessage);
        });
    }
    else
    {
        rootPage->NotifyUser("Error: Printer not claimed anymore to enable.", NotifyType::ErrorMessage);
        return task_from_result();
    }
}

//
//Prints a sample receipt.
//
void Scenario1_ReceiptPrinter::PrintReceipt()
{
    if (printer != nullptr && claimedPrinter != nullptr)
    {
        String^ receiptString = "======================\n" +
                                "|   Sample Header    |\n" +
                                "======================\n" +
                                "Item             Price\n" +
                                "----------------------\n" +
                                "Books            10.40\n" +
                                "Games             9.60\n" +
                                "----------------------\n" +
                                "Total----------- 20.00\n";

        ReceiptPrintJob^ merchantJob = claimedPrinter->Receipt->CreateJob();
        String^ merchantFooter = GetMerchantFooter();
        PrintLineFeedAndCutPaper(merchantJob, receiptString + merchantFooter);
        ReceiptPrintJob^ customerJob = claimedPrinter->Receipt->CreateJob();
        String^ customerFooter = GetCustomerFooter();
        PrintLineFeedAndCutPaper(customerJob, receiptString + customerFooter);



        create_task(merchantJob->ExecuteAsync()).then([this, customerJob](bool)
        {
            rootPage->NotifyUser("Printed merchant receipt.", NotifyType::StatusMessage);
            create_task(customerJob->ExecuteAsync()).then([this](bool)
            {
                rootPage->NotifyUser("Printed customer receipt.", NotifyType::StatusMessage);
            });
        });


    }
    else
    {
        rootPage->NotifyUser("Must find and claim printer before printing.", NotifyType::ErrorMessage);
    }
}

Platform::String^ Scenario1_ReceiptPrinter::GetMerchantFooter()
{
    return "\n" +
           "______________________\n" +
           "Tip\n" +
           "\n" +
           "______________________\n" +
           "Signature\n" +
           "\n" +
           "Merchant Copy\n";
}

Platform::String^ Scenario1_ReceiptPrinter::GetCustomerFooter()
{
    return "\n" +
           "______________________\n" +
           "Tip\n" +
           "\n" +
           "Customer Copy\n";
}

// Cut the paper after printing enough blank lines to clear the paper cutter.
void Scenario1_ReceiptPrinter::PrintLineFeedAndCutPaper(ReceiptPrintJob^ job, Platform::String^ receipt)
{
    if (printer != nullptr && claimedPrinter != nullptr)
    {
        // Passing a multi-line string to the Print method results in
        // smoother paper feeding than sending multiple single-line strings
        // to PrintLine.

        Platform::String^ feedString = "";
        for (unsigned int n = 0; n < claimedPrinter->Receipt->LinesToPaperCut; n++)
        {
            feedString += "\n";
        }
        job->Print(receipt + feedString);
        if (printer->Capabilities->Receipt->CanCutPaper)
        {
            job->CutPaper();
        }
    }
}

task<void> Scenario1_ReceiptPrinter::FindReceiptPrinter()
{
    if (printer == nullptr)
    {
        rootPage->NotifyUser("Finding printer", NotifyType::StatusMessage);
        return DeviceHelpers::GetFirstReceiptPrinterAsync().then([this](PosPrinter^ _printer)
        {
            printer = _printer;
            if (printer != nullptr)
            {
                rootPage->NotifyUser("Got Printer with Device Id : " + printer->DeviceId, NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Could not get printer.", NotifyType::ErrorMessage);
            }
        });
    }
    return task_from_result();
}

//
//Remove event handler and delete claimed instance.
//
void Scenario1_ReceiptPrinter::ResetTheScenarioState()
{
    if (claimedPrinter != nullptr)
    {
        claimedPrinter->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken);
        delete claimedPrinter;
        claimedPrinter = nullptr;
    }

    if (printer != nullptr)
    {
        delete printer;
        printer = nullptr;
    }
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario1_ReceiptPrinter::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario1_ReceiptPrinter::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}
