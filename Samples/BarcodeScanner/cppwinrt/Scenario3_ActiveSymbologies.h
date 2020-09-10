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

#include "Scenario3_ActiveSymbologies.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_ActiveSymbologies : Scenario3_ActiveSymbologiesT<Scenario3_ActiveSymbologies>
    {
        Scenario3_ActiveSymbologies();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        fire_and_forget ScenarioStartScanButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget SetActiveSymbologies_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void ScenarioEndScanButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Devices::PointOfService::BarcodeScanner m_scanner{ nullptr };
        Windows::Devices::PointOfService::ClaimedBarcodeScanner m_claimedScanner{ nullptr };

        Windows::Foundation::Collections::IObservableVector<SymbologyListEntry> m_listOfSymbologies{ nullptr };

        event_token m_scannerReleaseRequestedToken;
        event_token m_scannerDataReceivedToken;

        void claimedScanner_ReleaseDeviceRequested(Windows::Foundation::IInspectable const&, Windows::Devices::PointOfService::ClaimedBarcodeScanner const& e);
        fire_and_forget claimedScanner_DataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner const&, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs args);

        void ResetTheScenarioState();
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_ActiveSymbologies : Scenario3_ActiveSymbologiesT<Scenario3_ActiveSymbologies, implementation::Scenario3_ActiveSymbologies>
    {
    };
}
