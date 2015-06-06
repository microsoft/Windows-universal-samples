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

#include "Scenario2_ErrorHandling.g.h"
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
    public ref class Scenario2_ErrorHandling sealed
    {
    public:
        Scenario2_ErrorHandling();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        PosPrinter^ printer;
        ClaimedPosPrinter^ claimedPrinter;
        bool IsAnImportantTransaction = true;
        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedToken;

        task<void> FindReceiptPrinter();
        void ClaimedPrinter_ReleaseDeviceRequested(ClaimedPosPrinter ^claimedInstance, PosPrinterReleaseDeviceRequestedEventArgs ^args);

        void FindClaimEnable_Click(Platform::Object^ sender, RoutedEventArgs^ e);
        void PrintLine_Click(Platform::Object^ sender, RoutedEventArgs e);
        void EndScenario_Click(Platform::Object^ sender, RoutedEventArgs e);

        void ResetTheScenarioState();

        task<void> ClaimPrinter();
        task<void> EnableAsync();
    };
}
