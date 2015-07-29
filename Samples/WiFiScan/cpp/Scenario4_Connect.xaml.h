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

#include "Scenario4_Connect.g.h"
#include "MainPage.xaml.h"
#include "DisplayHelpers.h"

namespace SDKTemplate
{
    namespace WiFiScan
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        [Windows::UI::Xaml::Data::Bindable]
        public ref class Scenario4_Connect sealed
        {
        public:
            Scenario4_Connect();

            property Windows::Foundation::Collections::IObservableVector<WiFiNetworkDisplay^>^ ResultCollection;

        protected:
            void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        private:
            void DisplayNetworkReport(Windows::Devices::WiFi::WiFiNetworkReport^ report);
            void ResultsListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
            void ConnectButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void ScanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void DisconnectButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void OnNetworkStatusChanged(Platform::Object^ sender);
            void UpdateConnectivityStatus();

            MainPage^ _rootPage;
            Platform::String^ _savedProfileName;
            Windows::Devices::WiFi::WiFiAdapter^ _firstAdapter;
            Windows::Foundation::EventRegistrationToken _registrationToken;
        };
    }
}