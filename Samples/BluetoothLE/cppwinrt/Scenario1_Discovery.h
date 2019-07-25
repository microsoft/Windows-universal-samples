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

#include "Scenario1_Discovery.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Discovery : Scenario1_DiscoveryT<Scenario1_Discovery>
    {
        Scenario1_Discovery();

        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> KnownDevices()
        {
            return m_knownDevices;
        }

        void EnumerateButton_Click();
        fire_and_forget PairButton_Click();
        bool Not(bool value) { return !value; }

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_knownDevices = single_threaded_observable_vector<Windows::Foundation::IInspectable>();
        std::vector<Windows::Devices::Enumeration::DeviceInformation> UnknownDevices;
        Windows::Devices::Enumeration::DeviceWatcher deviceWatcher{ nullptr };
        event_token deviceWatcherAddedToken;
        event_token deviceWatcherUpdatedToken;
        event_token deviceWatcherRemovedToken;
        event_token deviceWatcherEnumerationCompletedToken;
        event_token deviceWatcherStoppedToken;

        void StartBleDeviceWatcher();
        void StopBleDeviceWatcher();
        std::tuple<SDKTemplate::BluetoothLEDeviceDisplay, uint32_t> FindBluetoothLEDeviceDisplay(hstring const& id);
        std::vector<Windows::Devices::Enumeration::DeviceInformation>::iterator FindUnknownDevices(hstring const& id);

        fire_and_forget DeviceWatcher_Added(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformation deviceInfo);
        fire_and_forget DeviceWatcher_Updated(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        fire_and_forget DeviceWatcher_Removed(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Devices::Enumeration::DeviceInformationUpdate deviceInfoUpdate);
        fire_and_forget DeviceWatcher_EnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&);
        fire_and_forget DeviceWatcher_Stopped(Windows::Devices::Enumeration::DeviceWatcher sender, Windows::Foundation::IInspectable const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Discovery : Scenario1_DiscoveryT<Scenario1_Discovery, implementation::Scenario1_Discovery>
    {
    };
}
