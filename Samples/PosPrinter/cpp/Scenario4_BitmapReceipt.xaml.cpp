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
#include "Scenario4_BitmapReceipt.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

Scenario4_BitmapReceipt::Scenario4_BitmapReceipt() : rootPage(MainPage::Current)
{
    InitializeComponent();
    ResetTheScenarioState();
}

void Scenario4_BitmapReceipt::PrintBitmap_Click(Platform::Object ^sender, RoutedEventArgs^ e)
{
    rootPage->NotifyUser("Trying to Print Receipt", NotifyType::StatusMessage);
    PrintBitmap();
}

//
//Creates multiple tasks, first to find a receipt printer, then to claim the printer and then to enable and add releasedevicerequested event handler.
//
void Scenario4_BitmapReceipt::FindClaimEnable_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    FindReceiptPrinter().then([this]()
    {
        if (printer != nullptr)
        {
            ClaimPrinter().then([this]()
            {
                if (claimedPrinter)
                {
                    EnableAsync().then([this]()
                    {
                        releaseDeviceRequestedToken = claimedPrinter->ReleaseDeviceRequested +=
                            ref new TypedEventHandler<ClaimedPosPrinter^, PosPrinterReleaseDeviceRequestedEventArgs^>(this, &Scenario4_BitmapReceipt::ClaimedPrinter_ReleaseDeviceRequested);
                    });
                }
            });
        }
    });
}

void Scenario4_BitmapReceipt::IsImportantTransaction_Click(Object^ sender, RoutedEventArgs^ e)
{
    auto checkBox = safe_cast<CheckBox^>(sender);
    IsAnImportantTransaction = checkBox->IsChecked->Value;
}

//
//Default checkbox selection makes it an important transaction, hence we do not release claim when we get a release devicerequested event.
//
void Scenario4_BitmapReceipt::ClaimedPrinter_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedInstance, PosPrinterReleaseDeviceRequestedEventArgs ^args)
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


void Scenario4_BitmapReceipt::EndScenario_Click(Platform::Object ^sender, RoutedEventArgs^ e)
{
    ResetTheScenarioState();
    rootPage->NotifyUser("Disconnected Printer", NotifyType::StatusMessage);
}

//
//Actual claim method task that claims the printer asynchronously
//
task<void> Scenario4_BitmapReceipt::ClaimPrinter()
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

task<void> Scenario4_BitmapReceipt::EnableAsync()
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
void Scenario4_BitmapReceipt::PrintBitmap()
{
    if (printer != nullptr && claimedPrinter != nullptr)
    {
        LoadLogoBitmapAsync().then([this](BitmapFrame^ logoFrame)
        {
            ReceiptPrintJob^ job = claimedPrinter->Receipt->CreateJob();
            claimedPrinter->Receipt->IsLetterQuality = true;
            job->PrintBitmap(logoFrame, PosPrinterAlignment::Center);
            return create_task(job->ExecuteAsync());
        }).then([this](bool success)
        {
            if (success)
            {
                rootPage->NotifyUser("Bitmap printed successfully", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Was not able to print bitmap", NotifyType::ErrorMessage);
            }
        });
    }
    else
    {
        rootPage->NotifyUser("Must find and claim printer before printing.", NotifyType::ErrorMessage);
    }
}

task<BitmapFrame^> Scenario4_BitmapReceipt::LoadLogoBitmapAsync()
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

task<void> Scenario4_BitmapReceipt::FindReceiptPrinter()
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
void Scenario4_BitmapReceipt::ResetTheScenarioState()
{
    if (claimedPrinter != nullptr)
    {
        claimedPrinter->ReleaseDeviceRequested -= releaseDeviceRequestedToken;
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
void Scenario4_BitmapReceipt::OnNavigatedTo(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}

/// <summary>
/// Invoked when this page is no longer displayed.
/// </summary>
/// <param name="e"></param>
void Scenario4_BitmapReceipt::OnNavigatedFrom(NavigationEventArgs^ e)
{
    ResetTheScenarioState();
}
