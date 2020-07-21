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
#include "Scenario1_Advertiser.h"
#include "SampleConfiguration.h"
#include "SocketReaderWriter.h"
#include "Scenario1_Advertiser.g.cpp"

using namespace winrt;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::WiFiDirect;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Credentials;
using namespace Windows::Security::Cryptography;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Advertiser::Scenario1_Advertiser()
    {
        InitializeComponent();
    }

    void Scenario1_Advertiser::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (btnStopAdvertisement().IsEnabled())
        {
            StopAdvertisement();
        }
    }

    void Scenario1_Advertiser::btnStartAdvertisement_Click(IInspectable const&, RoutedEventArgs const&)
    {
        publisher = WiFiDirectAdvertisementPublisher();
        statusChangedToken = publisher.StatusChanged({ get_weak(), &Scenario1_Advertiser::OnStatusChanged });

        listener = WiFiDirectConnectionListener();

        if (chkListener().IsChecked().Value())
        {
            try
            {
                // This can raise an exception if the machine does not support WiFi. Sorry.
                listener.ConnectionRequested({ get_weak(), &Scenario1_Advertiser::OnConnectionRequested });
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(L"Error preparing Advertisement: " + ex.message(), NotifyType::ErrorMessage);
                return;
            }
        }

        auto discoverability = GetSelectedItemTag<WiFiDirectAdvertisementListenStateDiscoverability>(cmbListenState());
        publisher.Advertisement().ListenStateDiscoverability(discoverability);

        publisher.Advertisement().IsAutonomousGroupOwnerEnabled(chkPreferGroupOwnerMode().IsChecked().Value());

        // Legacy settings are meaningful only if IsAutonomousGroupOwnerEnabled is true.
        if (publisher.Advertisement().IsAutonomousGroupOwnerEnabled() && chkLegacySetting().IsChecked().Value())
        {
            publisher.Advertisement().LegacySettings().IsEnabled(true);
            if (!txtPassphrase().Text().empty())
            {
                PasswordCredential creds;
                creds.Password(txtPassphrase().Text());
                publisher.Advertisement().LegacySettings().Passphrase(creds);
            }

            if (!txtSsid().Text().empty())
            {
                publisher.Advertisement().LegacySettings().Ssid(txtSsid().Text());
            }
        }

        // Add the information elements.
        publisher.Advertisement().InformationElements().ReplaceAll(informationElements);

        publisher.Start();

        if (publisher.Status() == WiFiDirectAdvertisementPublisherStatus::Started)
        {
            btnStartAdvertisement().IsEnabled(false);
            btnStopAdvertisement().IsEnabled(true);
            rootPage.NotifyUser(L"Advertisement started.", NotifyType::StatusMessage);
        }
        else
        {
            rootPage.NotifyUser(L"Advertisement failed to start. Status is " + to_hstring(publisher.Status()), NotifyType::ErrorMessage);
        }
    }

    void Scenario1_Advertiser::btnAddIe_Click(IInspectable const&, RoutedEventArgs const&)
    {
        WiFiDirectInformationElement informationElement;

        // Information element blob
        DataWriter dataWriter;
        dataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
        dataWriter.ByteOrder(ByteOrder::LittleEndian);
        dataWriter.WriteUInt32(dataWriter.MeasureString(txtInformationElement().Text()));
        dataWriter.WriteString(txtInformationElement().Text());
        informationElement.Value(dataWriter.DetachBuffer());

        // Organizational unit identifier (OUI)
        informationElement.Oui(CryptographicBuffer::CreateFromByteArray(CustomOui));

        // OUI Type
        informationElement.OuiType(CustomOuiType);

        // Save this information element so we can add it when we advertise.
        informationElements.push_back(informationElement);

        txtInformationElement().Text({});
        rootPage.NotifyUser(L"IE added successfully", NotifyType::StatusMessage);
    }

    void Scenario1_Advertiser::btnStopAdvertisement_Click(IInspectable const&, RoutedEventArgs const&)
    {
        StopAdvertisement();
        rootPage.NotifyUser(L"Advertisement stopped successfully", NotifyType::StatusMessage);
    }

    void Scenario1_Advertiser::StopAdvertisement()
    {
        publisher.Stop();
        publisher.StatusChanged(statusChangedToken);

        listener.ConnectionRequested(connectionRequestedToken);

        connectionSettingsPanel().Reset();
        informationElements.clear();

        btnStartAdvertisement().IsEnabled(true);
        btnStopAdvertisement().IsEnabled(false);
    }

    IAsyncOperation<bool> Scenario1_Advertiser::HandleConnectionRequestAsync(WiFiDirectConnectionRequest connectionRequest)
    {
        auto lifetime = get_strong();

        hstring deviceName = connectionRequest.DeviceInformation().Name();

        DeviceInformationPairing pairing = connectionRequest.DeviceInformation().Pairing();
        bool isPaired = (pairing && pairing.IsPaired()) ||
            (co_await IsAepPairedAsync(connectionRequest.DeviceInformation().Id()));

        // Show the prompt only in case of WiFiDirect reconnection or Legacy client connection.
        if (isPaired || publisher.Advertisement().LegacySettings().IsEnabled())
        {
            MessageDialog messageDialog{ L"Connection request received from " + deviceName, L"Connection Request" };

            // Add two commands, distinguished by their tag.
            // The default command is "Decline", and if the user cancels, we treat it as "Decline".
            messageDialog.Commands().Append(UICommand(L"Accept", nullptr, box_value(true)));
            messageDialog.Commands().Append(UICommand(L"Decline", nullptr, nullptr));

            messageDialog.DefaultCommandIndex(1);
            messageDialog.CancelCommandIndex(1);

            // Show the message dialog
            auto commandChosen = co_await messageDialog.ShowAsync();

            if (commandChosen.Id() == nullptr)
            {
                // Decline
                co_return false;
            }

            rootPage.NotifyUser(L"Connecting to " + deviceName + L"...", NotifyType::StatusMessage);

            // Pair device if not already paired and not using legacy settings
            if (!isPaired && !publisher.Advertisement().LegacySettings().IsEnabled())
            {
                if (!co_await connectionSettingsPanel().RequestPairDeviceAsync(pairing))
                {
                    co_return false;
                }
            }

            WiFiDirectDevice wfdDevice = nullptr;
            try
            {
                // IMPORTANT: FromIdAsync needs to be called from the UI thread
                wfdDevice = co_await WiFiDirectDevice::FromIdAsync(connectionRequest.DeviceInformation().Id());
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(L"Exception in FromIdAsync: " + ex.message(), NotifyType::ErrorMessage);
                co_return false;
            }

            // Register for the ConnectionStatusChanged event handler
            connectionStatusChangedToken = wfdDevice.ConnectionStatusChanged({ get_weak(), &Scenario1_Advertiser::OnConnectionStatusChanged });

            StreamSocketListener listenerSocket;

            // Remembering this listenerSocket serves two purposes.
            // 1. It keeps the listenerSocket alive until the connection is received.
            // 2. It allows us to map the listenerSocket to the corresponding WiFiDirectDevice
            //    when the connection is received.
            {
                auto lock = slim_lock_guard(pendingConnectionsMutex);
                pendingConnections.insert_or_assign(listenerSocket, wfdDevice);
            }
            listenerSocket.ConnectionReceived({ get_weak(), &Scenario1_Advertiser::OnSocketConnectionReceived });

            auto EndpointPairs = wfdDevice.GetConnectionEndpointPairs();
            try
            {
                co_await listenerSocket.BindEndpointAsync(EndpointPairs.GetAt(0).LocalHostName(), ServerPortString);
            }
            catch (hresult_error const& ex)
            {
                rootPage.NotifyUser(L"Connect operation threw an exception: " + ex.message(), NotifyType::ErrorMessage);
                co_return false;
            }

            rootPage.NotifyUser(L"Devices connected on L2, listening on IP Address: " + EndpointPairs.GetAt(0).LocalHostName().DisplayName() +
                L" Port: " + ServerPortString, NotifyType::StatusMessage);
            co_return true;
        }
    }

    fire_and_forget Scenario1_Advertiser::OnConnectionRequested(WiFiDirectConnectionListener const&, WiFiDirectConnectionRequestedEventArgs const& e)
    {
        auto lifetime = get_strong();

        WiFiDirectConnectionRequest connectionRequest = e.GetConnectionRequest();
        co_await resume_foreground(Dispatcher());
        if (!co_await HandleConnectionRequestAsync(connectionRequest))
        {
            // Decline the connection request
            rootPage.NotifyUser(L"Connection request from " + connectionRequest.DeviceInformation().Name() + L" was declined", NotifyType::ErrorMessage);
            connectionRequest.Close();
        }
    }

    IAsyncOperation<bool> Scenario1_Advertiser::IsAepPairedAsync(hstring deviceId)
    {
        auto lifetime = get_strong();

        DeviceInformation devInfo = nullptr;
        try
        {
            devInfo = co_await DeviceInformation::CreateFromIdAsync(deviceId, { L"System.Devices.Aep.DeviceAddress" });
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"DeviceInformation.CreateFromIdAsync threw an exception: " + ex.message(), NotifyType::ErrorMessage);
        }

        if (devInfo == nullptr)
        {
            rootPage.NotifyUser(L"Device Information is null", NotifyType::ErrorMessage);
            co_return false;
        }

        hstring deviceAddress = unbox_value<hstring>(devInfo.Properties().Lookup(L"System.Devices.Aep.DeviceAddress"));
        hstring deviceSelector = L"System.Devices.Aep.DeviceAddress:=\"" + deviceAddress + L"\"";
        DeviceInformationCollection pairedDeviceCollection = co_await DeviceInformation::FindAllAsync(deviceSelector, nullptr, DeviceInformationKind::Device);
        co_return pairedDeviceCollection.Size() > 0;
    }

    fire_and_forget Scenario1_Advertiser::OnStatusChanged(WiFiDirectAdvertisementPublisher sender, WiFiDirectAdvertisementPublisherStatusChangedEventArgs e)
    {
        auto lifetime = get_strong();

        if (e.Status() == WiFiDirectAdvertisementPublisherStatus::Started)
        {
            co_await resume_foreground(Dispatcher());
            if (sender.Advertisement().LegacySettings().IsEnabled())
            {
                // Show the autogenerated passphrase and SSID.
                if (txtPassphrase().Text().empty())
                {
                    txtPassphrase().Text(publisher.Advertisement().LegacySettings().Passphrase().Password());
                }

                if (txtSsid().Text().empty())
                {
                    txtSsid().Text(publisher.Advertisement().LegacySettings().Ssid());
                }
            }
        }

        rootPage.NotifyUser(L"Advertisement: Status: " + to_hstring(e.Status()) + L", Error: " + to_hstring(e.Error()), NotifyType::StatusMessage);
        co_return;
    }

    fire_and_forget Scenario1_Advertiser::OnSocketConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs e)
    {
        auto lifetime = get_strong();

        co_await resume_foreground(Dispatcher());
        rootPage.NotifyUser(L"Connecting to remote side on L4 layer...", NotifyType::StatusMessage);
        StreamSocket serverSocket = e.Socket();

        // Look up the WiFiDirectDevice associated with this StreamSocketListener.
        WiFiDirectDevice wfdDevice = nullptr;
        decltype(pendingConnections.extract(sender)) node;
        {
            auto lock = slim_lock_guard(pendingConnectionsMutex);
            node = pendingConnections.extract(sender);
            if (node)
            {
                wfdDevice = node.mapped();
            }
        }

        if (wfdDevice == nullptr)
        {
            rootPage.NotifyUser(L"Unexpected connection ignored.", NotifyType::ErrorMessage);
            serverSocket.Close();
            co_return;
        }

        auto socketRW = make_self<SocketReaderWriter>(serverSocket, rootPage);

        // The first message sent is the name of the connection.
        IReference<hstring> message = co_await socketRW->ReadMessageAsync();

        // Add this connection to the list of active connections.
        connectedDevices.Append(make<ConnectedDevice>(message ? message.Value() : L"(unnamed)", wfdDevice, socketRW));

        // Keep reading messages until the socket is closed.
        while (message)
        {
            message = co_await socketRW->ReadMessageAsync();
        }
    }

    void Scenario1_Advertiser::OnConnectionStatusChanged(WiFiDirectDevice const& sender, IInspectable const&)
    {
        rootPage.NotifyUser(L"Connection status changed: " + to_hstring(sender.ConnectionStatus()), NotifyType::StatusMessage);

        if (sender.ConnectionStatus() == WiFiDirectConnectionStatus::Disconnected)
        {
            // TODO: Should we remove this connection from the list?
            // (Yes, probably.)
        }
    }

    fire_and_forget Scenario1_Advertiser::btnSendMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        auto connectedDevice = lvConnectedDevices().SelectedItem().as<SDKTemplate::ConnectedDevice>();
        co_await get_self<ConnectedDevice>(connectedDevice)->SocketRW()->WriteMessageAsync(txtSendMessage().Text());
    }

    void Scenario1_Advertiser::btnCloseDevice_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto connectedDevice = lvConnectedDevices().SelectedItem().as<SDKTemplate::ConnectedDevice>();
        RemoveByValue(connectedDevices, connectedDevice);

        // Close socket and WiFiDirect object
        connectedDevice.Close();
    }
}


