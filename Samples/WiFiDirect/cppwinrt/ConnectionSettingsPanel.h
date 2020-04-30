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
#include "ConnectionSettingsPanel.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct ConnectionSettingsPanel : ConnectionSettingsPanelT<ConnectionSettingsPanel>
    {
        ConnectionSettingsPanel();

        void Reset();
        Windows::Foundation::IAsyncOperation<bool> RequestPairDeviceAsync(Windows::Devices::Enumeration::DeviceInformationPairing pairing);

        void btnAddConfigMethods_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);        

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };
        std::vector<Windows::Devices::WiFiDirect::WiFiDirectConfigurationMethod> supportedConfigMethods;

        void OnPairingRequested(Windows::Devices::Enumeration::DeviceInformationCustomPairing const&, Windows::Devices::Enumeration::DevicePairingRequestedEventArgs const& e);
    };
}
namespace winrt::SDKTemplate::factory_implementation
{
    struct ConnectionSettingsPanel : ConnectionSettingsPanelT<ConnectionSettingsPanel, implementation::ConnectionSettingsPanel>
    {
    };
}
