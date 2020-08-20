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
#include "windows.h"
#include <ppltasks.h>
#include <ppl.h>
#include <mutex>

using namespace Concurrency;


namespace VoipBackEnd
{
    namespace TransportMessageType
    {
        enum Value
        {
            Connect = 0,
            Disconnect = 1,
            Audio = 2,
            Video = 3
        };
    }

    public delegate void MessageReceivedEventHandler(Windows::Storage::Streams::IBuffer ^pBuffer, UINT64 hnsPresentationTime, UINT64 hnsSampleDuration);
    public delegate void ConnectionMessageReceivedEventHandler();

    /// <summary>
    /// This is an abstraction of a network transport class
    /// which does not actually send data over the network.
    /// </summary>
    public ref class BackEndTransport sealed
    {
    public:
        // Constructor
        BackEndTransport();

        // Destructor
        virtual ~BackEndTransport();

        void WriteAudio(BYTE* bytes, int byteCount);
        void WriteVideo(BYTE* bytes, int byteCount, UINT64 hnsPresentationTime, UINT64 hnsSampleDuration);

        void Connect(Platform::String^ hostNameStr, Platform::String^ remotePort);
        void Listen(Platform::String^ localPort);

        void Disconnect();

        event MessageReceivedEventHandler^ AudioMessageReceived;
        event MessageReceivedEventHandler^ VideoMessageReceived;

        event ConnectionMessageReceivedEventHandler^ ConnectMessageReceived;
        event ConnectionMessageReceivedEventHandler^ DisconnectMessageReceived;

    private:
        void Write(BYTE* bytes, unsigned int byteCount, TransportMessageType::Value dataType, UINT64 hnsPresentationTime, UINT64 hnsSampleDurationTime);

        void OnConnectionReceived(Windows::Networking::Sockets::DatagramSocket^ socket, Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);
        void OnMessage(Windows::Networking::Sockets::DatagramSocket^ socket, Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);

        Windows::Storage::Streams::DataWriter^ GetWriter();

        bool isConnected;
        bool isListening;

        Windows::Networking::Sockets::DatagramSocket^ listener;
        Windows::Networking::Sockets::DatagramSocket^ socket;
            
        Windows::Storage::Streams::IOutputStream^ outputStream;
        Windows::Storage::Streams::DataWriter^ writer;
            
        const unsigned int headerSize; 

        cancellation_token_source cancelToken;
            
        Windows::Networking::HostName^ hostName;
        Platform::String^ port;
        CRITICAL_SECTION lock;
        CRITICAL_SECTION readLock;
        const unsigned int MaxPacketSize;
    };
}

