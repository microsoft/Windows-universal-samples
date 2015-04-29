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
#include "Scenario3_MultipleReceipt.xaml.h"

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

Scenario3_MultipleReceipt::Scenario3_MultipleReceipt() : rootPage(MainPage::Current)
{
    InitializeComponent();
}

//
//Claims printer instance 1
//
void Scenario3_MultipleReceipt::Claim1_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (printerInstance1 == nullptr)
    {
        rootPage->NotifyUser("Cound not claim printer instance 1. Make sure you find printer first.", NotifyType::ErrorMessage);
        return;
    }

    create_task(ClaimAndEnablePrinter1());
}

//
//Releases claim of printer instance 1
//
void Scenario3_MultipleReceipt::Release1_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (claimedPrinter1 != nullptr)
    {
        claimedPrinter1->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken1);
        delete claimedPrinter1;
        claimedPrinter1 = nullptr;
        rootPage->NotifyUser("Released claimed Instance 1", NotifyType::StatusMessage);
    }
    else
    {
        rootPage->NotifyUser("Instance 1 not claimed to release", NotifyType::StatusMessage);
    }
}

//
//Claims instance 2.
//
void Scenario3_MultipleReceipt::Claim2_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (printerInstance1 == nullptr)
    {
        rootPage->NotifyUser("Cound not claim printer instance 1. Make sure you find printer first.", NotifyType::ErrorMessage);
        return;
    }

    create_task(ClaimAndEnablePrinter2());
}

//
//Releases claim of printer instance 2.
//
void Scenario3_MultipleReceipt::Release2_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (claimedPrinter2 != nullptr)
    {
        claimedPrinter2->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken2);
        delete claimedPrinter2;
        claimedPrinter2 = nullptr;
        rootPage->NotifyUser("Released claimed Instance 2", NotifyType::StatusMessage);
    }
    else
    {
        rootPage->NotifyUser("Instance 2 not claimed to release", NotifyType::StatusMessage);
    }
}

//
//Actual claim method task that claims and enables printer instance 1 asynchronously. It then adds the releasedevicerequested event handler as well to the claimed printer.
//
task<void> Scenario3_MultipleReceipt::ClaimAndEnablePrinter1()
{
    if (claimedPrinter1 == nullptr)
    {
        return create_task(printerInstance1->ClaimPrinterAsync()).then([this](ClaimedPosPrinter^ _claimedPrinter)
        {
            this->claimedPrinter1 = _claimedPrinter;
            if (claimedPrinter1 != nullptr)
            {
                rootPage->NotifyUser("Claimed Printer Instance 1", NotifyType::StatusMessage);
                create_task(claimedPrinter1->EnableAsync()).then([this](bool success)
                {
                    if (success)
                    {
                        releaseDeviceRequestedToken1 = claimedPrinter1->ReleaseDeviceRequested::add
                            (ref new TypedEventHandler<ClaimedPosPrinter^, PosPrinterReleaseDeviceRequestedEventArgs^ >(this, &Scenario3_MultipleReceipt::ClaimedPrinter1_ReleaseDeviceRequested));
                    }
                    else
                    {
                        rootPage->NotifyUser("Could not enable printer instance 1", NotifyType::ErrorMessage);
                    }
                });
            }
            else
            {
                rootPage->NotifyUser("Claim Printer failed for Instance 1. Probably because Instance 2 is holding on to it.", NotifyType::ErrorMessage);
            }
        });
    }

    return task_from_result();
}

//
//Actual claim method task that claims and enables printer instance 2 asynchronously. It then adds the releasedevicerequested event handler as well to the claimed printer.
//
task<void> Scenario3_MultipleReceipt::ClaimAndEnablePrinter2()
{
    if (printerInstance2 == nullptr)
    {
        rootPage->NotifyUser("Cound not claim printer. Make sure you find printer first.", NotifyType::ErrorMessage);
        return task_from_result();
    }
    else if (claimedPrinter2 == nullptr)
    {
        return create_task(printerInstance2->ClaimPrinterAsync()).then([this](ClaimedPosPrinter^ _claimedPrinter)
        {
            this->claimedPrinter2 = _claimedPrinter;
            if (claimedPrinter2 != nullptr)
            {
                rootPage->NotifyUser("Claimed Printer Instance 2", NotifyType::StatusMessage);
                create_task(claimedPrinter2->EnableAsync()).then([this](bool success)
                {
                    if (success)
                    {
                        releaseDeviceRequestedToken2 = claimedPrinter2->ReleaseDeviceRequested::add
                            (ref new TypedEventHandler<ClaimedPosPrinter^, PosPrinterReleaseDeviceRequestedEventArgs^ >(this, &Scenario3_MultipleReceipt::ClaimedPrinter2_ReleaseDeviceRequested));
                    }
                    else
                    {
                        rootPage->NotifyUser("Could not enable printer instance 2", NotifyType::ErrorMessage);
                    }
                });
            }
            else
            {
                rootPage->NotifyUser("Claim Printer failed for Instance 2. Probably because Instance 1 is holding on to it.", NotifyType::ErrorMessage);
            }
        });
    }

    return task_from_result();
}

