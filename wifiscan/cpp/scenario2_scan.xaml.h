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

#include "Scenario2_Scan.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    namespace WiFiScan
    {
        /// <summary>
        /// An empty page that can be used on its own or navigated to within a Frame.
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario2_Scan sealed
        {
        public:
            Scenario2_Scan();
            void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
        private:
            Windows::Devices::WiFi::WiFiAdapter^ _firstAdapter;
            void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void DisplayNetworkReport(Windows::Devices::WiFi::WiFiNetworkReport^ report);
        };
    }
}