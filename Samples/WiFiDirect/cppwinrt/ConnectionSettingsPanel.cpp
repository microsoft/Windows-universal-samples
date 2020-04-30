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
#include "ConnectionSettingsPanel.h"
#include "SampleConfiguration.h"
#include "ConnectionSettingsPanel.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFiDirect;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SDKTemplate::implementation
{
    ConnectionSettingsPanel::ConnectionSettingsPanel()
    {
        InitializeComponent();

        auto CreateComboBoxItem = [](hstring const& name, IInspectable const& tag = nullptr)
        {
            ComboBoxItem item;
            item.Content(box_value(name));
            item.Tag(tag);
            return item;
        };

        // Initialize the GroupOwnerIntent combo box.
        // The options are "Default", then values 0 through 15.
        cmbGOIntent().Items().Append(CreateComboBoxItem(L"Default"));
        for (int i = 0; i <= 15; i++)
        {
            cmbGOIntent().Items().Append(CreateComboBoxItem(to_hstring(i), box_value(static_cast<int16_t>(i))));
        }
        cmbGOIntent().SelectedIndex(0);

    }

    void ConnectionSettingsPanel::Reset()
    {
        supportedConfigMethods.clear();
    }

    void ConnectionSettingsPanel::btnAddConfigMethods_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // The configuration methods are added in the order of preference, most preferred first.
        // Save the values here so we can apply them in RequestPairDeviceAsync.
        // In practice, most apps will have a hard-coded preference list.
        WiFiDirectConfigurationMethod method = GetSelectedItemTag<WiFiDirectConfigurationMethod>(cmbSupportedConfigMethods());
        supportedConfigMethods.push_back(method);
        rootPage.NotifyUser(L"Added configuration method " + to_hstring(method), NotifyType::StatusMessage);
    }

    IAsyncOperation<bool> ConnectionSettingsPanel::RequestPairDeviceAsync(DeviceInformationPairing pairing)
    {
        auto lifetime = get_strong();

        WiFiDirectConnectionParameters connectionParams;

        // Optional custom GroupOwnerIntent.
        IReference<int16_t> groupOwnerIntent = GetSelectedItemTag<IReference<int16_t>>(cmbGOIntent());
        if (groupOwnerIntent)
        {
            connectionParams.GroupOwnerIntent(groupOwnerIntent.Value());
        }

        DevicePairingKinds devicePairingKinds = DevicePairingKinds::None;

        // If specific configuration methods were added, then use them.
        if (!supportedConfigMethods.empty())
        {
            for (WiFiDirectConfigurationMethod configMethod : supportedConfigMethods)
            {
                connectionParams.PreferenceOrderedConfigurationMethods().Append(configMethod);
                devicePairingKinds |= WiFiDirectConnectionParameters::GetDevicePairingKinds(configMethod);
            }
        }
        else
        {
            // If specific configuration methods were not added, then we'll use these pairing kinds.
            devicePairingKinds = DevicePairingKinds::ConfirmOnly | DevicePairingKinds::DisplayPin | DevicePairingKinds::ProvidePin;
        }

        connectionParams.PreferredPairingProcedure(GetSelectedItemTag<WiFiDirectPairingProcedure>(cmbPreferredPairingProcedure()));
        DeviceInformationCustomPairing customPairing = pairing.Custom();
        customPairing.PairingRequested({ get_weak(), &ConnectionSettingsPanel::OnPairingRequested });

        DevicePairingResult result = co_await customPairing.PairAsync(devicePairingKinds, DevicePairingProtectionLevel::Default, connectionParams);
        if (result.Status() != DevicePairingResultStatus::Paired)
        {
            rootPage.NotifyUser(L"PairAsync failed, Status: " + to_hstring(result.Status()), NotifyType::ErrorMessage);
            co_return false;
        }
        co_return true;
    }

    void ConnectionSettingsPanel::OnPairingRequested(DeviceInformationCustomPairing const&, DevicePairingRequestedEventArgs const& e)
    {
        HandlePairing(Dispatcher(), e);
    }
}
