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

#include "Scenario4_SeekerSession.g.h"

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// UI that shows connected Wi-Fi Direct service sessions and allows a user to choose one to close or interact with
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.h/cpp and WiFiDirectServicesWrappers.h/cpp
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario4 sealed
        {
        public:
            Scenario4();

            void AddSession(SessionWrapper^ session);

            void RemoveSession(unsigned int index);

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

            virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

        private:
            SDKTemplate::MainPage^ _rootPage;
            std::mutex _mutex;

            void SelectSession_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            void CloseSession_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };
    }
}
