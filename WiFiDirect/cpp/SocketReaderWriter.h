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
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    extern Platform::Array<byte>^ CustomOui;
    extern byte CustomOuiType;
    extern Platform::Array<byte>^ WfaOui;
    extern Platform::Array<byte>^ MsftOui;
    extern Platform::String^ strServerPort;

    public ref class SocketReaderWriter sealed
    {
    public:
        SocketReaderWriter(Windows::Networking::Sockets::StreamSocket^ socket, SDKTemplate::MainPage^ mainPage) :
            _streamSocket(socket),
            _rootPage(mainPage)
        {
            _socketReader = ref new Windows::Storage::Streams::DataReader(socket->InputStream);
            _socketReader->UnicodeEncoding = Windows::Storage::Streams::UnicodeEncoding::Utf8;
            _socketReader->ByteOrder = Windows::Storage::Streams::ByteOrder::LittleEndian;

            _socketWriter = ref new Windows::Storage::Streams::DataWriter(socket->OutputStream);
            _socketWriter->UnicodeEncoding = Windows::Storage::Streams::UnicodeEncoding::Utf8;
            _socketWriter->ByteOrder = Windows::Storage::Streams::ByteOrder::LittleEndian;

            _currentMessage = nullptr;
        }

        void WriteMessage(Platform::String^ message);
        void ReadMessage();
        void Close();
        Platform::String^ GetCurrentMessage();

    private:
        Windows::Storage::Streams::DataReader^ _socketReader;
        Windows::Storage::Streams::DataWriter^ _socketWriter;
        Windows::Networking::Sockets::StreamSocket^ _streamSocket;
        SDKTemplate::MainPage^ _rootPage;
        Platform::String^ _currentMessage;
    };

    [Windows::UI::Xaml::Data::Bindable]
    public ref class DiscoveredDevice sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        DiscoveredDevice(Windows::Devices::Enumeration::DeviceInformation^ deviceInfoIn);

        property Windows::Devices::Enumeration::DeviceInformation^ DeviceInfo
        {
            Windows::Devices::Enumeration::DeviceInformation^ get() { return deviceInfo; }
        }

        property Platform::String^ DisplayName
        {
            Platform::String^ get() { return deviceInfo->Name; }
        }

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    private:
        Windows::Devices::Enumeration::DeviceInformation^ deviceInfo;
    };

    [Windows::UI::Xaml::Data::Bindable]
    public ref class ConnectedDevice sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
    public:
        ConnectedDevice(Platform::String^ displayName, Windows::Devices::WiFiDirect::WiFiDirectDevice^ wfdDevice, SocketReaderWriter^ socketRW);

        property Platform::String^ DisplayName
        {
            Platform::String^ get() { return displayName; }
        }

        property SocketReaderWriter^ SocketRW
        {
            SocketReaderWriter^ get() { return socketRW; }

            void set(SocketReaderWriter^ socketRW) { this->socketRW = socketRW; }
        }

        property Windows::Devices::WiFiDirect::WiFiDirectDevice^ WfdDevice
        {
            Windows::Devices::WiFiDirect::WiFiDirectDevice^ get() { return wfdDevice; }

            void set(Windows::Devices::WiFiDirect::WiFiDirectDevice^ wfdDevice) { this->wfdDevice = wfdDevice; }
        }

        virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

    private:
        SocketReaderWriter^ socketRW;
        Windows::Devices::WiFiDirect::WiFiDirectDevice^ wfdDevice;
        Platform::String^ displayName;
    };
}