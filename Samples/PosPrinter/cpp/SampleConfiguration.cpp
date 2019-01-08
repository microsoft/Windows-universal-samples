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
using namespace Windows::UI::Core;

Platform::Array<Scenario>^ MainPage::scenariosInner = ref new Platform::Array<Scenario>
{
    { "Find, claim, and enable printer", "SDKTemplate.Scenario1_FindClaimEnable" },
    { "Print receipt", "SDKTemplate.Scenario2_PrintReceipt" },
    { "Multiple claims", "SDKTemplate.Scenario3_MultipleClaims" },
};

void MainPage::SubscribeToReleaseDeviceRequested()
{
    releaseDeviceRequestedToken = ClaimedPrinter->ReleaseDeviceRequested += ref new TypedEventHandler<ClaimedPosPrinter^, PosPrinterReleaseDeviceRequestedEventArgs^>(this, &MainPage::ClaimedPrinter_ReleaseDeviceRequested);
}

void MainPage::ReleaseClaimedPrinter()
{
    if (ClaimedPrinter != nullptr)
    {
        ClaimedPrinter->ReleaseDeviceRequested -= releaseDeviceRequestedToken;
        delete ClaimedPrinter;
        ClaimedPrinter = nullptr;
        if (StateChanged) StateChanged();
    }
}

void MainPage::ReleaseAllPrinters()
{
    ReleaseClaimedPrinter();

    if (Printer != nullptr)
    {
        delete Printer;
        Printer = nullptr;
        if (StateChanged) StateChanged();
    }
}

/// <summary>
/// If the "Retain device" checkbox is checked, we retain the device.
/// Otherwise, we allow the other claimant to claim the device.
/// </summary>
void MainPage::ClaimedPrinter_ReleaseDeviceRequested(ClaimedPosPrinter^ sender, PosPrinterReleaseDeviceRequestedEventArgs^ args)
{
    if (IsAnImportantTransaction)
    {
        create_task(sender->RetainDeviceAsync());
    }
    else
    {
        create_task(Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this]()
        {
            NotifyUser("Lost printer claim.", NotifyType::ErrorMessage);
            ReleaseClaimedPrinter();
        })));
    }
}
