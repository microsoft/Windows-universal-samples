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
#include "Scenario1_Discovery.h"
#include "Scenario1_Discovery.g.cpp"
#include "SampleConfiguration.h"
#include "BluetoothLEDeviceDisplay.h"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt
{
    hstring to_hstring(DevicePairingResultStatus status)
    {
        switch (status)
        {
        case DevicePairingResultStatus::Paired: return L"Paired";
        case DevicePairingResultStatus::NotReadyToPair: return L"NotReadyToPair";
        case DevicePairingResultStatus::NotPaired: return L"NotPaired";
        case DevicePairingResultStatus::AlreadyPaired: return L"AlreadyPaired";
        case DevicePairingResultStatus::ConnectionRejected: return L"ConnectionRejected";
        case DevicePairingResultStatus::TooManyConnections: return L"TooManyConnections";
        case DevicePairingResultStatus::HardwareFailure: return L"HardwareFailure";
        case DevicePairingResultStatus::AuthenticationTimeout: return L"AuthenticationTimeout";
        case DevicePairingResultStatus::AuthenticationNotAllowed: return L"AuthenticationNotAllowed";
        case DevicePairingResultStatus::AuthenticationFailure: return L"AuthenticationFailure";
        case DevicePairingResultStatus::NoSupportedProfiles: return L"NoSupportedProfiles";
        case DevicePairingResultStatus::ProtectionLevelCouldNotBeMet: return L"ProtectionLevelCouldNotBeMet";
        case DevicePairingResultStatus::AccessDenied: return L"AccessDenied";
        case DevicePairingResultStatus::InvalidCeremonyData: return L"InvalidCeremonyData";
        case DevicePairingResultStatus::PairingCanceled: return L"PairingCanceled";
        case DevicePairingResultStatus::OperationAlreadyInProgress: return L"OperationAlreadyInProgress";
        case DevicePairingResultStatus::RequiredHandlerNotRegistered: return L"RequiredHandlerNotRegistered";
        case DevicePairingResultStatus::RejectedByHandler: return L"RejectedByHandler";
        case DevicePairingResultStatus::RemoteDeviceHasAssociation: return L"RemoteDeviceHasAssociation";
        case DevicePairingResultStatus::Failed: return L"Failed";
        }
        return L"Code " + to_hstring(static_cast<int>(status));
    }
}

namespace winrt::SDKTemplate::implementation
{
    // This scenario uses a DeviceWatcher to enumerate nearby Bluetooth Low Energy devices,
    // displays them in a ListView, and lets the user select a device and pair it.
    // This device will be used by future scenarios.
    // For more information about device discovery and pairing, including examples of
    // customizing the pairing process, see the DeviceEnumerationAndPairing sample.

#pragma region UI Code

    Scenario1_Discovery::Scenario1_Discovery()
    {
        InitializeComponent();
    }

    void Scenario1_Discovery::OnNavigatedFrom(NavigationEventArgs const&)
    {
        StopBleDeviceWatcher();

        // Save the selected device's ID for use in other scenarios.
        auto bleDeviceDisplay = ResultsListView().SelectedItem().as<SDKTemplate::BluetoothLEDeviceDisplay>();
        if (bleDeviceDisplay != nullptr)
        {
            SampleState::SelectedBleDeviceId = bleDeviceDisplay.Id();
            SampleState::SelectedBleDeviceName = bleDeviceDisplay.Name();
        }
    }

