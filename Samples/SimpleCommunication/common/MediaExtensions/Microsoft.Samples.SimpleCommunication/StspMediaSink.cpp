//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include <InitGuid.h>
#include "StspMediaSink.h"
#include "StspStreamSink.h"
#include "StspMediaSinkProxy.h"

using namespace Microsoft::Samples::SimpleCommunication;
using namespace Microsoft::Samples::SimpleCommunication::Network;

namespace
{
    const DWORD c_cbReceiveBuffer = 8 * 1024;

    class ShutdownFunc
    {
    public:
        HRESULT operator()(IMFStreamSink *pStream) const
        {
            static_cast<CStreamSink *>(pStream)->Shutdown();
            return S_OK;
        }
    };

    class SetConnectedFunc
    {
    public:
        SetConnectedFunc(bool fConnected, LONGLONG llStartTime)
            : _fConnected(fConnected)
            , _llStartTime(llStartTime)
        {
        }

        HRESULT operator()(IMFStreamSink *pStream) const
        {
            return static_cast<CStreamSink *>(pStream)->SetConnected(_fConnected, _llStartTime);
        }

        bool _fConnected;
        LONGLONG _llStartTime;
    };

    class StartFunc
    {
    public:
        StartFunc(LONGLONG llStartTime)
            : _llStartTime(llStartTime)
        {
        }

        HRESULT operator()(IMFStreamSink *pStream) const
        {
            return static_cast<CStreamSink *>(pStream)->Start(_llStartTime);
        }

        LONGLONG _llStartTime;
    };

    class StopFunc
    {
    public:
        HRESULT operator()(IMFStreamSink *pStream) const
        {
            return static_cast<CStreamSink *>(pStream)->Stop();
        }
    };

    template <class T, class TFunc>
    HRESULT ForEach(ComPtrList<T> &col, TFunc fn)
    {
        ComPtrList<T>::POSITION pos = col.FrontPosition();
        ComPtrList<T>::POSITION endPos = col.EndPosition();
        HRESULT hr = S_OK;

        for (;pos != endPos; pos = col.Next(pos))
        {
            ComPtr<T> spStream;

            hr = col.GetItemPos(pos, &spStream);
            if (FAILED(hr))
            {
                break;
            }

            hr = fn(spStream.Get());
        }

        return hr;
    }

    static void AddAttribute(_In_ GUID guidKey, _In_ IPropertyValue ^value, _In_ IMFAttributes *pAttr)
    {
        PropertyType type = value->Type;
        switch (type)
        {
            case PropertyType::UInt8Array:
            {
                Array<BYTE>^ arr;
                value->GetUInt8Array(&arr);
                
                ThrowIfError(pAttr->SetBlob(guidKey, arr->Data, arr->Length));
            }
            break;

            case PropertyType::Double:
            {
                ThrowIfError(pAttr->SetDouble(guidKey, value->GetDouble()));
            }
            break;

            case PropertyType::Guid:
            {
                ThrowIfError(pAttr->SetGUID(guidKey, value->GetGuid()));
            }
            break;

            case PropertyType::String:
            {
                ThrowIfError(pAttr->SetString(guidKey, value->GetString()->Data()));
            }
            break;

            case PropertyType::UInt32:
            {
                ThrowIfError(pAttr->SetUINT32(guidKey, value->GetUInt32()));
            }
            break;

            case PropertyType::UInt64:
            {
                ThrowIfError(pAttr->SetUINT64(guidKey, value->GetUInt64()));
            }
            break;

            // ignore unknown values
        }
    }

    void ConvertPropertiesToMediaType(_In_ IMediaEncodingProperties ^mep, _Outptr_ IMFMediaType **ppMT)
    {
        if (mep == nullptr || ppMT == nullptr)
        {
            throw ref new InvalidArgumentException();
        }
        ComPtr<IMFMediaType> spMT;
        *ppMT = nullptr;
        ThrowIfError(MFCreateMediaType(&spMT));

        auto it = mep->Properties->First();            

        while (it->HasCurrent)
        {
            auto currentValue = it->Current;
            AddAttribute(currentValue->Key, safe_cast<IPropertyValue^>(currentValue->Value), spMT.Get());
            it->MoveNext();
        }

        GUID guiMajorType = safe_cast<IPropertyValue^>(mep->Properties->Lookup(MF_MT_MAJOR_TYPE))->GetGuid();

        if (guiMajorType != MFMediaType_Video && guiMajorType != MFMediaType_Audio)
        {
            Throw(E_UNEXPECTED);
        }

        *ppMT = spMT.Detach();
    }

