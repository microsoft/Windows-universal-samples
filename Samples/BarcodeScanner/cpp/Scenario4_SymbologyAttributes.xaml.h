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
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario4_SymbologyAttributes sealed
    {
    public:
        Scenario4_SymbologyAttributes();

    protected:
        virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

    private:
        MainPage^ rootPage;
        Windows::Devices::PointOfService::BarcodeScanner^ scanner;
        Windows::Devices::PointOfService::ClaimedBarcodeScanner^ claimedScanner;
        Windows::Foundation::EventRegistrationToken dataReceivedToken;
        Windows::Foundation::EventRegistrationToken releaseDeviceRequestedToken;
        Windows::Foundation::Collections::IObservableVector<SymbologyListEntry^>^ listOfSymbologies;
        Windows::Devices::PointOfService::BarcodeSymbologyAttributes^ symbologyAttributes;

        void ScenarioStartScanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ScenarioEndScanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SymbologySelection_Changed(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void SetSymbologyAttributes_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void ResetTheScenarioState();

        void OnDataReceived(Windows::Devices::PointOfService::ClaimedBarcodeScanner^ sender, Windows::Devices::PointOfService::BarcodeScannerDataReceivedEventArgs^ args);
        void OnReleaseDeviceRequested(Platform::Object^ sender, Windows::Devices::PointOfService::ClaimedBarcodeScanner^ args);
    };
}
