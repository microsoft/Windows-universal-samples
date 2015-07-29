//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "NetworkClient.h"
#include "StspDefs.h"

using namespace Microsoft::Samples::SimpleCommunication::Network;

INetworkClient ^Microsoft::Samples::SimpleCommunication::Network::CreateNetworkClient()
{
    return ref new CNetworkClient();
}

CNetworkClient::CNetworkClient()
{
    SetSocket(ref new StreamSocket());
}

CNetworkClient::~CNetworkClient()
{
}

Windows::Foundation::IAsyncAction ^CNetworkClient::ConnectAsync(String ^url, WORD wPort)
{
    if (url == nullptr)
    {
        throw ref new InvalidArgumentException();
    }

    AutoLock lock(_critSec);
    CheckClosed();
    WCHAR szPortNumber[6];
    HostName ^hostName;
    IStreamSocket ^socket = GetSocket();
    WORD port = wPort;
    HStringReference hostNameId(RuntimeClass_Windows_Networking_HostName);

    if (socket == nullptr)
    {
        Throw(MF_E_NOT_INITIALIZED);
    }

    if (port == 0)
    {
        port = c_wStspDefaultPort;
    }
    ThrowIfError(StringCchPrintf(szPortNumber, _countof(szPortNumber), L"%hu", wPort));

    hostName = ref new HostName(url);

    // Start asynchronous operation
    return socket->ConnectAsync(hostName, ref new String(szPortNumber));
}
