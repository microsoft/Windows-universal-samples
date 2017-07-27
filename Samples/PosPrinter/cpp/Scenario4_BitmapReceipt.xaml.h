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

#include "Scenario4_BitmapReceipt.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_BitmapReceipt sealed
    {
    public:
        Scenario4_BitmapReceipt();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        Windows::Devices::PointOfService::PosPrinter^ printer;
        Windows::Devices::PointOfService::ClaimedPosPrinter^ claimedPrinter;
        bool IsAnImportantTransaction = true;
        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedToken;

        Concurrency::task<void> FindReceiptPrinter();
        void ClaimedPrinter_ReleaseDeviceRequested(Windows::Devices::PointOfService::ClaimedPosPrinter^ claimedInstance, Windows::Devices::PointOfService::PosPrinterReleaseDeviceRequestedEventArgs^ args);

        void FindClaimEnable_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void IsImportantTransaction_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void PrintBitmap_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void EndScenario_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ResetTheScenarioState();
//        void PrintLineFeedAndCutPaper(Windows::Devices::PointOfService::ReceiptPrintJob^ job, Platform::String^ receipt);

        Concurrency::task<void> ClaimPrinter();
        Concurrency::task<void> EnableAsync();
        Concurrency::task<Windows::Graphics::Imaging::BitmapFrame^> LoadLogoBitmapAsync();
        void PrintBitmap();

    };
}
