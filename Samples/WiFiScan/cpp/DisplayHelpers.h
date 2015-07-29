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
#include "Objbase.h"

namespace SDKTemplate
{
    namespace WiFiScan
    {
        [Windows::UI::Xaml::Data::Bindable]
        public ref class WiFiNetworkDisplay sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
        {
        public:

            WiFiNetworkDisplay(Windows::Devices::WiFi::WiFiAvailableNetwork^ availableNetwork, Windows::Devices::WiFi::WiFiAdapter^ adapter);

            property Platform::String^ Ssid
            {
                Platform::String^ get() { return AvailableNetwork->Ssid; }
            }

            property Platform::String^ Bssid
            {
                Platform::String^ get() { return AvailableNetwork->Bssid; }
            }

            property Platform::String^ ChannelCenterFrequency
            {
                Platform::String^ get() {
                    return AvailableNetwork->ChannelCenterFrequencyInKilohertz + L"kHz";
                }
            }

            property Platform::String^ Rssi
            {
                Platform::String^ get() {
                    return AvailableNetwork->NetworkRssiInDecibelMilliwatts + L"dBm";
                }
            }

            property Platform::String^ SecuritySettings
            {

                Platform::String^ get()
                {
                    return L"Authentication: " + GetNetworkAuthenticationTypeAsString(AvailableNetwork->SecuritySettings->NetworkAuthenticationType) +
                        L"; Encryption: " + GetNetworkEncryptionTypeAsString(AvailableNetwork->SecuritySettings->NetworkEncryptionType);
                }
            }

            property Windows::Devices::WiFi::WiFiAvailableNetwork^ AvailableNetwork;
            property Platform::String^ ConnectivityLevel;
            property Windows::UI::Xaml::Media::Imaging::BitmapImage^ WiFiImage;
            virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

        protected:
            void OnPropertyChanged(Platform::String^ name);

        internal:
            static Platform::String^ GetConnectionStatusAsString(Windows::Devices::WiFi::WiFiConnectionStatus connectionStatus);
            static Platform::String^ GetConnectivityLevelAsString(Windows::Networking::Connectivity::NetworkConnectivityLevel conectivityLevel);
            static Concurrency::task<void> UpdateConnectivityLevels(Windows::Foundation::Collections::IObservableVector<WiFiNetworkDisplay^>^ networkCollection, UINT current);

        private:
            void UpdateWiFiImage();
            Concurrency::task<void> UpdateConnectivityLevel();
            Windows::Devices::WiFi::WiFiAdapter^ adapter;
            Platform::String^ GetNetworkAuthenticationTypeAsString(Windows::Networking::Connectivity::NetworkAuthenticationType  authenticationType);
            Platform::String^ GetNetworkEncryptionTypeAsString(Windows::Networking::Connectivity::NetworkEncryptionType encryptionType);
        };

    }
}