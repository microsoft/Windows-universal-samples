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

#include "Scenario3_SeekerDiscover.g.h"

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// UI that provides a way to discover any Wi-Fi Direct service by name
        /// A real application would likely hard-code the service name to a well-known service rather than expose this in the UI
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.h/cpp and WiFiDirectServicesWrappers.h/cpp
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario3 sealed
        {
        public:
            Scenario3();

            void SessionConnected();

            void UpdateDiscoveryList(Windows::Foundation::Collections::IVector<DiscoveredDeviceWrapper^>^ devices);

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

            virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

        private:
            SDKTemplate::MainPage^ _rootPage;
            std::mutex _mutex;

            Windows::Devices::WiFiDirect::Services::WiFiDirectServiceProvisioningInfo^ _provisioningInfo;

            void Discover_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            // An application has two ways to connect over Wi-Fi Direct services
            // 1) First call GetProvisioningInfoAsync to begin connecting,
            //        a) if a PIN is required (group formation is required and selected config method is PinEntry or PinDisplay) then call ConnectAsync with the PIN
            //        b) else if no PIN is required then call ConnectAsync with no PIN
            // 2) Call ConnectAsync to connect without a PIN and prefer the Wi-Fi Direct Default config method, if possible

            void GetProvisioningInfo_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            void DoConnect_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };
    }
}
