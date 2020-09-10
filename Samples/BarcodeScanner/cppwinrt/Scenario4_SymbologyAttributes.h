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

#include "Scenario4_SymbologyAttributes.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_SymbologyAttributes : Scenario4_SymbologyAttributesT<Scenario4_SymbologyAttributes>
    {
        Scenario4_SymbologyAttributes();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        fire_and_forget ScenarioStartScanButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void ScenarioEndScanButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget SymbologySelection_Changed(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        fire_and_forget SetSymbologyAttributes_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Devices::PointOfService::BarcodeScanner m_scanner{ nullptr };
        Windows::Devices::PointOfService::ClaimedBarcodeScanner m_claimedScanner{ nullptr };

        Windows::Foundation::Collections::IObservableVector<SymbologyListEntry> m_listOfSymbologies{ nullptr };
        Windows::Devices::PointOfService::BarcodeSymbologyAttributes m_symbologyAttributes{ nullptr };
        Windows::Devices::PointOfService::ClaimedBarcodeScanner::ReleaseDeviceRequested_revoker m_scannerReleaseRequestedToken;
        Windows::Devices::PointOfService::ClaimedBarcodeScanner::DataReceived_revoker m_scannerDataReceivedToken;

        void claimedScanner_ReleaseDeviceRequested(Windows::Foundation::IInspectable const&, Windows::Devices::PointOfService::ClaimedBarcodeScanner const& args);
        fire_and_forget claimedScanner_DataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner const&, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs args);

        void ResetTheScenarioState();

    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_SymbologyAttributes : Scenario4_SymbologyAttributesT<Scenario4_SymbologyAttributes, implementation::Scenario4_SymbologyAttributes>
    {
    };
}