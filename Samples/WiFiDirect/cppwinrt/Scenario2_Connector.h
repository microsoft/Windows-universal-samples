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

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Connector : Scenario2_ConnectorT<Scenario2_Connector>
    {
        Scenario2_Connector();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        auto DiscoveredDevices() { return discoveredDevices; }
        auto ConnectedDevices() { return connectedDevices; }

        void btnWatcher_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnIe_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget btnFromId_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget btnSendMessage_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void btnClose_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget btnUnpair_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Devices::Enumeration::DeviceWatcher deviceWatcher{ nullptr };
        Windows::Devices::WiFiDirect::WiFiDirectAdvertisementPublisher publisher;

        Windows::Foundation::Collections::IObservableVector<SDKTemplate::DiscoveredDevice> discoveredDevices{ single_threaded_observable_vector<SDKTemplate::DiscoveredDevice>() };
        Windows::Foundation::Collections::IObservableVector<SDKTemplate::ConnectedDevice> connectedDevices{ single_threaded_observable_vector<SDKTemplate::ConnectedDevice>() };

        event_token watcherAddedToken;
        event_token watcherRemovedToken;
        event_token watcherUpdatedToken;
        event_token watcherEnumerationCompletedToken;
        event_token watcherStoppedToken;

        void StopWatcher();
        fire_and_forget OnDeviceAdded(Windows::Devices::Enumeration::DeviceWatcher const&, Windows::Devices::Enumeration::DeviceInformation deviceInfo);
        fire_and_forget OnDeviceRemoved(Windows::Devices::Enumeration::DeviceWatcher const&, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        fire_and_forget OnDeviceUpdated(Windows::Devices::Enumeration::DeviceWatcher const&, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        void OnEnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher const&, Windows::Foundation::IInspectable const&);
        void OnStopped(Windows::Devices::Enumeration::DeviceWatcher const&, Windows::Foundation::IInspectable const&);

        void OnConnectionStatusChanged(Windows::Devices::WiFiDirect::WiFiDirectDevice const& sender, Windows::Foundation::IInspectable const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Connector : Scenario2_ConnectorT<Scenario2_Connector, implementation::Scenario2_Connector>
    {
    };
}
