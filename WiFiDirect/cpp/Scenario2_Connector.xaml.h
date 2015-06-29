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

#include "Scenario2_Connector.g.h"
#include "MainPage.xaml.h"
#include "SocketReaderWriter.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Connector sealed
    {
    public:
        Scenario2_Connector();

        property Windows::Foundation::Collections::IObservableVector<DiscoveredDevice^>^ _discoveredDevices;
        property Windows::Foundation::Collections::IObservableVector<ConnectedDevice^>^ _connectedDevices;

    private:
        void btnWatcher_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnIe_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnFromId_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnClose_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void btnSendMessage_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        MainPage^ rootPage;
        Windows::Devices::Enumeration::DeviceWatcher^ _deviceWatcher;
        std::mutex _discoveryMutex;
        bool _fWatcherStarted;

        void OnDeviceAdded(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Windows::Devices::Enumeration::DeviceInformation^ deviceInfo);
        void OnDeviceRemoved(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Windows::Devices::Enumeration::DeviceInformationUpdate^ deviceInfoUpdate);
        void OnDeviceUpdated(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Windows::Devices::Enumeration::DeviceInformationUpdate^ deviceInfoUpdate);
        void OnEnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Object^ o);
        void OnStopped(Windows::Devices::Enumeration::DeviceWatcher^ deviceWatcher, Object^ o);

        void OnConnectionStatusChanged(Windows::Devices::WiFiDirect::WiFiDirectDevice^ sender, Object^ arg);

        Windows::Foundation::EventRegistrationToken _deviceAddedToken;
        Windows::Foundation::EventRegistrationToken _deviceRemovedToken;
        Windows::Foundation::EventRegistrationToken _deviceUpdatedToken;
        Windows::Foundation::EventRegistrationToken _deviceWatcherEnumerationCompletedToken;
        Windows::Foundation::EventRegistrationToken _deviceWatcherStoppedToken;
    };
}