    DWORD GetStreamId(Windows::Media::Capture::MediaStreamType mediaStreamType)
    {
        switch(mediaStreamType)
        {
        case Windows::Media::Capture::MediaStreamType::VideoRecord:
            return 0;
        case Windows::Media::Capture::MediaStreamType::Audio:
            return 1;
        }

        throw ref new InvalidArgumentException();
    }
}

CMediaSink::CMediaSink() 
: _cRef(1)
, _IsShutdown(false)
, _IsConnected(false)
, _llStartTime(0)
, _cStreamsEnded(0)
, _waitingConnectionId(0)
{
}

CMediaSink::~CMediaSink()
{
    assert(_IsShutdown);
}

HRESULT CMediaSink::RuntimeClassInitialize(
    ISinkCallback ^callback,
    Windows::Media::MediaProperties::IMediaEncodingProperties ^audioEncodingProperties,
    Windows::Media::MediaProperties::IMediaEncodingProperties ^videoEncodingProperties
    )
{
    try
    {
        _callback = callback;
        // Create network server
        INetworkServer ^server = CreateNetworkServer(c_wStspDefaultPort);
        _networkSender = safe_cast<INetworkChannel^>(server);

        // Set up media streams
        SetMediaStreamProperties(MediaStreamType::Audio, audioEncodingProperties);
        SetMediaStreamProperties(MediaStreamType::VideoRecord, videoEncodingProperties);

        // Start listening from the connection from the client.
        StartListening();
    }
    catch (Exception ^exc)
    {
        _callback = nullptr;
        if (_networkSender != nullptr)
        {
            _networkSender->Close();
        }
        _networkSender = nullptr;
        return exc->HResult;
    }

    return S_OK;
}

void CMediaSink::SetMediaStreamProperties(
    Windows::Media::Capture::MediaStreamType MediaStreamType,
    _In_opt_ Windows::Media::MediaProperties::IMediaEncodingProperties ^mediaEncodingProperties)
{
    if (MediaStreamType != MediaStreamType::VideoRecord && MediaStreamType != MediaStreamType::Audio)
    {
        throw ref new InvalidArgumentException();
    }

    RemoveStreamSink(GetStreamId(MediaStreamType));
    const unsigned int streamId = GetStreamId(MediaStreamType);

    if (mediaEncodingProperties != nullptr)
    {
        ComPtr<IMFStreamSink> spStreamSink;
        ComPtr<IMFMediaType> spMediaType;
        ConvertPropertiesToMediaType(mediaEncodingProperties, &spMediaType);
        ThrowIfError(AddStreamSink(streamId, spMediaType.Get(), spStreamSink.GetAddressOf()));
    }
}

//   IStspSinkInternal
void CMediaSink::TriggerAcceptConnection(DWORD connectionId)
{
    try
    {
        AutoLock lock(_critSec);
        ThrowIfError(CheckShutdown());

        if (_waitingConnectionId != 0 && connectionId == _waitingConnectionId)
        {
            _waitingConnectionId = 0;
            ComPtr<CMediaSink> spThis = this;
            concurrency::create_task([this, spThis]
            {
                AutoLock lock(_critSec);
                try
                {
                    ThrowIfError(CheckShutdown());

                    // Create receive buffer
                    ThrowIfError(CreateMediaBufferWrapper(c_cbReceiveBuffer, &_spReceiveBuffer));

                    // Start receiving
                    StartReceiving(_spReceiveBuffer.Get());
                }
                catch(Exception ^exc)
                {
                    HandleError(exc->HResult);
                }
            });
        }
    }
    catch(Exception^)
    {
    }
}

void CMediaSink::TriggerRefuseConnection(DWORD connectionId)
{
    try
    {
        AutoLock lock(_critSec);
        ThrowIfError(CheckShutdown());

        if (_waitingConnectionId != 0 && connectionId == _waitingConnectionId)
        {
            _waitingConnectionId = 0;
            ComPtr<CMediaSink> spThis;
            concurrency::create_task([this, spThis]
            {
                AutoLock lock(_critSec);
                try
                {
                    ThrowIfError(CheckShutdown());

                    // Connection refused disconnect current client.
                    _networkSender->Disconnect();

                    StartListening();
                }
                catch(Exception ^exc)
                {
                    HandleError(exc->HResult);
                }
            });
        }
    }
    catch(Exception^)
    {
    }
}