    void Scenario1_Discovery::EnumerateButton_Click()
    {
        if (deviceWatcher == nullptr)
        {
            StartBleDeviceWatcher();
            EnumerateButton().Content(box_value(L"Stop enumerating"));
            rootPage.NotifyUser(L"Device watcher started.", NotifyType::StatusMessage);
        }
        else
        {
            StopBleDeviceWatcher();
            EnumerateButton().Content(box_value(L"Start enumerating"));
            rootPage.NotifyUser(L"Device watcher stopped.", NotifyType::StatusMessage);
        }
    }
#pragma endregion

#pragma region Device discovery
    /// <summary>
    /// Starts a device watcher that looks for all nearby Bluetooth devices (paired or unpaired). 
    /// Attaches event handlers to populate the device collection.
    /// </summary>
    void Scenario1_Discovery::StartBleDeviceWatcher()
    {
        // Additional properties we would like about the device.
        // Property strings are documented here https://msdn.microsoft.com/en-us/library/windows/desktop/ff521659(v=vs.85).aspx
        auto requestedProperties = single_threaded_vector<hstring>({ L"System.Devices.Aep.DeviceAddress", L"System.Devices.Aep.IsConnected", L"System.Devices.Aep.Bluetooth.Le.IsConnectable" });

        // BT_Code: Example showing paired and non-paired in a single query.
        hstring aqsAllBluetoothLEDevices = L"(System.Devices.Aep.ProtocolId:=\"{bb7bb05e-5972-42b5-94fc-76eaa7084d49}\")";

        deviceWatcher =
            Windows::Devices::Enumeration::DeviceInformation::CreateWatcher(
                aqsAllBluetoothLEDevices,
                requestedProperties,
                DeviceInformationKind::AssociationEndpoint);

        // Register event handlers before starting the watcher.
        deviceWatcherAddedToken = deviceWatcher.Added({ get_weak(), &Scenario1_Discovery::DeviceWatcher_Added });
        deviceWatcherUpdatedToken = deviceWatcher.Updated({ get_weak(), &Scenario1_Discovery::DeviceWatcher_Updated });
        deviceWatcherRemovedToken = deviceWatcher.Removed({ get_weak(), &Scenario1_Discovery::DeviceWatcher_Removed });
        deviceWatcherEnumerationCompletedToken = deviceWatcher.EnumerationCompleted({ get_weak(), &Scenario1_Discovery::DeviceWatcher_EnumerationCompleted });
        deviceWatcherStoppedToken = deviceWatcher.Stopped({ get_weak(), &Scenario1_Discovery::DeviceWatcher_Stopped });

        // Start over with an empty collection.
        m_knownDevices.Clear();

        // Start the watcher. Active enumeration is limited to approximately 30 seconds.
        // This limits power usage and reduces interference with other Bluetooth activities.
        // To monitor for the presence of Bluetooth LE devices for an extended period,
        // use the BluetoothLEAdvertisementWatcher runtime class. See the BluetoothAdvertisement
        // sample for an example.
        deviceWatcher.Start();
    }

    /// <summary>
    /// Stops watching for all nearby Bluetooth devices.
    /// </summary>
    void Scenario1_Discovery::StopBleDeviceWatcher()
    {
        if (deviceWatcher != nullptr)
        {
            // Unregister the event handlers.
            deviceWatcher.Added(deviceWatcherAddedToken);
            deviceWatcher.Updated(deviceWatcherUpdatedToken);
            deviceWatcher.Removed(deviceWatcherRemovedToken);
            deviceWatcher.EnumerationCompleted(deviceWatcherEnumerationCompletedToken);
            deviceWatcher.Stopped(deviceWatcherStoppedToken);

            // Stop the watcher.
            deviceWatcher.Stop();
            deviceWatcher = nullptr;
        }
    }

    std::tuple<SDKTemplate::BluetoothLEDeviceDisplay, uint32_t> Scenario1_Discovery::FindBluetoothLEDeviceDisplay(hstring const& id)
    {
        uint32_t size = m_knownDevices.Size();
        for (uint32_t index = 0; index < size; index++)
        {
            auto bleDeviceDisplay = m_knownDevices.GetAt(index).as<SDKTemplate::BluetoothLEDeviceDisplay>();
            if (bleDeviceDisplay.Id() == id)
            {
                return { bleDeviceDisplay, index };
            }
        }
        return { nullptr, 0-1U };
    }

    std::vector<Windows::Devices::Enumeration::DeviceInformation>::iterator Scenario1_Discovery::FindUnknownDevices(hstring const& id)
    {
        return std::find_if(UnknownDevices.begin(), UnknownDevices.end(), [&](auto&& bleDeviceInfo)
        {
            return bleDeviceInfo.Id() == id;
        });
    }

