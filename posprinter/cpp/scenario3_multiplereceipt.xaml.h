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

#pragma once

#include "Scenario3_MultipleReceipt.g.h"
#include "MainPage.xaml.h"

using namespace Windows::Devices::PointOfService;
using namespace Windows::UI::Xaml;
using namespace Concurrency;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_MultipleReceipt sealed
    {
    public:
        Scenario3_MultipleReceipt();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;

        PosPrinter^ printerInstance1 = nullptr;
        PosPrinter^ printerInstance2 = nullptr;
        Windows::Devices::PointOfService::ClaimedPosPrinter^ claimedPrinter1 = nullptr;
        Windows::Devices::PointOfService::ClaimedPosPrinter^ claimedPrinter2 = nullptr;
        bool IsAnImportantTransactionInstance1;
        bool IsAnImportantTransactionInstance2;

        void Release1_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void Release2_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void Claim1_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void Claim2_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedToken1;
        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedToken2;

        task<void> FindReceiptPrinter();
        void ClaimedPrinter1_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedPrinter, PosPrinterReleaseDeviceRequestedEventArgs ^args);
        void ClaimedPrinter2_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedPrinter, PosPrinterReleaseDeviceRequestedEventArgs ^args);

        void FindReceiptPrinter_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void PrintLine1_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void PrintLine2_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void EndScenario_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void chkInstance1_Checked(Platform::Object^ sender, RoutedEventArgs^ e);
        void chkInstance2_Checked(Platform::Object^ sender, RoutedEventArgs^ e);
        void chkInstance1_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e);
        void chkInstance2_Unchecked(Platform::Object^ sender, RoutedEventArgs^ e);

        task<void> ClaimAndEnablePrinter1();
        task<void> ClaimAndEnablePrinter2();

        void ResetTheScenarioState();

    };
}
