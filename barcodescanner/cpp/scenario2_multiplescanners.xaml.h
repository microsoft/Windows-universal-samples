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

#include "Scenario2_MultipleScanners.g.h"
#include "MainPage.xaml.h"

using namespace Windows::Devices::PointOfService;

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class Scenario2_MultipleScanners sealed
    {
    public:
        Scenario2_MultipleScanners();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        ClaimedBarcodeScanner^ claimedBarcodeScannerInstance1;
        ClaimedBarcodeScanner^ claimedBarcodeScannerInstance2;
        BarcodeScanner^ scannerInstance1;
        BarcodeScanner^ scannerInstance2;

        // tokens for instance1
        Windows::Foundation::EventRegistrationToken dataReceivedTokenInstance1;
        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedTokenInstance1;

        // tokens for instance2
        Windows::Foundation::EventRegistrationToken dataReceivedTokenInstance2;
        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedTokenInstance2;

        /// <summary>
        /// Enumerator for current active Instance.
        /// </summary>
        enum BarcodeScannerInstance
        {
            Instance1,
            Instance2
        };

        //utility functions for creating, enabling and disabling barcode scanner instances
        Concurrency::task<void> CreateDefaultScannerObject(BarcodeScannerInstance instance);
        Concurrency::task<void> ClaimScanner(BarcodeScannerInstance instance);
        Concurrency::task<void> EnableScanner(BarcodeScannerInstance instance);
        void ResetTheScenarioState();

        void DisableScanner(BarcodeScannerInstance instance);
        void DestroyScanner(BarcodeScannerInstance instance);

        //UI updates
        void ResetUI();
        void SetUI(BarcodeScannerInstance instance);

        //Event handlers for the Scanner Instance1
        void ButtonStartScanningInstance1_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ButtonEndScanningInstance1_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnDataReceivedInstance1(Windows::Devices::PointOfService::ClaimedBarcodeScanner ^sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs ^args);
        void OnReleaseDeviceRequestedInstance1(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner ^args);

        //Event handlers for Scanner Instance2
        void ButtonStartScanningInstance2_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ButtonEndScanningInstance2_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void OnDataReceivedInstance2(Windows::Devices::PointOfService::ClaimedBarcodeScanner ^sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs ^args);
        void OnReleaseDeviceRequestedInstance2(Platform::Object ^sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner ^args);

    };
}
