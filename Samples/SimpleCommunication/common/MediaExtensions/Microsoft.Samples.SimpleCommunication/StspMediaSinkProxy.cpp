//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "StspMediaSinkProxy.h"
#include "StspDefs.h"
#include "StspMediaSink.h"

using namespace Microsoft::Samples::SimpleCommunication;
using namespace Microsoft::Samples::SimpleCommunication::Network;

IncomingConnectionEventArgs::IncomingConnectionEventArgs(IStspSinkInternal *pSink, DWORD connectionId, String ^remoteUrl)
    : _spSink(pSink)
    , _connectionId(connectionId)
    , _remoteUrl(remoteUrl)
{
}

// User accepted connection
void IncomingConnectionEventArgs::Accept()
{
    _spSink->TriggerAcceptConnection(_connectionId);
}

// User refused connection
void IncomingConnectionEventArgs::Refuse()
{
    _spSink->TriggerRefuseConnection(_connectionId);
}

StspMediaSinkProxy::StspMediaSinkProxy()
{
}

StspMediaSinkProxy::~StspMediaSinkProxy()
{
    AutoLock lock(_critSec);

    if (_spMediaSink != nullptr)
    {
        _spMediaSink->Shutdown();
        _spMediaSink = nullptr;
    }
}

Windows::Media::IMediaExtension ^StspMediaSinkProxy::GetMFExtensions()
{
    AutoLock lock(_critSec);

    if (_spMediaSink == nullptr)
    {
        Throw(MF_E_NOT_INITIALIZED);
    }

    ComPtr<IInspectable> spInspectable;
    ThrowIfError(_spMediaSink.As(&spInspectable));

    return safe_cast<IMediaExtension^>(reinterpret_cast<Object^>(spInspectable.Get()));
}


Windows::Foundation::IAsyncOperation<IMediaExtension^>^ StspMediaSinkProxy::InitializeAsync(
    Windows::Media::MediaProperties::IMediaEncodingProperties ^audioEncodingProperties,
    Windows::Media::MediaProperties::IMediaEncodingProperties ^videoEncodingProperties
    )
{
    return concurrency::create_async([this, videoEncodingProperties, audioEncodingProperties]()
    {
        AutoLock lock(_critSec);
        CheckShutdown();

        if (_spMediaSink != nullptr)
        {
            Throw(MF_E_ALREADY_INITIALIZED);
        }

        // Prepare the MF extension
        ThrowIfError(MakeAndInitialize<CMediaSink>(&_spMediaSink, ref new StspSinkCallback(this), audioEncodingProperties, videoEncodingProperties));

        ComPtr<IInspectable> spInspectable;
        ThrowIfError(_spMediaSink.As(&spInspectable));

        return safe_cast<IMediaExtension^>(reinterpret_cast<Object^>(spInspectable.Get()));
    });
}

void StspMediaSinkProxy::FireIncomingConnection(IncomingConnectionEventArgs ^args)
{
    IncomingConnectionEvent(this, args);
}

void StspMediaSinkProxy::OnShutdown()
{
    AutoLock lock(_critSec);
    if (_fShutdown)
    {
        return;
    }
    _fShutdown = true;
    _spMediaSink = nullptr;
}
