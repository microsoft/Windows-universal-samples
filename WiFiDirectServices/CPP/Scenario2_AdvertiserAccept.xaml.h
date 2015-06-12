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

#include "Scenario2_AdvertiserAccept.g.h"

namespace SDKTemplate
{
    namespace WiFiDirectServices
    {
        /// <summary>
        /// Helper class to display session requests in list with extra data
        /// </summary>
        [Windows::UI::Xaml::Data::Bindable]
        public ref class SessionRequestWrapper sealed
        {
        public:
            SessionRequestWrapper(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^ request,
                AdvertisementWrapper^ advertisement
                );

            property Platform::String^ Id;
            property Platform::String^ ProvisioningInfo;
            property Platform::String^ Pin;
            property Windows::Storage::Streams::IBuffer^ SessionInfo;

            property AdvertisementWrapper^ Advertisement;
        };

        /// <summary>
        /// UI exposing an interface to accept/decline incoming connection requests as an advertiser
        /// and to interact with connected sessions
        ///
        /// Most of the actual logic for Wi-Fi Direct Services is in WiFiDirectServicesManager.h/cpp and WiFiDirectServicesWrappers.h/cpp
        /// </summary>
        [Windows::Foundation::Metadata::WebHostHidden]
        public ref class Scenario2 sealed
        {
        public:
            Scenario2();

            void AddSessionRequest(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^ request,
                AdvertisementWrapper^ advertiser
                );

            void RemoveSessionRequest(
                Windows::Devices::WiFiDirect::Services::WiFiDirectServiceSessionRequest^ request,
                AdvertisementWrapper^ advertiser
                );

            void AddAdvertiser(AdvertisementWrapper^ advertiser);

            void RemoveAdvertiser(unsigned int index);

            void AddSession(SessionWrapper^ session);

            void RemoveSession(unsigned int index);

            void UpdateAdvertiserStatus(AdvertisementWrapper^ advertiser);

        protected:
            virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

            virtual void OnNavigatingFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs^ e) override;

        private:
            SDKTemplate::MainPage^ _rootPage;
            std::mutex _mutex;

            void Accept_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            
            void Decline_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
            
            void CloseSession_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            void SelectSession_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

            void StopAdvertisement_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        };
    }
}
