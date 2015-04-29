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

#include "Scenario3_RegisterForUpdates.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace WiFiScan
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario3_RegisterForUpdates sealed
        {
        public:
            Scenario3_RegisterForUpdates();
            void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        private:
            Windows::Devices::WiFi::WiFiAdapter^ _firstAdapter;
            MainPage^ _rootPage;
            Windows::Foundation::EventRegistrationToken _registrationToken;
            void Button_Click_Register(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void Button_Click_Unregister(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void DisplayNetworkReport(Windows::Devices::WiFi::WiFiNetworkReport^ report);
            void OnAvailableNetworksChanged(Windows::Devices::WiFi::WiFiAdapter ^sender, Platform::Object ^args);
        };
    }
}