    fire_and_forget Scenario1_Discovery::DeviceWatcher_Added(DeviceWatcher sender, DeviceInformation deviceInfo)
    {
        // We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        OutputDebugStringW((L"Added " + deviceInfo.Id() + deviceInfo.Name()).c_str());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            // Make sure device isn't already present in the list.
            if (std::get<0>(FindBluetoothLEDeviceDisplay(deviceInfo.Id())) == nullptr)
            {
                if (!deviceInfo.Name().empty())
                {
                    // If device has a friendly name display it immediately.
                    m_knownDevices.Append(make<BluetoothLEDeviceDisplay>(deviceInfo));
                }
                else
                {
                    // Add it to a list in case the name gets updated later. 
                    UnknownDevices.push_back(deviceInfo);
                }
            }
        }
    }

    fire_and_forget Scenario1_Discovery::DeviceWatcher_Updated(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
    {
        // We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        OutputDebugStringW((L"Updated " + deviceInfoUpdate.Id()).c_str());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            SDKTemplate::BluetoothLEDeviceDisplay bleDeviceDisplay = std::get<0>(FindBluetoothLEDeviceDisplay(deviceInfoUpdate.Id()));
            if (bleDeviceDisplay != nullptr)
            {
                // Device is already being displayed - update UX.
                bleDeviceDisplay.Update(deviceInfoUpdate);
                co_return;
            }

            auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
            if (deviceInfo != UnknownDevices.end())
            {
                deviceInfo->Update(deviceInfoUpdate);
                // If device has been updated with a friendly name it's no longer unknown.
                if (!deviceInfo->Name().empty())
                {
                    m_knownDevices.Append(make<BluetoothLEDeviceDisplay>(*deviceInfo));
                    UnknownDevices.erase(deviceInfo);
                }
            }
        }
    }

    fire_and_forget Scenario1_Discovery::DeviceWatcher_Removed(DeviceWatcher sender, DeviceInformationUpdate deviceInfoUpdate)
    {
        // We must update the collection on the UI thread because the collection is databound to a UI element.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        OutputDebugStringW((L"Removed " + deviceInfoUpdate.Id()).c_str());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            // Find the corresponding DeviceInformation in the collection and remove it.
            auto[bleDeviceDisplay, index] = FindBluetoothLEDeviceDisplay(deviceInfoUpdate.Id());
            if (bleDeviceDisplay != nullptr)
            {
                m_knownDevices.RemoveAt(index);
            }

            auto deviceInfo = FindUnknownDevices(deviceInfoUpdate.Id());
            if (deviceInfo != UnknownDevices.end())
            {
                UnknownDevices.erase(deviceInfo);
            }
        }
    }

    fire_and_forget Scenario1_Discovery::DeviceWatcher_EnumerationCompleted(DeviceWatcher sender, IInspectable const&)
    {
        // Access this->deviceWatcher on the UI thread to avoid race conditions.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            rootPage.NotifyUser(to_hstring(m_knownDevices.Size()) + L" devices found. Enumeration completed.",
                NotifyType::StatusMessage);
        }
    }

    fire_and_forget Scenario1_Discovery::DeviceWatcher_Stopped(DeviceWatcher sender, IInspectable const&)
    {
        // Access this->deviceWatcher on the UI thread to avoid race conditions.
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());

        // Protect against race condition if the task runs after the app stopped the deviceWatcher.
        if (sender == deviceWatcher)
        {
            rootPage.NotifyUser(L"No longer watching for devices.",
                sender.Status() == DeviceWatcherStatus::Aborted ? NotifyType::ErrorMessage : NotifyType::StatusMessage);
        }
    }
#pragma endregion

#pragma region Pairing
    fire_and_forget Scenario1_Discovery::PairButton_Click()
    {
        auto lifetime = get_strong();
        EnumerateButton().IsEnabled(false);

        rootPage.NotifyUser(L"Pairing started. Please wait...", NotifyType::StatusMessage);

        // For more information about device pairing, including examples of
        // customizing the pairing process, see the DeviceEnumerationAndPairing sample.

        // Capture the current selected item in case the user changes it while we are pairing.
        auto bleDeviceDisplay = ResultsListView().SelectedItem().as<SDKTemplate::BluetoothLEDeviceDisplay>();

        // BT_Code: Pair the currently selected device.
        DevicePairingResult result = co_await bleDeviceDisplay.DeviceInformation().Pairing().PairAsync();
        DevicePairingResultStatus status = result.Status();
        rootPage.NotifyUser(L"Pairing result = " + to_hstring(status),
            status == DevicePairingResultStatus::Paired || status == DevicePairingResultStatus::AlreadyPaired
            ? NotifyType::StatusMessage
            : NotifyType::ErrorMessage);

        EnumerateButton().IsEnabled(true);
    }
#pragma endregion
}
