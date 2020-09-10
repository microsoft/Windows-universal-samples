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

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_MultipleScanners : Scenario2_MultipleScannersT<Scenario2_MultipleScanners>
    {
        Scenario2_MultipleScanners();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget ButtonStartScanningInstance1_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget claimedBarcodeScannerInstance1_ReleaseDeviceRequested(Windows::Foundation::IInspectable const& sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner const&);
        fire_and_forget ButtonStartScanningInstance2_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget claimedBarcodeScannerInstance2_ReleaseDeviceRequested(Windows::Foundation::IInspectable const& sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner const&);

        void ButtonEndScanningInstance1_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void ButtonEndScanningInstance2_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ SDKTemplate::MainPage::Current() };

        Windows::Devices::PointOfService::BarcodeScanner m_barcodeScannerInstance1{ nullptr };
        Windows::Devices::PointOfService::BarcodeScanner m_barcodeScannerInstance2{ nullptr };
        Windows::Devices::PointOfService::ClaimedBarcodeScanner m_claimedBarcodeScannerInstance1{ nullptr };
        Windows::Devices::PointOfService::ClaimedBarcodeScanner m_claimedBarcodeScannerInstance2{ nullptr };
        event_token m_dev1ReleaseRequestedToken;
        event_token m_dev1DataReceivedToken;
        event_token m_dev2ReleaseRequestedToken;
        event_token m_dev2DataReceivedToken;

        /// <summary>
        /// Enumerator for current active Instance.
        /// </summary>
        enum class BarcodeScannerInstance
        {
            Instance1,
            Instance2
        };

        Windows::Foundation::IAsyncOperation<bool> CreateDefaultScannerObjectAsync(BarcodeScannerInstance instance);
        Windows::Foundation::IAsyncOperation<bool> ClaimBarcodeScannerAsync(BarcodeScannerInstance instance);
        Windows::Foundation::IAsyncOperation<bool> EnableBarcodeScannerAsync(BarcodeScannerInstance instance);

        void ResetTheScenarioState();
        void ResetUI();

        void SetUI(BarcodeScannerInstance instance);

        fire_and_forget claimedBarcodeScannerInstance1_DataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner const&, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs args);
        fire_and_forget claimedBarcodeScannerInstance2_DataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner const&, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs args);

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_MultipleScanners : Scenario2_MultipleScannersT<Scenario2_MultipleScanners, implementation::Scenario2_MultipleScanners>
    {
    };
}
