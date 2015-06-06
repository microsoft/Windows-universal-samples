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

#include "Scenario1_CachedNetworks.g.h"
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
        public ref class Scenario1_CachedNetworks sealed
        {
        public:
            Scenario1_CachedNetworks();

            property Windows::Foundation::Collections::IObservableVector<WiFiNetworkDisplay^>^ ResultCollection;

        protected:
            void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

        private:
            void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            void DisplayNetworkReport(Windows::Devices::WiFi::WiFiNetworkReport^ report, Windows::Devices::WiFi::WiFiAdapter^ adapter);

            MainPage^ _rootPage;
            Windows::Foundation::Collections::IVectorView<Windows::Devices::WiFi::WiFiAdapter^>^ _wiFiAdapters;
        };
    }
}