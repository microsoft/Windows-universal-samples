//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once
#include "NetworkChannel.h"

namespace Microsoft { namespace Samples { namespace SimpleCommunication { namespace Network {

    ref class CNetworkClient sealed: 
        public CNetworkChannel, 
        public INetworkClient
    {
    public:
        virtual ~CNetworkClient();

        // INetworkClient
        virtual Windows::Foundation::IAsyncAction ^ConnectAsync(String ^url, WORD wPort);

    internal:
        CNetworkClient();
    };

}}}}