///  IMFMediaSink
IFACEMETHODIMP CMediaSink::GetCharacteristics(DWORD *pdwCharacteristics)
{
    if (pdwCharacteristics == NULL)
    {
        return E_INVALIDARG;
    }
    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        // Rateless sink.
        *pdwCharacteristics = MEDIASINK_RATELESS;
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::AddStreamSink(
    DWORD dwStreamSinkIdentifier,
    IMFMediaType *pMediaType,
    IMFStreamSink **ppStreamSink)
{
    CStreamSink *pStream = nullptr;
    ComPtr<IMFStreamSink> spMFStream;
    AutoLock lock(_critSec);
    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = GetStreamSinkById(dwStreamSinkIdentifier, &spMFStream);
    }

    if (SUCCEEDED(hr))
    {
        hr = MF_E_STREAMSINK_EXISTS;
    }
    else
    {
        hr = S_OK;
    }

    if (SUCCEEDED(hr))
    {
        pStream = new CStreamSink(dwStreamSinkIdentifier);
        if (pStream == nullptr)
        {
            hr = E_OUTOFMEMORY;
        }
        spMFStream.Attach(pStream);
    }

    // Initialize the stream.
    if (SUCCEEDED(hr))
    {
        hr = pStream->Initialize(this, _networkSender);
    }

    if (SUCCEEDED(hr) && pMediaType != nullptr)
    {
        hr = pStream->SetCurrentMediaType(pMediaType);
    }

    if (SUCCEEDED(hr))
    {
        StreamContainer::POSITION pos = _streams.FrontPosition();
        StreamContainer::POSITION posEnd = _streams.EndPosition();

        // Insert in proper position
        for (; pos != posEnd; pos = _streams.Next(pos))
        {
            DWORD dwCurrId;
            ComPtr<IMFStreamSink> spCurr;
            hr = _streams.GetItemPos(pos, &spCurr);
            if (FAILED(hr))
            {
                break;
            }
            hr = spCurr->GetIdentifier(&dwCurrId);
            if (FAILED(hr))
            {
                break;
            }

            if (dwCurrId > dwStreamSinkIdentifier)
            {
                break;
            }
        }

        if (SUCCEEDED(hr))
        {
            hr = _streams.InsertPos(pos, pStream);
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppStreamSink = spMFStream.Detach();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::RemoveStreamSink(DWORD dwStreamSinkIdentifier)
{
    AutoLock lock(_critSec);
    HRESULT hr = CheckShutdown();
    StreamContainer::POSITION pos = _streams.FrontPosition();
    StreamContainer::POSITION endPos = _streams.EndPosition();
    ComPtr<IMFStreamSink> spStream;

    if (SUCCEEDED(hr))
    {
        for (;pos != endPos; pos = _streams.Next(pos))
        {
            hr = _streams.GetItemPos(pos, &spStream);
            DWORD dwId;

            if (FAILED(hr))
            {
                break;
            }

            hr = spStream->GetIdentifier(&dwId);
            if (FAILED(hr) || dwId == dwStreamSinkIdentifier)
            {
                break;
            }
        }

        if (pos == endPos)
        {
            hr = MF_E_INVALIDSTREAMNUMBER;
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = _streams.Remove(pos, nullptr);
        static_cast<CStreamSink *>(spStream.Get())->Shutdown();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::GetStreamSinkCount(_Out_ DWORD *pcStreamSinkCount)
{
    if (pcStreamSinkCount == NULL)
    {
        return E_INVALIDARG;
    }

    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        *pcStreamSinkCount = _streams.GetCount();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::GetStreamSinkByIndex(
DWORD dwIndex,
_Outptr_ IMFStreamSink **ppStreamSink)
{
    if (ppStreamSink == NULL)
    {
        return E_INVALIDARG;
    }

    ComPtr<IMFStreamSink> spStream;
    AutoLock lock(_critSec);
    DWORD cStreams = _streams.GetCount();

    if (dwIndex >= cStreams)
    {
        return MF_E_INVALIDINDEX;
    }

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        StreamContainer::POSITION pos = _streams.FrontPosition();
        StreamContainer::POSITION endPos = _streams.EndPosition();
        DWORD dwCurrent = 0;

        for (;pos != endPos && dwCurrent < dwIndex; pos = _streams.Next(pos), ++dwCurrent)
        {
            // Just move to proper position
        }

        if (pos == endPos)
        {
            hr = MF_E_UNEXPECTED;
        }
        else
        {
            hr = _streams.GetItemPos(pos, &spStream);
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppStreamSink = spStream.Detach();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::GetStreamSinkById(
DWORD dwStreamSinkIdentifier,
IMFStreamSink **ppStreamSink)
{
    if (ppStreamSink == NULL)
    {
        return E_INVALIDARG;
    }

    AutoLock lock(_critSec);
    HRESULT hr = CheckShutdown();
    ComPtr<IMFStreamSink> spResult;

    if (SUCCEEDED(hr))
    {
        StreamContainer::POSITION pos = _streams.FrontPosition();
        StreamContainer::POSITION endPos = _streams.EndPosition();

        for (;pos != endPos; pos = _streams.Next(pos))
        {
            ComPtr<IMFStreamSink> spStream;
            hr = _streams.GetItemPos(pos, &spStream);
            DWORD dwId;

            if (FAILED(hr))
            {
                break;
            }

            hr = spStream->GetIdentifier(&dwId);
            if (FAILED(hr))
            {
                break;
            }
            else if (dwId == dwStreamSinkIdentifier)
            {
                spResult = spStream;
                break;
            }
        }

        if (pos == endPos)
        {
            hr = MF_E_INVALIDSTREAMNUMBER;
        }
    }

    if (SUCCEEDED(hr))
    {
        assert(spResult);
        *ppStreamSink = spResult.Detach();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::SetPresentationClock(IMFPresentationClock *pPresentationClock)
{
    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    // If we already have a clock, remove ourselves from that clock's
    // state notifications.
    if (SUCCEEDED(hr))
    {
        if (_spClock)
        {
            hr = _spClock->RemoveClockStateSink(this);
        }
    }

    // Register ourselves to get state notifications from the new clock.
    if (SUCCEEDED(hr))
    {
        if (pPresentationClock)
        {
            hr = pPresentationClock->AddClockStateSink(this);
        }
    }

    if (SUCCEEDED(hr))
    {
        // Release the pointer to the old clock.
        // Store the pointer to the new clock.
        _spClock = pPresentationClock;
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::GetPresentationClock(IMFPresentationClock **ppPresentationClock)
{
    if (ppPresentationClock == NULL)
    {
        return E_INVALIDARG;
    }

    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (_spClock == NULL)
        {
            hr = MF_E_NO_CLOCK; // There is no presentation clock.
        }
        else
        {
            // Return the pointer to the caller.
            *ppPresentationClock = _spClock.Get();
            (*ppPresentationClock)->AddRef();
        }
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::Shutdown()
{
    ISinkCallback ^callback;
    {
        AutoLock lock(_critSec);

        HRESULT hr = CheckShutdown();

        if (SUCCEEDED(hr))
        {
            ForEach(_streams, ShutdownFunc());
            _streams.Clear();

            if (_networkSender != nullptr)
            {
                _networkSender->Close();
            }

            _networkSender = nullptr;
            _spClock.Reset();

            _IsShutdown = true;
            callback = _callback;
        }
    }

    if (callback != nullptr)
    {
        callback->OnShutdown();
    }

    return S_OK;
}

// IMFClockStateSink
IFACEMETHODIMP CMediaSink::OnClockStart(
                                        MFTIME hnsSystemTime,
                                        LONGLONG llClockStartOffset)
{
    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        TRACE(TRACE_LEVEL_LOW, L"OnClockStart ts=%I64d\n", llClockStartOffset);
        // Start each stream.
        _llStartTime = llClockStartOffset;
        hr = ForEach(_streams, StartFunc(llClockStartOffset));
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSink::OnClockStop(
                                       MFTIME hnsSystemTime)
{
    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        // Stop each stream
        hr = ForEach(_streams, StopFunc());
    }

    TRACEHR_RET(hr);
}


IFACEMETHODIMP CMediaSink::OnClockPause(
                                        MFTIME hnsSystemTime)
{
    return MF_E_INVALID_STATE_TRANSITION;
}

IFACEMETHODIMP CMediaSink::OnClockRestart(
MFTIME hnsSystemTime)
{
    return MF_E_INVALID_STATE_TRANSITION;
}

IFACEMETHODIMP CMediaSink::OnClockSetRate(
/* [in] */ MFTIME hnsSystemTime,
/* [in] */ float flRate)
{
    return S_OK;
}

void CMediaSink::ReportEndOfStream()
{
    AutoLock lock(_critSec);
    ++_cStreamsEnded;
}

/// Private methods
// Start listening on the network server
void CMediaSink::StartListening()
{
    ComPtr<CMediaSink> spThis = this;
    concurrency::create_task(safe_cast<INetworkServer^>(_networkSender)->AcceptAsync()).then([spThis, this](concurrency::task<StreamSocketInformation^>& acceptTask)
    {
        IncomingConnectionEventArgs ^args;
        ISinkCallback ^callback;
        try
        {
            {
                AutoLock lock(_critSec);
                auto info = acceptTask.get();
                ThrowIfError(CheckShutdown());
                if (_callback == nullptr)
                {
                    Throw(E_UNEXPECTED);
                }
                _remoteUrl = PrepareRemoteUrl(info);

                _waitingConnectionId = LODWORD(GetTickCount64());
                if (_waitingConnectionId == 0) 
                {
                    ++_waitingConnectionId;
                }

                args = ref new IncomingConnectionEventArgs(this, _waitingConnectionId, _remoteUrl);
                callback = _callback;
            }

            callback->FireIncomingConnection(args);
        }
        catch(Exception ^exc)
        {
            AutoLock lock(_critSec);
            HandleError(exc->HResult);
        }
    });    
}

void CMediaSink::StartReceiving(IMediaBufferWrapper *pReceiveBuffer)
{
    ComPtr<CMediaSink> spThis = this;
    ComPtr<IMediaBufferWrapper> spReceiveBuffer = pReceiveBuffer;

    concurrency::create_task(_networkSender->ReceiveAsync(pReceiveBuffer)).then([spThis, spReceiveBuffer, this](concurrency::task<void>& task)
    {
        AutoLock lock(_critSec);
        try
        {
            StspOperation eOp = StspOperation_Unknown;
            task.get();
            ThrowIfError(CheckShutdown());

            BYTE *pBuf = spReceiveBuffer->GetBuffer();
            DWORD cbCurrentLen;
            spReceiveBuffer->GetMediaBuffer()->GetCurrentLength(&cbCurrentLen);

            // Validate if the data received from the client is sufficient to fit operation header which is the smallest size of message that we can handle.
            if (cbCurrentLen != sizeof(StspOperationHeader))
            {
                Throw(MF_E_INVALID_FORMAT);
            }

            StspOperationHeader *pOpHeader = reinterpret_cast<StspOperationHeader *>(pBuf);
            // We only support client's request for media description
            if (pOpHeader->cbDataSize != 0 ||
                ((pOpHeader->eOperation != StspOperation_ClientRequestDescription) &&
                (pOpHeader->eOperation != StspOperation_ClientRequestStart) &&
                (pOpHeader->eOperation != StspOperation_ClientRequestStop)))
            {
                Throw(MF_E_INVALID_FORMAT);
            }
            else
            {
                eOp = pOpHeader->eOperation;
            }

            switch(eOp)
            {
            case StspOperation_ClientRequestDescription:
                // Send description to the client
                SendDescription();
                break;
            case StspOperation_ClientRequestStart:
                {
                    _IsConnected = true;
                    if (_spClock)
                    {
                        ThrowIfError(_spClock->GetTime(&_llStartTime));
                    }

                    // We are now connected we can start streaming.
                    ForEach(_streams, SetConnectedFunc(true, _llStartTime));
                }
                break;
            default:
                Throw(MF_E_INVALID_FORMAT);
                break;
            }
        }
        catch(Exception ^exc)
        {
            HandleError(exc->HResult);
        }
    });
}

// Send packet
concurrency::task<void> CMediaSink::SendPacket(Network::IBufferPacket *pPacket)
{
    return concurrency::create_task(_networkSender->SendAsync(pPacket));
}

// Send media description to the client
void CMediaSink::SendDescription()
{
    // Size of the description buffer
    const DWORD c_cStreams = _streams.GetCount();
    const DWORD c_cbDescriptionSize = sizeof(StspDescription) + (c_cStreams-1) * sizeof(StspStreamDescription);
    const DWORD c_cbPacketSize = sizeof(StspOperationHeader) + c_cbDescriptionSize;

    ComPtr<IMediaBufferWrapper> spBuffer;
    ComPtr<IMediaBufferWrapper> *arrspAttributes = new ComPtr<IMediaBufferWrapper>[c_cStreams];
    if (arrspAttributes == nullptr)
    {
        Throw(E_OUTOFMEMORY);
    }

    try
    {
        // Create send buffer
        ThrowIfError(CreateMediaBufferWrapper(c_cbPacketSize, &spBuffer));

        // Prepare operation header
        BYTE *pBuf = spBuffer->GetBuffer();
        StspOperationHeader *pOpHeader = reinterpret_cast<StspOperationHeader *>(pBuf);
        pOpHeader->cbDataSize = c_cbDescriptionSize;
        pOpHeader->eOperation = StspOperation_ServerDescription;

        // Prepare description
        StspDescription *pDescription = reinterpret_cast<StspDescription *>(pBuf + sizeof(StspOperationHeader));
        pDescription->cNumStreams = c_cStreams;

        StreamContainer::POSITION pos = _streams.FrontPosition();
        StreamContainer::POSITION endPos = _streams.EndPosition();
        DWORD nStream = 0;
        for (;pos != endPos; pos = _streams.Next(pos), ++nStream)
        {
            ComPtr<IMFStreamSink> spStream;
            ThrowIfError(_streams.GetItemPos(pos, &spStream));

            // Fill out stream description
            arrspAttributes[nStream] = FillStreamDescription(static_cast<CStreamSink *>(spStream.Get()), &pDescription->aStreams[nStream]);

            // Add size of variable size attribute blob to size of the package.
            pOpHeader->cbDataSize += pDescription->aStreams[nStream].cbAttributesSize;
        }

        // Set length of the packet
        ThrowIfError(spBuffer->SetCurrentLength(c_cbPacketSize));

        // Prepare packet to send
        ComPtr<IBufferPacket> spPacket;

        ThrowIfError(CreateBufferPacket(&spPacket));
        // Add fixed size header and description to the packet
        ThrowIfError(spPacket->AddBuffer(spBuffer.Get()));

        for (DWORD nStream = 0; nStream < c_cStreams; ++nStream)
        {
            // Add variable size attributes.
            ThrowIfError(spPacket->AddBuffer(arrspAttributes[nStream].Get()));
        }

        ComPtr<CMediaSink> spThis = this;
        // Send the data.
        SendPacket(spPacket.Get()).then([this, spThis](concurrency::task<void>& task)
        {
            try
            {
                task.get();
            }
            catch(Exception ^exc)
            {
                AutoLock lock(_critSec);
                HandleError(exc->HResult);
            }
        });

        // Keep receiving
        StartReceiving(_spReceiveBuffer.Get());

        delete[] arrspAttributes;
    }
    catch(Exception ^exc)
    {
        delete[] arrspAttributes;
        throw;
    }
}

// Fill stream description and prepare attributes blob.
ComPtr<Network::IMediaBufferWrapper> CMediaSink::FillStreamDescription(CStreamSink *pStream, StspStreamDescription *pStreamDescription)
{
    assert(pStream != nullptr);
    assert(pStreamDescription != nullptr);

    ComPtr<IMFMediaType> spMediaType;

    // Get current media type
    ThrowIfError(pStream->GetCurrentMediaType(&spMediaType));

    return pStream->FillStreamDescription(spMediaType.Get(), pStreamDescription);
}

void CMediaSink::HandleError(HRESULT hr)
{
    Shutdown();
}

String ^CMediaSink::PrepareRemoteUrl(StreamSocketInformation ^info)
{
    WCHAR szBuffer[MAX_PATH];

    if ( info->RemoteHostName->Type == HostNameType::Ipv4 || info->RemoteHostName->Type == HostNameType::DomainName)
    {
        ThrowIfError(StringCchPrintf(szBuffer, _countof(szBuffer), L"%s://%s", c_szStspScheme, info->RemoteHostName->RawName->Data()));
    }
    else if (info->RemoteHostName->Type == HostNameType::Ipv6)
    {
        ThrowIfError(StringCchPrintf(szBuffer, _countof(szBuffer), L"%s://[%s]", c_szStspScheme, info->RemoteHostName->RawName->Data()));
    }
    else
    {
        throw ref new InvalidArgumentException();
    }

    return ref new String(szBuffer);
}
