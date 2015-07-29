//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "NetworkServer.h"
#include "StspDefs.h"

using namespace Microsoft::Samples::SimpleCommunication::Network;

INetworkServer ^Microsoft::Samples::SimpleCommunication::Network::CreateNetworkServer(unsigned short listeningPort)
{
    return ref new CNetworkServer(listeningPort);
}

CNetworkServer::CNetworkServer(unsigned short listeningPort)
    : _listeningPort(listeningPort)
{
}

CNetworkServer::~CNetworkServer()
{
    OnClose();
}

Windows::Foundation::IAsyncOperation<Windows::Networking::Sockets::StreamSocketInformation^>^ CNetworkServer::AcceptAsync()
{
    return concurrency::create_async([this](){
        AutoLock lock(_critSec);

        try
        {
            if (_acceptOperation != nullptr)
            {
                // We only support accepting one connection.
                Throw(MF_E_INVALIDREQUEST);
            }

            _acceptOperation = ref new CAcceptOperation(this, _critSec);
            return concurrency::create_task(_acceptOperation->AcceptAsync(_listeningPort)).then([this](concurrency::task<IStreamSocket^>& acceptTask)
            {
                try
                {
                    auto clientSocket = acceptTask.get();                        

                    SetSocket(clientSocket);

                    return clientSocket->Information;
                }
                catch (Exception ^exc)
                {
                    if (_acceptOperation != nullptr)
                    {
                        delete _acceptOperation;
                        _acceptOperation = nullptr;
                    }
                    throw;
                }
            });
        }
        catch (Exception ^exc)
        {
            if (_acceptOperation != nullptr)
            {
                delete _acceptOperation;
                _acceptOperation = nullptr;
            }
            throw;
        }
    });
}

void CNetworkServer::OnClose()
{
    if (_acceptOperation != nullptr)
    {
        delete _acceptOperation;
        _acceptOperation = nullptr;
    }
}

CAcceptOperation::CAcceptOperation(CNetworkServer ^parent, CritSec &critSec)
    : _parent(parent)
    , _critSec(critSec)
    , _fAsyncOperationInProgress(false)
{
}

CAcceptOperation::~CAcceptOperation()
{
    AutoLock lock(_critSec);
    Detach();
    if (_fAsyncOperationInProgress && !_completionEvent._IsTriggered())
    {
        _completionEvent.set_exception(ref new DisconnectedException());
    }
}

concurrency::task<IStreamSocket^> CAcceptOperation::AcceptAsync(unsigned short port)
{
    AutoLock lock(_critSec);
    if (_listener != nullptr)
    {
        Throw(MF_E_INVALIDREQUEST);
    }
    WCHAR szPortNumber[6];
    if (port == 0)
    {
        port = c_wStspDefaultPort;
    }

    ThrowIfError(StringCchPrintf(szPortNumber, _countof(szPortNumber), L"%hu", port));
    _listener = ref new StreamSocketListener();
    _connectionReceivedToken = _listener->ConnectionReceived += ref new Windows::Foundation::TypedEventHandler<Windows::Networking::Sockets::StreamSocketListener ^, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs ^>(this, &Microsoft::Samples::SimpleCommunication::Network::CAcceptOperation::OnConnectionReceived);
    concurrency::create_task(_listener->BindServiceNameAsync(ref new String(szPortNumber)));
    auto &result = concurrency::create_task(_completionEvent);
    _fAsyncOperationInProgress = true;
    return result;
}

void CAcceptOperation::OnConnectionReceived(Windows::Networking::Sockets::StreamSocketListener ^sender, Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs ^args)
{
    AutoLock lock(_critSec);
    Detach();
    if (!_completionEvent.set(args->Socket))
    {
        delete args->Socket;
    }
}

void CAcceptOperation::Detach()
{
    _parent = nullptr;
    delete _listener;
    _listener = nullptr;
}
