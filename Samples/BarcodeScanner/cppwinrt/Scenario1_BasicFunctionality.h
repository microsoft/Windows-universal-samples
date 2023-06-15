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

#include "Scenario1_BasicFunctionality.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_BasicFunctionality : Scenario1_BasicFunctionalityT<Scenario1_BasicFunctionality>
    {
        Scenario1_BasicFunctionality();

        fire_and_forget ScenarioStartScanButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ScenarioEndScanButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ScenarioStartTriggerButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget ScenarioStopTriggerButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void claimedScanner_ReleaseDeviceRequested(Windows::Foundation::IInspectable const&, Windows::Devices::PointOfService::ClaimedBarcodeScanner const& e);
        fire_and_forget claimedScanner_DataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner const& sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs args);

    private:

        void ResetTheScenarioState();

        SDKTemplate::MainPage m_rootPage{ MainPage::Current() };
        Windows::Devices::PointOfService::BarcodeScanner m_scanner{ nullptr };
        Windows::Devices::PointOfService::ClaimedBarcodeScanner m_claimedScanner{ nullptr };
        event_token m_claimedScannerDataReceivedToken;
        event_token m_claimedScannerReleaseDeviceRequestedToken;
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_BasicFunctionality : Scenario1_BasicFunctionalityT<Scenario1_BasicFunctionality, implementation::Scenario1_BasicFunctionality>
    {
    };
}
