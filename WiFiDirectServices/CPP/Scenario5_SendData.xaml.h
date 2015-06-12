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

#include "Scenario5_SendData.g.h"

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// UI that exposes a way to open TCP/UDP ports and connect sockets between two devices connected over Wi-Fi Direct Services
        /// This sample just demonstrates setting up the connection and sending strings
        /// A real application would likely open pre-determined ports for application communication
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.h/cpp and WiFiDirectServicesWrappers.h/cpp
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario5 sealed
        {
        public:
            Scenario5();

            void AddSocket(SocketWrapper^ socket);

            void RemoveSocket(unsigned int index);

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

            virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

        private:
            SDKTemplate::MainPage^ _rootPage;
            std::mutex _mutex;
            SessionWrapper^ _session = nullptr;

            void AddPort_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            void DoSend_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };
    }
}
