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
#include "MainPage.h"
#include "DiscoveredDevice.g.h"
#include "ConnectedDevice.g.h"
#include "Utils.g.h"

namespace winrt::SDKTemplate
{
    struct SocketReaderWriter : implements<SocketReaderWriter, Windows::Foundation::IClosable>
    {
        SocketReaderWriter(Windows::Networking::Sockets::StreamSocket socket, MainPage mainPage);
        void Close();
        Windows::Foundation::IAsyncAction WriteMessageAsync(hstring const& message);
        Windows::Foundation::IAsyncOperation<Windows::Foundation::IReference<hstring>> ReadMessageAsync();
    private:
        MainPage rootPage;
        Windows::Networking::Sockets::StreamSocket streamSocket;
        Windows::Storage::Streams::DataReader dataReader;
        Windows::Storage::Streams::DataWriter dataWriter;
    };
}

namespace winrt::SDKTemplate::implementation
{
    struct DiscoveredDevice : DiscoveredDeviceT<DiscoveredDevice>
    {
    public:
        DiscoveredDevice(Windows::Devices::Enumeration::DeviceInformation const& deviceInfoIn) : deviceInfo(deviceInfoIn)
        {
        }

        auto DeviceInfo() { return deviceInfo; }

        hstring DisplayName()
        {
            return deviceInfo.Name() + L" - " + (deviceInfo.Pairing().IsPaired() ? L"Paired" : L"Unpaired");
        }

        void UpdateDeviceInfo(Windows::Devices::Enumeration::DeviceInformationUpdate const& update)
        {
            deviceInfo.Update(update);
            propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs(L"DisplayName"));
        }

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return propertyChanged.add(handler);
        }
        
        void PropertyChanged(event_token token)
        {
            propertyChanged.remove(token);
        }

    private:
        Windows::Devices::Enumeration::DeviceInformation deviceInfo;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged;
    };
}

namespace winrt::SDKTemplate::implementation
{
    struct ConnectedDevice : ConnectedDeviceT<ConnectedDevice>
    {
        ConnectedDevice(hstring displayNameIn, Windows::Devices::WiFiDirect::WiFiDirectDevice const& wfdDeviceIn, com_ptr<SocketReaderWriter> const& socketRWIn)
            : displayName(std::move(displayNameIn)), wfdDevice(wfdDeviceIn), socketRW(socketRWIn)
        {
        }

        auto WfdDevice() { return wfdDevice; }
        auto DisplayName() { return displayName; }
        auto SocketRW() { return socketRW; }
        void Close()
        {
            wfdDevice.Close();
            socketRW->Close();
        }
    private:
        hstring displayName;
        Windows::Devices::WiFiDirect::WiFiDirectDevice wfdDevice;
        com_ptr<SocketReaderWriter> socketRW;
    };
}

namespace winrt::SDKTemplate::implementation
{
    struct Utils
    {
        Utils() = default;

        static bool CanSendMessage(hstring const& message, Windows::Foundation::IInspectable const& connectedDevice)
        {
            return !message.empty() && connectedDevice;
        }

        static bool IsNonNull(Windows::Foundation::IInspectable const& value)
        {
            return value != nullptr;
        }

        static bool IsNonEmptyString(hstring const& value)
        {
            return !value.empty();
        }
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Utils : UtilsT<Utils, implementation::Utils>
    {
    };
}

