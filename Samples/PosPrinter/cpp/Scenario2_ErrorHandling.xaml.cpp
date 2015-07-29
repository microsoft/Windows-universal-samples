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
#include "Scenario2_ErrorHandling.xaml.h"

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

Scenario2_ErrorHandling::Scenario2_ErrorHandling() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

//
//Creates multiple tasks, first to find a receipt printer, then to claim the printer and then to enable and add releasedevicerequested event handler.
//
void Scenario2_ErrorHandling::FindClaimEnable_Click(Platform::Object^ sender, RoutedEventArgs^ e)
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
                            (ref new TypedEventHandler<ClaimedPosPrinter^, PosPrinterReleaseDeviceRequestedEventArgs^ >(this, &Scenario2_ErrorHandling::ClaimedPrinter_ReleaseDeviceRequested));
                    });
                }
            });
        }
    });
}

//
//Default checkbox selection makes it an important transaction, hence we do not release claim when we get a release devicerequested event.
//
void Scenario2_ErrorHandling::ClaimedPrinter_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedInstance, PosPrinterReleaseDeviceRequestedEventArgs ^args)
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
//Prints the line that is in the textbox. Then checks for different error scenarios, if an error were to occur.
//
void Scenario2_ErrorHandling::PrintLine_Click(Platform::Object ^sender, RoutedEventArgs e)
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
                if (claimedPrinter->Receipt->IsCartridgeEmpty)
                {
                    rootPage->NotifyUser("Printer is out of ink. Please replace cartridge.", NotifyType::StatusMessage);
                }
                else if (claimedPrinter->Receipt->IsCartridgeRemoved)
                {
                    rootPage->NotifyUser("Printer cartridge is missing. Please replace cartridge.", NotifyType::StatusMessage);
                }
                else if (claimedPrinter->Receipt->IsCoverOpen)
                {
                    rootPage->NotifyUser("Printer cover is open. Please close it.", NotifyType::StatusMessage);
                }
                else if (claimedPrinter->Receipt->IsHeadCleaning)
                {
                    rootPage->NotifyUser("Printer is currently cleaning the cartridge. Please wait until cleaning has completed.", NotifyType::StatusMessage);
                }
                else if (claimedPrinter->Receipt->IsPaperEmpty)
                {
                    rootPage->NotifyUser("Printer is out of paper. Please insert a new roll.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("Was not able to print line", NotifyType::StatusMessage);
                }
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Could not print line. No claimed printer found.", NotifyType::ErrorMessage);
    }

}

void Scenario2_ErrorHandling::EndScenario_Click(Platform::Object ^sender, RoutedEventArgs e)
{
    ResetTheScenarioState();
    rootPage->NotifyUser("Disconnected Printer", NotifyType::StatusMessage);
}

//
//Actual claim method task that claims the printer asynchronously
//
task<void> Scenario2_ErrorHandling::ClaimPrinter()
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

task<void> Scenario2_ErrorHandling::EnableAsync()
{
    if (claimedPrinter)
    {
        return create_task(claimedPrinter->EnableAsync()).then([this](bool success)
        {
            if (success)
            {
                rootPage->NotifyUser("Enabled printer.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Could not enable printer.", NotifyType::ErrorMessage);
            }
        });
    }

    rootPage->NotifyUser("Error: Printer not claimed anymore to enable.", NotifyType::ErrorMessage);
    return task_from_result();
}

//
//PosPrinter GetDeviceSelector gets the string format used to search for pos printer. This is then used to find any pos printers.
//The method then takes the first printer id found and tries to create an instance for that printer.
//
task<void> Scenario2_ErrorHandling::FindReceiptPrinter()
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
                        }
                    }
                });
            }
        });

        rootPage->NotifyUser("No Printer found", NotifyType::ErrorMessage);
    }

    rootPage->NotifyUser("Done finding printer", NotifyType::StatusMessage);
    return task_from_result();
}

//
//Remove event handler and delete claimed instance.
//
void Scenario2_ErrorHandling::ResetTheScenarioState()
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
void Scenario2_ErrorHandling::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario2_ErrorHandling::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}
