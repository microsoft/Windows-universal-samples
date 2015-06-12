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
#include "SocketReaderWriter.h"

using namespace Platform;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace SDKTemplate;
using namespace Windows::Devices::WiFiDirect;
using namespace Windows::Devices::Enumeration;

Platform::Array<byte>^ SDKTemplate::CustomOui = ref new Platform::Array<byte>{ 0xAA, 0xBB, 0xCC };
byte SDKTemplate::CustomOuiType = 0xDD;
Platform::Array<byte>^ SDKTemplate::WfaOui = ref new Platform::Array<byte>{ 0x50, 0x6F, 0x9A };
Platform::Array<byte>^ SDKTemplate::MsftOui = ref new Platform::Array<byte>{ 0x00, 0x50, 0xF2 };
Platform::String^ SDKTemplate::strServerPort = "50001";

void SocketReaderWriter::Close()
{
    delete _socketReader;
    delete _socketWriter;
    delete _streamSocket;
}

void SocketReaderWriter::WriteMessage(String^ message)
{
    _socketWriter->WriteUInt32(_socketWriter->MeasureString(message));
    _socketWriter->WriteString(message);
    concurrency::task<unsigned int> storeTask(_socketWriter->StoreAsync());
    storeTask.then([this, message](concurrency::task<unsigned int> resultTask)
    {
        try
        {
            unsigned int numBytesWritten = resultTask.get();
            if (numBytesWritten > 0)
            {
                _rootPage->NotifyUserFromBackground("Sent message: " + message, NotifyType::StatusMessage);
            }
            else
            {
                _rootPage->NotifyUserFromBackground("The remote side closed the socket", NotifyType::ErrorMessage);
            }
        }
        catch (Exception^ e)
        {
            _rootPage->NotifyUserFromBackground("Failed to send message with error: " + e->Message, NotifyType::ErrorMessage);
        }
    });
}

void SocketReaderWriter::ReadMessage()
{
    concurrency::task<unsigned int> loadTask(_socketReader->LoadAsync(sizeof(UINT32)));
    loadTask.then([this](concurrency::task<unsigned int> stringBytesTask)
    {
        try
        {
            unsigned int bytesRead = stringBytesTask.get();
            if (bytesRead > 0)
            {
                unsigned int strLength = (unsigned int)_socketReader->ReadUInt32();
                concurrency::task<unsigned int> loadStringTask(_socketReader->LoadAsync(strLength));
                loadStringTask.then([this, strLength](concurrency::task<unsigned int> resultTask)
                {
                    try
                    {
                        unsigned int bytesRead = resultTask.get();
                        if (bytesRead > 0)
                        {
                            _currentMessage = _socketReader->ReadString(strLength);
                            _rootPage->NotifyUserFromBackground("Got message: " + _currentMessage, NotifyType::StatusMessage);
                            ReadMessage();
                        }
                        else
                        {
                            _rootPage->NotifyUserFromBackground("The remote side closed the socket", NotifyType::ErrorMessage);
                        }
                    }
                    catch (Exception^ e)
                    {
                        _rootPage->NotifyUserFromBackground("Failed to read from socket: " + e->Message, NotifyType::ErrorMessage);
                    }
                });
            }
            else
            {
                _rootPage->NotifyUserFromBackground("The remote side closed the socket", NotifyType::ErrorMessage);
            }
        }
        catch (Exception^ e)
        {
            _rootPage->NotifyUserFromBackground("Failed to read from socket: " + e->Message, NotifyType::ErrorMessage);
        }
    });
}

String^ SocketReaderWriter::GetCurrentMessage()
{
    return _currentMessage;
}

DiscoveredDevice::DiscoveredDevice(DeviceInformation^ deviceInfoIn)
{
    deviceInfo = deviceInfoIn;
}

ConnectedDevice::ConnectedDevice(String^ displayName, WiFiDirectDevice^ wfdDevice, SocketReaderWriter^ socketRW)
{
    this->socketRW = socketRW;
    this->wfdDevice = wfdDevice;
    this->displayName = displayName;
}