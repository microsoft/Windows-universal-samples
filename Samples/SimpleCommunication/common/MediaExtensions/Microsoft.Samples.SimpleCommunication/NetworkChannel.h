//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once
#include "StspNetwork.h"

namespace Microsoft { namespace Samples { namespace SimpleCommunication { namespace Network {
    
    ref class CNetworkChannel: public INetworkChannel
    {
    public:
        virtual ~CNetworkChannel();

    internal:
        CNetworkChannel();

        // INetworkChannel
        virtual Windows::Foundation::IAsyncAction ^SendAsync(_In_ IBufferPacket *pPacket) override;
        virtual Windows::Foundation::IAsyncAction ^ReceiveAsync(_In_ IMediaBufferWrapper *pBuffer) override;
        virtual void Close() override;
        virtual void Disconnect() override;

    private protected:

        virtual void OnClose() {}

        void SetSocket(IStreamSocket ^socket) { _socket = socket; }
        IStreamSocket ^GetSocket() { return _socket; }

        void CheckClosed()
        {
            if (_isClosed)
            {
                throw ref new Exception(MF_E_SHUTDOWN);
            }
        }

        CritSec _critSec;                // critical section for thread safety

    private:
        bool _isClosed;
        IStreamSocket ^_socket;
    };
}}}}
