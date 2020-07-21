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
#include "pch.h"
#include "SocketReaderWriter.h"
#include "ConnectedDevice.g.cpp"
#include "Utils.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;

namespace winrt::SDKTemplate
{
    SocketReaderWriter::SocketReaderWriter(StreamSocket socket, MainPage mainPage) :
        streamSocket(socket), rootPage(mainPage),
        dataReader(socket.InputStream()), dataWriter(socket.OutputStream())
    {
        dataReader.UnicodeEncoding(UnicodeEncoding::Utf8);
        dataReader.ByteOrder(ByteOrder::LittleEndian);

        dataWriter.UnicodeEncoding(UnicodeEncoding::Utf8);
        dataWriter.ByteOrder(ByteOrder::LittleEndian);
    }

    void SocketReaderWriter::Close()
    {
        dataReader.Close();
        dataWriter.Close();
        streamSocket.Close();
    }

    IAsyncAction SocketReaderWriter::WriteMessageAsync(hstring const& message)
    {
        auto lifetime = get_strong();

        try
        {
            dataWriter.WriteUInt32(dataWriter.MeasureString(message));
            dataWriter.WriteString(message);
            co_await dataWriter.StoreAsync();
            rootPage.NotifyUser(L"Sent message: " + message, NotifyType::StatusMessage);
        }
        catch (hresult_error const& ex)
        {
            rootPage.NotifyUser(L"WriteMessage raised exception: " + ex.message(), NotifyType::ErrorMessage);
        }
    }

    IAsyncOperation<IReference<hstring>> SocketReaderWriter::ReadMessageAsync()
    {
        auto lifetime = get_strong();

        try
        {
            uint32_t bytesRead = co_await dataReader.LoadAsync(sizeof(uint32_t));
            if (bytesRead > 0)
            {
                // Determine how long the string is.
                uint32_t messageLength = dataReader.ReadUInt32();
                bytesRead = co_await dataReader.LoadAsync(messageLength);
                if (bytesRead > 0)
                {
                    // Decode the string.
                    hstring message = dataReader.ReadString(messageLength);
                    rootPage.NotifyUser(L"Got message: " + message, NotifyType::StatusMessage);
                    co_return message;
                }
            }
        }
        catch (hresult_error const&)
        {
            rootPage.NotifyUser(L"Socket was closed!", NotifyType::StatusMessage);
        }
        co_return nullptr;
    }
}
