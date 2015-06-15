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
#include "MainPage.xaml.h"
#include "SocketReaderWriter.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Advertiser sealed
    {
    public:
        Scenario1_Advertiser();

        property Windows::Foundation::Collections::IObservableVector<ConnectedDevice^>^ _connectedDevices;

    private:
        void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Click_2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Button_Click_3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnStartAdvertisement_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnStopAdvertisement_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnAddIe_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnSendMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void OnConnectionRequested(Windows::Devices::WiFiDirect::WiFiDirectConnectionListener^ sender, Windows::Devices::WiFiDirect::WiFiDirectConnectionRequestedEventArgs^ connectionEventArgs);
        void OnStatusChanged(Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisher^ sender, Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisherStatusChangedEventArgs^ statusEventArgs);
        void OnSocketConnectionReceived(Windows::Networking::Sockets::StreamSocketListener^ sender, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ connectionReceivedEventArgs);
        void SDKTemplate::Scenario1_Advertiser::OnConnectionStatusChanged(Windows::Devices::WiFiDirect::WiFiDirectDevice^ sender, Platform::Object^ arg);

        MainPage^ rootPage;

        Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisher^ _publisher;
        Windows::Devices::WiFiDirect::WiFiDirectConnectionListener^ _listener;
        Windows::Networking::Sockets::StreamSocketListener^ _listenerSocket;

        Windows::Foundation::EventRegistrationToken _connectionRequestedToken;
        Windows::Foundation::EventRegistrationToken _statusChangedToken;
    };
}