//
//PosPrinter GetDeviceSelector gets the string format used to search for pos printer. This is then used to find any pos printers.
//The method then takes the first printer id found and tries to create two instances for that printer.
//
void Scenario3_MultipleReceipt::FindReceiptPrinter_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (printerInstance1 == nullptr)
    {
        rootPage->NotifyUser("Finding printer", NotifyType::StatusMessage);
        create_task(DeviceInformation::FindAllAsync(PosPrinter::GetDeviceSelector())).then([this](DeviceInformationCollection^ deviceCollection)
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
                String^ deviceId = printerInfo->Id;
                return create_task(PosPrinter::FromIdAsync(deviceId)).then([this, deviceId](PosPrinter^ _printer)
                {
                    printerInstance1 = _printer;;
                    if (printerInstance1 != nullptr)
                    {
                        if (printerInstance1->Capabilities->Receipt->IsPrinterPresent)
                        {
                            rootPage->NotifyUser("Got Printer instance 1 with Device Id : " + printerInstance1->DeviceId, NotifyType::StatusMessage);
                            create_task(PosPrinter::FromIdAsync(deviceId)).then([this](PosPrinter^ _printer)
                            {
                                printerInstance2 = _printer;
                                if (printerInstance2 != nullptr)
                                {
                                    rootPage->NotifyUser("Got Printer Instance 2 with Device Id : " + printerInstance2->DeviceId, NotifyType::StatusMessage);
                                }
                            });
                        }
                        else
                        {
                            printerInstance1 = nullptr;
                            rootPage->NotifyUser("No receipt printer found. ", NotifyType::ErrorMessage);
                        }
                    }
                });
            }
        });
    }
}

//
//If user checks the check box to not release instance 1, it then calls RetainDeviceAsync method to retain the claim on that printer else it releases the claim.
//
void Scenario3_MultipleReceipt::ClaimedPrinter1_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedPrinter, PosPrinterReleaseDeviceRequestedEventArgs ^args)
{
    if (IsAnImportantTransactionInstance1)
    {
        create_task(claimedPrinter->RetainDeviceAsync()).then([this](bool success)
        {
            if (success)
            {
                Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
                {
                    rootPage->NotifyUser("Instance 1 retained device.", NotifyType::StatusMessage);
                }));
            }
        });
    }
    else
    {
        claimedPrinter1->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken1);
        delete claimedPrinter1;
        claimedPrinter1 = nullptr;
    }
}

//
//If user checks the check box to not release instance 2, it then calls RetainDeviceAsync method to retain the claim on that printer else it releases the claim.
//
void Scenario3_MultipleReceipt::ClaimedPrinter2_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedPrinter, PosPrinterReleaseDeviceRequestedEventArgs ^args)
{
    if (IsAnImportantTransactionInstance2)
    {
        create_task(claimedPrinter->RetainDeviceAsync()).then([this](bool success)
        {
            if (success)
            {
                Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
                {
                    rootPage->NotifyUser("Instance 2 retained device.", NotifyType::StatusMessage);
                }));
            }
        });
    }
    else
    {
        claimedPrinter2->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken2);
        delete claimedPrinter2;
        claimedPrinter2 = nullptr;
    }
}

//
//Prints the line that is in the textbox for instance 1
//
void Scenario3_MultipleReceipt::PrintLine1_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (claimedPrinter1 != nullptr)
    {
        ReceiptPrintJob^ job = claimedPrinter1->Receipt->CreateJob();
        job->PrintLine(txtPrintLine1->Text);

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

//
//Prints the line that is in the textbox for instance 2
//
void Scenario3_MultipleReceipt::PrintLine2_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    if (claimedPrinter2 != nullptr)
    {
        ReceiptPrintJob^ job = claimedPrinter2->Receipt->CreateJob();
        job->PrintLine(txtPrintLine2->Text);

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

void Scenario3_MultipleReceipt::EndScenario_Click(Platform::Object ^sender, RoutedEventArgs^ e)
{
    ResetTheScenarioState();
    rootPage->NotifyUser("Disconnected Printer", NotifyType::StatusMessage);
}

void Scenario3_MultipleReceipt::chkInstance1_Checked(Platform::Object^ sender, RoutedEventArgs^ e)
{
    IsAnImportantTransactionInstance1 = true;
}

void Scenario3_MultipleReceipt::chkInstance2_Checked(Platform::Object^ sender, RoutedEventArgs^ e)
{
    IsAnImportantTransactionInstance2 = true;
}

void Scenario3_MultipleReceipt::chkInstance2_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e)
{
    IsAnImportantTransactionInstance2 = false;
}

void Scenario3_MultipleReceipt::chkInstance1_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e)
{
    IsAnImportantTransactionInstance1 = false;
}

//
//Clean out all claimed instances.
//
void Scenario3_MultipleReceipt::ResetTheScenarioState()
{
    IsAnImportantTransactionInstance1 = true;
    IsAnImportantTransactionInstance2 = false;
    chkInstance1->IsChecked = true;
    chkInstance2->IsChecked = false;

    if (printerInstance1 != nullptr)
    {
        delete printerInstance1;
        printerInstance1 = nullptr;
    }

    if (claimedPrinter1 != nullptr)
    {
        claimedPrinter1->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken1);
        delete claimedPrinter1;
        claimedPrinter1 = nullptr;
    }

    if (claimedPrinter2 != nullptr)
    {
        claimedPrinter2->ReleaseDeviceRequested::remove(releaseDeviceRequestedToken2);
        delete claimedPrinter2;
        claimedPrinter2 = nullptr;
    }

    if (printerInstance2 != nullptr)
    {
        delete printerInstance2;
        printerInstance2 = nullptr;
    }

}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void Scenario3_MultipleReceipt::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario3_MultipleReceipt::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}
