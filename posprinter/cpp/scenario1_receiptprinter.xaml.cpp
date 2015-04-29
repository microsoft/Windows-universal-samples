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

        ReceiptPrintJob^ job = claimedPrinter->Receipt->CreateJob();
        job->PrintLine("======================");
        job->PrintLine("|   Sample Header    |");
        job->PrintLine("======================");

        job->PrintLine("Item             Price");
        job->PrintLine("----------------------");

        job->PrintLine("Books            10.40");
        job->PrintLine("Games             9.60");
        job->PrintLine("----------------------");
        job->PrintLine("Total----------- 20.00");

        ReceiptPrintJob^ merchantFooter = GetMerchantFooter(claimedPrinter);
        ReceiptPrintJob^ customerFooter = GetCustomerFooter(claimedPrinter);

        create_task(job->ExecuteAsync()).then([this, customerFooter, merchantFooter, job](bool)
        {
            create_task(customerFooter->ExecuteAsync()).then([this, merchantFooter, job](bool)
            {
                rootPage->NotifyUser("Printed customer receipt.", NotifyType::StatusMessage);

                create_task(job->ExecuteAsync()).then([this, merchantFooter](bool)
                {
                    create_task(merchantFooter->ExecuteAsync()).then([this](bool)
                    {
                        rootPage->NotifyUser("Printed merchant receipt.", NotifyType::StatusMessage);
                    });
                });
            });
        });


    }
    else
    {
        rootPage->NotifyUser("Must find and claim printer before printing.", NotifyType::ErrorMessage);
    }
}

ReceiptPrintJob^ Scenario1_ReceiptPrinter::GetMerchantFooter(ClaimedPosPrinter ^ claimedInstance)
{
    ReceiptPrintJob^ merchantFooter = claimedInstance->Receipt->CreateJob();
    merchantFooter->PrintLine();
    merchantFooter->PrintLine("______________________");
    merchantFooter->PrintLine("Tip");
    merchantFooter->PrintLine();
    merchantFooter->PrintLine("______________________");
    merchantFooter->PrintLine("Signature");
    merchantFooter->PrintLine();
    merchantFooter->PrintLine("Merchant Copy");
    merchantFooter->CutPaper();

    return merchantFooter;
}

ReceiptPrintJob^ Scenario1_ReceiptPrinter::GetCustomerFooter(ClaimedPosPrinter ^ claimedInstance)
{
    ReceiptPrintJob^ customerFooter = claimedInstance->Receipt->CreateJob();
    customerFooter->PrintLine();
    customerFooter->PrintLine("______________________");
    customerFooter->PrintLine("Tip");
    customerFooter->PrintLine();
    customerFooter->PrintLine("Customer Copy");
    customerFooter->CutPaper();

    return customerFooter;
}

//
//PosPrinter GetDeviceSelector gets the string format used to search for pos printer. This is then used to find any pos printers.
//The method then takes the first printer id found and tries to create an instance of that printer.
//
task<void> Scenario1_ReceiptPrinter::FindReceiptPrinter()
{
    if (printer == nullptr)
    {
        rootPage->NotifyUser("Finding printer", NotifyType::StatusMessage);
        return create_task(DeviceInformation::FindAllAsync(PosPrinter::GetDeviceSelector())).then([this](DeviceInformationCollection^ deviceCollection)
        {
            if (deviceCollection->Size == 0)
            {
                rootPage->NotifyUser("Did not find any printers", NotifyType::ErrorMessage);
                return task_from_result();
            }
            else
            {
                //Try to get the first printer that matched.
                DeviceInformation^ printerInfo = deviceCollection->GetAt(0);
                return create_task(PosPrinter::FromIdAsync(printerInfo->Id)).then([this](PosPrinter^ _printer)
                {
                    printer = _printer;
                    if (printer != nullptr)
                    {
                        if (printer->Capabilities->Receipt->IsPrinterPresent)
                        {
                            rootPage->NotifyUser("Got Printer with Device Id : " + printer->DeviceId, NotifyType::StatusMessage);
                            return;
                        }
                    }
                    else
                    {
                        rootPage->NotifyUser("Could not get printer.", NotifyType::ErrorMessage);
                    }
                });
            }
        });

        rootPage->NotifyUser("No Printer found", NotifyType::ErrorMessage);
    }

    rootPage->NotifyUser("done finding printer", NotifyType::StatusMessage);
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
