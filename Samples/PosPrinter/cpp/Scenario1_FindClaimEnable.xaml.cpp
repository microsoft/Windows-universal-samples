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
#include "Scenario1_FindClaimEnable.xaml.h"

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Devices::PointOfService;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_FindClaimEnable::Scenario1_FindClaimEnable()
{
    InitializeComponent();
}

void Scenario1_FindClaimEnable::OnNavigatedTo(NavigationEventArgs^ e)
{
    RetainDeviceCheckBox->IsChecked = rootPage->IsAnImportantTransaction;
    rootPage->StateChanged = std::function<void(void)>([this]() { UpdateButtons(); });
    UpdateButtons();
}

void Scenario1_FindClaimEnable::OnNavigatedFrom(NavigationEventArgs^ e)
{
    rootPage->StateChanged = nullptr;
}

void Scenario1_FindClaimEnable::UpdateButtons()
{
    PrinterNameRun->Text = (rootPage->Printer == nullptr) ? "None" : rootPage->deviceInfo->Name + " (" + rootPage->Printer->DeviceId + ")";
    if (isBusy)
    {
        FindButton->IsEnabled = false;
        ClaimAndEnableButton->IsEnabled = false;
        ReleaseClaimedPrinterButton->IsEnabled = false;
        ReleaseAllPrintersButton->IsEnabled = false;
    }
    else if (rootPage->Printer == nullptr)
    {
        FindButton->IsEnabled = true;
        ClaimAndEnableButton->IsEnabled = false;
        ReleaseClaimedPrinterButton->IsEnabled = false;
        ReleaseAllPrintersButton->IsEnabled = false;
    }
    else
    {
        FindButton->IsEnabled = false;
        ReleaseAllPrintersButton->IsEnabled = true;
        if (rootPage->ClaimedPrinter == nullptr)
        {
            ClaimAndEnableButton->IsEnabled = true;
            ReleaseClaimedPrinterButton->IsEnabled = false;
        }
        else
        {
            ClaimAndEnableButton->IsEnabled = false;
            ReleaseClaimedPrinterButton->IsEnabled = true;
        }
    }
}

void Scenario1_FindClaimEnable::FindPrinter_Click()
{
    isBusy = true;
    UpdateButtons();

    rootPage->NotifyUser("", NotifyType::StatusMessage);

    rootPage->ReleaseAllPrinters();

    // Select a PosPrinter device using the Device Picker.
    DevicePicker^ devicePicker = ref new DevicePicker();
    devicePicker->Filter->SupportedDeviceSelectors->Append(PosPrinter::GetDeviceSelector());

    // Anchor the picker on the Find button.
    GeneralTransform^ ge = FindButton->TransformToVisual(safe_cast<UIElement^>(Window::Current->Content));
    Rect rect = ge->TransformBounds(Rect(0, 0, FindButton->ActualWidth, FindButton->ActualHeight));

    create_task(devicePicker->PickSingleDeviceAsync(rect)).then([this](DeviceInformation^ deviceInfo)
    {
        rootPage->deviceInfo = deviceInfo;
        if (deviceInfo != nullptr)
        {
            return create_task(PosPrinter::FromIdAsync(deviceInfo->Id));
        }
        else
        {
            return task_from_result<PosPrinter^>(nullptr);
        }
    }).then([this](PosPrinter^ printer)
    {
        if (printer && printer->Capabilities->Receipt->IsPrinterPresent)
        {
            rootPage->Printer = printer;
            rootPage->NotifyUser("Found receipt printer.", NotifyType::StatusMessage);
        }
        else
        {
            // Get rid of the printer we can't use.
            delete printer;
            rootPage->NotifyUser("Please select a device whose printer is present.", NotifyType::ErrorMessage);
        }

        isBusy = false;
        UpdateButtons();
    });
}

void Scenario1_FindClaimEnable::ClaimAndEnable_Click()
{
    isBusy = true;
    UpdateButtons();
    create_task(rootPage->Printer->ClaimPrinterAsync()).then([this](ClaimedPosPrinter^ claimedPrinter)
    {
        rootPage->ClaimedPrinter = claimedPrinter;

        if (claimedPrinter == nullptr)
        {
            rootPage->NotifyUser("Unable to claim printer", NotifyType::ErrorMessage);
            return task_from_result();
        }
        else
        {
            rootPage->NotifyUser("Claimed printer", NotifyType::StatusMessage);

            // Register for the ReleaseDeviceRequested event so we know when somebody
            // wants to claim the printer away from us.
            rootPage->SubscribeToReleaseDeviceRequested();

            return create_task(claimedPrinter->EnableAsync()).then([this](bool enabled)
            {
                if (enabled)
                {
                    rootPage->NotifyUser("Enabled printer.", NotifyType::StatusMessage);
                }
                else
                {
                    rootPage->NotifyUser("Could not enable printer", NotifyType::ErrorMessage);
                    rootPage->ReleaseClaimedPrinter();
                }
            });
        }
    }).then([this]()
    {
        isBusy = false;
        UpdateButtons();
    });
}

void Scenario1_FindClaimEnable::IsImportantTransaction_Click()
{
    rootPage->IsAnImportantTransaction = RetainDeviceCheckBox->IsChecked->Value;
}

void Scenario1_FindClaimEnable::ReleaseClaim_Click()
{
    rootPage->ReleaseClaimedPrinter();
}

void Scenario1_FindClaimEnable::ReleaseAll_Click()
{
    rootPage->ReleaseAllPrinters();
}
