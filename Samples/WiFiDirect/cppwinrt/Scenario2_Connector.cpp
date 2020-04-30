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
#include "Scenario2_Connector.h"
#include "SampleConfiguration.h"
#include "SocketReaderWriter.h"
#include "Scenario2_Connector.g.cpp"
#include <chrono>
#include <sstream>

using namespace std::chrono_literals;
using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFiDirect;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Security::Cryptography;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Connector::Scenario2_Connector()
    {
        InitializeComponent();
    }

    void Scenario2_Connector::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (deviceWatcher)
        {
            StopWatcher();
        }
    }

    void Scenario2_Connector::StopWatcher()
    {
        deviceWatcher.Added(watcherAddedToken);
        deviceWatcher.Removed(watcherRemovedToken);
        deviceWatcher.Updated(watcherUpdatedToken);
        deviceWatcher.EnumerationCompleted(watcherEnumerationCompletedToken);
        deviceWatcher.Stopped(watcherStoppedToken);

        deviceWatcher.Stop();
        deviceWatcher = nullptr;
    }

    void Scenario2_Connector::btnWatcher_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!deviceWatcher)
        {
            publisher.Start();

            if (publisher.Status() != WiFiDirectAdvertisementPublisherStatus::Started)
            {
                rootPage.NotifyUser(L"Failed to start advertisement.", NotifyType::ErrorMessage);
                return;
            }

            discoveredDevices.Clear();
            rootPage.NotifyUser(L"Finding Devices...", NotifyType::StatusMessage);

            hstring deviceSelector = WiFiDirectDevice::GetDeviceSelector(
                GetSelectedItemTag<WiFiDirectDeviceSelectorType>(cmbDeviceSelector()));

            deviceWatcher = DeviceInformation::CreateWatcher(deviceSelector, { L"System.Devices.WiFiDirect.InformationElements" });

            watcherAddedToken = deviceWatcher.Added({ get_weak(), &Scenario2_Connector::OnDeviceAdded });
            watcherRemovedToken = deviceWatcher.Removed({ get_weak(), &Scenario2_Connector::OnDeviceRemoved });
            watcherUpdatedToken = deviceWatcher.Updated({ get_weak(), &Scenario2_Connector::OnDeviceUpdated });
            watcherEnumerationCompletedToken = deviceWatcher.EnumerationCompleted({ get_weak(), &Scenario2_Connector::OnEnumerationCompleted });
            watcherStoppedToken = deviceWatcher.Stopped({ get_weak(), &Scenario2_Connector::OnStopped });

            deviceWatcher.Start();

            btnWatcher().Content(box_value(L"Stop Watcher"));
        }
        else
        {
            publisher.Stop();

            btnWatcher().Content(box_value(L"Start Watcher"));

            StopWatcher();

            rootPage.NotifyUser(L"Device watcher stopped.", NotifyType::StatusMessage);
        }
    }

    #pragma region DeviceWatcherEvents
    fire_and_forget Scenario2_Connector::OnDeviceAdded(DeviceWatcher const&, DeviceInformation deviceInfo)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());
        discoveredDevices.Append(make<DiscoveredDevice>(deviceInfo));
    }

    fire_and_forget Scenario2_Connector::OnDeviceRemoved(DeviceWatcher const&, DeviceInformationUpdate deviceInfoUpdate)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());
        unsigned int size = discoveredDevices.Size();
        for (unsigned int index = 0; index < size; index++)
        {
            SDKTemplate::DiscoveredDevice discoveredDevice = discoveredDevices.GetAt(index);
            DiscoveredDevice* discoveredDeviceImpl = get_self<DiscoveredDevice>(discoveredDevice);
            if (discoveredDeviceImpl->DeviceInfo().Id() == deviceInfoUpdate.Id())
            {
                discoveredDevices.RemoveAt(index);
                break;
            }
        }
    }

    fire_and_forget Scenario2_Connector::OnDeviceUpdated(DeviceWatcher const&, DeviceInformationUpdate deviceInfoUpdate)
    {
        auto lifetime = get_strong();
        co_await resume_foreground(Dispatcher());
        for (SDKTemplate::DiscoveredDevice const& discoveredDevice : discoveredDevices)
        {
            DiscoveredDevice* discoveredDeviceImpl = get_self<DiscoveredDevice>(discoveredDevice);
            if (discoveredDeviceImpl->DeviceInfo().Id() == deviceInfoUpdate.Id())
            {
                discoveredDeviceImpl->UpdateDeviceInfo(deviceInfoUpdate);
                break;
            }
        }
    }

    void Scenario2_Connector::OnEnumerationCompleted(DeviceWatcher const&, IInspectable const&)
    {
        rootPage.NotifyUser(L"DeviceWatcher enumeration completed", NotifyType::StatusMessage);
    }

    void Scenario2_Connector::OnStopped(DeviceWatcher const&, IInspectable const&)
    {
        rootPage.NotifyUser(L"DeviceWatcher stopped", NotifyType::StatusMessage);
    }
    #pragma endregion

    void Scenario2_Connector::btnIe_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto discoveredDevice = lvDiscoveredDevices().SelectedItem().as<SDKTemplate::DiscoveredDevice>();

        IVector<WiFiDirectInformationElement> informationElements = nullptr;
        try
        {
            DiscoveredDevice* discoveredDeviceImpl = get_self<DiscoveredDevice>(discoveredDevice);
            informationElements = WiFiDirectInformationElement::CreateFromDeviceInformation(discoveredDeviceImpl->DeviceInfo());
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"No Information element found: " + ex.message(), NotifyType::ErrorMessage);
            return;
        }

        std::wostringstream message;

        for (WiFiDirectInformationElement const& informationElement : informationElements)
        {
            hstring ouiName = CryptographicBuffer::EncodeToHexString(informationElement.Oui());
            hstring value;

            com_array<uint8_t> ouiArray;
            CryptographicBuffer::CopyToByteArray(informationElement.Oui(), ouiArray);
            array_view<uint8_t const> ouiConstArray{ ouiArray.begin(), ouiArray.end() };

            if (ouiConstArray == array_view<uint8_t const>(MsftOui))
            {
                // The format of Microsoft information elements is documented here:
                // https://msdn.microsoft.com/en-us/library/dn392651.aspx
                // with errata here:
                // https://msdn.microsoft.com/en-us/library/mt242386.aspx
                ouiName = ouiName + L" (Microsoft)";
            }
            else if (ouiConstArray == array_view<uint8_t const>(WfaOui))
            {
                ouiName = ouiName + L" (WFA)";
            }
            else if (ouiConstArray == array_view<uint8_t const>(CustomOui))
            {
                ouiName = ouiName + L" (Custom)";

                if (informationElement.OuiType() == CustomOuiType)
                {
                    DataReader dataReader = DataReader::FromBuffer(informationElement.Value());
                    dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
                    dataReader.ByteOrder(ByteOrder::LittleEndian);

                    // Read the string.
                    try
                    {
                        hstring data = dataReader.ReadString(dataReader.ReadUInt32());
                        value = L"Data: " + data;
                    }
                    catch (hresult_error const&)
                    {
                        value = L"(Unable to parse)";
                    }
                }
            }

            message << L"OUI " << std::wstring_view(ouiName)
                << L", Type " << static_cast<uint32_t>(informationElement.OuiType())
                << L" " << std::wstring_view(value) << std::endl;
        }

        message << L"Information elements found: " << informationElements.Size();

        rootPage.NotifyUser(message.str(), NotifyType::StatusMessage);
    }

    fire_and_forget Scenario2_Connector::btnFromId_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        auto discoveredDevice = lvDiscoveredDevices().SelectedItem().as<SDKTemplate::DiscoveredDevice>();
        DeviceInformation deviceInfo = get_self<DiscoveredDevice>(discoveredDevice)->DeviceInfo();
        rootPage.NotifyUser(L"Connecting to " + deviceInfo.Name() + L"...", NotifyType::StatusMessage);

        if (!deviceInfo.Pairing().IsPaired())
        {
            if (!co_await connectionSettingsPanel().RequestPairDeviceAsync(deviceInfo.Pairing()))
            {
                co_return;
            }
        }

        WiFiDirectDevice wfdDevice = nullptr;
        try
        {
            // IMPORTANT: FromIdAsync needs to be called from the UI thread
            wfdDevice = co_await WiFiDirectDevice::FromIdAsync(deviceInfo.Id());
        }
        catch (hresult_canceled const&)
        {
            rootPage.NotifyUser(L"FromIdAsync was canceled by user", NotifyType::ErrorMessage);
            co_return;
        }

        // Register for the ConnectionStatusChanged event handler
        wfdDevice.ConnectionStatusChanged({ get_weak(), &Scenario2_Connector::OnConnectionStatusChanged });

        IVectorView<EndpointPair> endpointPairs = wfdDevice.GetConnectionEndpointPairs();
        HostName remoteHostName = endpointPairs.GetAt(0).RemoteHostName();

        rootPage.NotifyUser(L"Devices connected on L2 layer, connecting to IP Address: " + remoteHostName.DisplayName() +
            L" Port: " + ServerPortString, NotifyType::StatusMessage);

        // Wait for server to start listening on a socket
        co_await resume_after(2s);

        // resume_after does not preserve thread context. Get back to the UI thread.
        co_await resume_foreground(Dispatcher());

        // Connect to Advertiser on L4 layer
        StreamSocket clientSocket;
        try
        {
            co_await clientSocket.ConnectAsync(remoteHostName, ServerPortString);
            rootPage.NotifyUser(L"Connected with remote side on L4 layer", NotifyType::StatusMessage);
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"Connect operation threw an exception: " + ex.message(), NotifyType::ErrorMessage);
            co_return;
        }

        auto socketRW = make_self<SocketReaderWriter>(clientSocket, rootPage);

        hstring sessionId = to_hstring(CryptographicBuffer::GenerateRandomNumber());
        connectedDevices.Append(make<ConnectedDevice>(sessionId, wfdDevice, socketRW));

        // The first message sent over the socket is the name of the connection.
        co_await socketRW->WriteMessageAsync(sessionId);

        while (co_await socketRW->ReadMessageAsync() != nullptr)
        {
            // Keep reading messages
        }
    }

    void Scenario2_Connector::OnConnectionStatusChanged(WiFiDirectDevice const& sender, IInspectable const&)
    {
        rootPage.NotifyUser(L"Connection status changed: " + to_hstring(sender.ConnectionStatus()), NotifyType::StatusMessage);
    }

    fire_and_forget Scenario2_Connector::btnSendMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();
        auto connectedDevice = lvConnectedDevices().SelectedItem().as<SDKTemplate::ConnectedDevice>();
        co_await get_self<ConnectedDevice>(connectedDevice)->SocketRW()->WriteMessageAsync(txtSendMessage().Text());
    }

    void Scenario2_Connector::btnClose_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto connectedDevice = lvConnectedDevices().SelectedItem().as<SDKTemplate::ConnectedDevice>();
        RemoveByValue(connectedDevices, connectedDevice);

        // Close socket and WiFiDirect object
        connectedDevice.Close();
    }

    fire_and_forget Scenario2_Connector::btnUnpair_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        auto discoveredDevice = lvDiscoveredDevices().SelectedItem().as<SDKTemplate::DiscoveredDevice>();
        DeviceInformation deviceInfo = get_self<DiscoveredDevice>(discoveredDevice)->DeviceInfo();

        DeviceUnpairingResult result = co_await deviceInfo.Pairing().UnpairAsync();
        rootPage.NotifyUser(L"Unpair result: " + to_hstring(result.Status()), NotifyType::StatusMessage);
    }
}
