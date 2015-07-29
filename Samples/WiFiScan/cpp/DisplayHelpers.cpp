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

#include "pch.h"
#include <ppltasks.h>
#include "DisplayHelpers.h"
#include "MainPage.xaml.h"

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Devices::WiFi;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Connectivity;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Media::Imaging;

using namespace SDKTemplate::WiFiScan;
namespace SDKTemplate {
    namespace WiFiScan {

        // WiFiNetworkDisplay

        WiFiNetworkDisplay::WiFiNetworkDisplay(WiFiAvailableNetwork^ availableNetwork, WiFiAdapter^ adapter)
        {
            AvailableNetwork = availableNetwork;
            this->adapter = adapter;
            UpdateWiFiImage();
        }

        void WiFiNetworkDisplay::UpdateWiFiImage()
        {
            Platform::String^ imageFileNamePrefix;
            if (AvailableNetwork->SecuritySettings->NetworkAuthenticationType == NetworkAuthenticationType::Open80211)
            {
                imageFileNamePrefix = L"open";
            }
            else
            {
                imageFileNamePrefix = L"secure";
            }

            Platform::String^ imageFileName = L"ms-appx:/Assets/" + imageFileNamePrefix + "_" + AvailableNetwork->SignalBars + "bar.png";

            WiFiImage = ref new BitmapImage(ref new Uri(imageFileName));

            OnPropertyChanged("WiFiImage");
        }

        task<void> WiFiNetworkDisplay::UpdateConnectivityLevels(IObservableVector<WiFiNetworkDisplay^>^ networkCollection, UINT current)
        {
            if (current < networkCollection->Size)
            {
                try {
                    return networkCollection->GetAt(current)->UpdateConnectivityLevel().then([networkCollection, current]()
                    {
                        // Wait until the first task is done, then recursively call the next one
                        // This way we won't call GetConnectedProfileAsync() too many times 
                        // which may exhaust the available wlan handles
                        return UpdateConnectivityLevels(networkCollection, current + 1);
                    });
                }
                catch (...)
                {
                    // The network collection may have changed. If that happens,
                    // return an empty task to terminate the recusrsion
                    return create_task([]()
                    {
                    });
                }
            }
            else
            {
                return create_task([]()
                {
                });
            }
        }

        task<void> WiFiNetworkDisplay::UpdateConnectivityLevel()
        {
            auto connectionProfileOperation = adapter->NetworkAdapter->GetConnectedProfileAsync();
            auto connectionProfileTask = create_task(connectionProfileOperation);

            return connectionProfileTask.then([this](ConnectionProfile^ connectedProfile)
            {
                Platform::String^ connectedSsid = nullptr;

                if (connectedProfile != nullptr &&
                    connectedProfile->IsWlanConnectionProfile &&
                    connectedProfile->WlanConnectionProfileDetails != nullptr)
                {
                    connectedSsid = connectedProfile->WlanConnectionProfileDetails->GetConnectedSsid();
                }

                if (connectedSsid != nullptr &&
                    AvailableNetwork->Ssid != nullptr &&
                    connectedSsid->Equals(AvailableNetwork->Ssid))
                {
                    ConnectivityLevel = WiFiNetworkDisplay::GetConnectivityLevelAsString(connectedProfile->GetNetworkConnectivityLevel());
                }
                else
                {
                    ConnectivityLevel = L"Not Connected";
                }

                OnPropertyChanged("ConnectivityLevel");
            });
        }

        Platform::String^ WiFiNetworkDisplay::GetNetworkAuthenticationTypeAsString(NetworkAuthenticationType authenticationType)
        {
            switch (authenticationType)
            {
            case NetworkAuthenticationType::None:
                return L"None";
            case NetworkAuthenticationType::Unknown:
                return L"Unknown";
            case NetworkAuthenticationType::Open80211:
                return L"Open80211";
            case NetworkAuthenticationType::SharedKey80211:
                return L"SharedKey80211";
            case NetworkAuthenticationType::Wpa:
                return L"Wpa";
            case NetworkAuthenticationType::WpaPsk:
                return L"WpaPsk";
            case NetworkAuthenticationType::WpaNone:
                return L"WpaNone";
            case NetworkAuthenticationType::Rsna:
                return L"Rsna";
            case NetworkAuthenticationType::RsnaPsk:
                return L"RsnaPsk";
            default:
                return L"Error getting Network Authentication Type";
            }
        }

        Platform::String^ WiFiNetworkDisplay::GetNetworkEncryptionTypeAsString(NetworkEncryptionType encryptionType)
        {
            switch (encryptionType)
            {
            case NetworkEncryptionType::None:
                return L"None";
            case NetworkEncryptionType::Unknown:
                return L"Unknown";
            case NetworkEncryptionType::Wep:
                return L"Wep";
            case NetworkEncryptionType::Wep40:
                return L"Wep40";
            case NetworkEncryptionType::Wep104:
                return L"Wep104";
            case NetworkEncryptionType::Tkip:
                return L"Tkip";
            case NetworkEncryptionType::Ccmp:
                return L"Ccmp";
            case NetworkEncryptionType::WpaUseGroup:
                return L"WpaUseGroup";
            case NetworkEncryptionType::RsnUseGroup:
                return L"RsnUseGroup";
            default:
                return L"Error getting Network Encryption Type";
            }
        }

        Platform::String^ WiFiNetworkDisplay::GetConnectionStatusAsString(WiFiConnectionStatus connectionStatus)
        {
            switch (connectionStatus)
            {
            case WiFiConnectionStatus::UnspecifiedFailure:
                return L"UnspecifiedFailure";
            case WiFiConnectionStatus::Success:
                return L"Success";
            case WiFiConnectionStatus::AccessRevoked:
                return L"AccessRevoked";
            case WiFiConnectionStatus::InvalidCredential:
                return L"InvalidCredential";
            case WiFiConnectionStatus::NetworkNotAvailable:
                return L"NetworkNotAvailable";
            case WiFiConnectionStatus::Timeout:
                return L"Timeout";
            case WiFiConnectionStatus::UnsupportedAuthenticationProtocol:
                return L"UnsupportedAuthenticationProtocol";
            default:
                return L"Error getting Connection Status";
            }
        }

        Platform::String^ WiFiNetworkDisplay::GetConnectivityLevelAsString(NetworkConnectivityLevel conectivityLevel)
        {
            switch (conectivityLevel)
            {
            case NetworkConnectivityLevel::ConstrainedInternetAccess:
                return L"ConstrainedInternetAccess";
            case NetworkConnectivityLevel::InternetAccess:
                return L"InternetAccess";
            case NetworkConnectivityLevel::LocalAccess:
                return  L"LocalAccess";
            case NetworkConnectivityLevel::None:
                return  L"Not Connected";
            default:
                return L"Not Connected";
            }
        }

        void WiFiNetworkDisplay::OnPropertyChanged(String^ name)
        {
            MainPage::Current->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([=]()
            {
                PropertyChanged(this, ref new PropertyChangedEventArgs(name));
            }));
        }
    }
}