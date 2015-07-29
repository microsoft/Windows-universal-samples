//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once
#include "NetworkChannel.h"

namespace Microsoft { namespace Samples { namespace SimpleCommunication { namespace Network {

    ref class CAcceptOperation;

    ref class CNetworkServer sealed: 
        public CNetworkChannel, 
        public INetworkServer
    {
    public:
        virtual ~CNetworkServer();

        // INetworkServer
        virtual Windows::Foundation::IAsyncOperation<Windows::Networking::Sockets::StreamSocketInformation^>^ AcceptAsync();

    internal:
        CNetworkServer(unsigned short listeningPort);

    protected:
        virtual void OnClose() override;

    private:
        unsigned short _listeningPort;
        CAcceptOperation ^_acceptOperation;
    };

    ref class CAcceptOperation sealed
    {
    public:
        virtual ~CAcceptOperation();

    internal:
        CAcceptOperation(CNetworkServer ^parent, CritSec &critSec);
        
        concurrency::task<IStreamSocket^> AcceptAsync(unsigned short port);

    private:
        void Detach();
        void OnConnectionReceived(Windows::Networking::Sockets::StreamSocketListener ^sender, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs ^args);

        CritSec &_critSec;
        IStreamSocketListener ^_listener;
        concurrency::task_completion_event<IStreamSocket^>  _completionEvent;
        Windows::Foundation::EventRegistrationToken _connectionReceivedToken;
        CNetworkServer ^_parent;
        bool _fAsyncOperationInProgress;
    };
}}}}