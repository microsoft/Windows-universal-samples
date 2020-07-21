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

#include "Scenario1_Advertiser.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Advertiser : Scenario1_AdvertiserT<Scenario1_Advertiser>
    {
        Scenario1_Advertiser();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        auto ConnectedDevices() { return connectedDevices; }

        void btnStartAdvertisement_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnAddIe_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnStopAdvertisement_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget btnSendMessage_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnCloseDevice_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        
    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisher publisher{ nullptr };
        Windows::Devices::WiFiDirect::WiFiDirectConnectionListener listener{ nullptr };

        event_token statusChangedToken{};
        event_token connectionRequestedToken{};
        event_token connectionStatusChangedToken{};

        Windows::Foundation::Collections::IObservableVector<SDKTemplate::ConnectedDevice> connectedDevices{ single_threaded_observable_vector<SDKTemplate::ConnectedDevice>() };
        std::vector<Windows::Devices::WiFiDirect::WiFiDirectInformationElement> informationElements;
        slim_mutex pendingConnectionsMutex;
        std::map<Windows::Networking::Sockets::StreamSocketListener, Windows::Devices::WiFiDirect::WiFiDirectDevice> pendingConnections;

        void StopAdvertisement();
        Windows::Foundation::IAsyncOperation<bool> HandleConnectionRequestAsync(Windows::Devices::WiFiDirect::WiFiDirectConnectionRequest connectionRequest);
        Windows::Foundation::IAsyncOperation<bool> IsAepPairedAsync(hstring deviceId);

        fire_and_forget OnStatusChanged(Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisher sender, Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisherStatusChangedEventArgs e);
        fire_and_forget OnConnectionRequested(Windows::Devices::WiFiDirect::WiFiDirectConnectionListener const&, Windows::Devices::WiFiDirect::WiFiDirectConnectionRequestedEventArgs const& e);
        void OnConnectionStatusChanged(Windows::Devices::WiFiDirect::WiFiDirectDevice const& sender, Windows::Foundation::IInspectable const&);
        fire_and_forget OnSocketConnectionReceived(Windows::Networking::Sockets::StreamSocketListener sender, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Advertiser : Scenario1_AdvertiserT<Scenario1_Advertiser, implementation::Scenario1_Advertiser>
    {
    };
}
