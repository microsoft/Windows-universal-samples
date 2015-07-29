//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "StspMediaSource.h"
#include "StspMediaStream.h"
#include <IntSafe.h>

using namespace Microsoft::Samples::SimpleCommunication;
using namespace Microsoft::Samples::SimpleCommunication::Network;

namespace
{
    const DWORD c_cbReceiveBufferSize = 2 * 1024;
    const DWORD c_cbMaxPacketSize = 1024 * 1024;
};

CSourceOperation::CSourceOperation(CSourceOperation::Type opType)
: _cRef(1)
, _opType(opType)
{
    ZeroMemory(&_data, sizeof(_data));
}

CSourceOperation::~CSourceOperation()
{
    PropVariantClear(&_data);
}

// IUnknown methods
IFACEMETHODIMP CSourceOperation::QueryInterface(REFIID riid, void **ppv)
{
    return E_NOINTERFACE;
}

IFACEMETHODIMP_(ULONG) CSourceOperation::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CSourceOperation::Release()
{
    long cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

HRESULT CSourceOperation::SetData(const PROPVARIANT &varData)
{
    return PropVariantCopy(&_data, &varData);
}

CStartOperation::CStartOperation(IMFPresentationDescriptor *pPD)
: CSourceOperation(CSourceOperation::Operation_Start)
, _spPD(pPD)
{
}

CStartOperation::~CStartOperation()
{
}

CSetRateOperation::CSetRateOperation(BOOL fThin, float flRate)
: CSourceOperation(CSourceOperation::Operation_SetRate)
, _fThin(fThin)
, _flRate(flRate)
{
}

CSetRateOperation::~CSetRateOperation()
{
}

CMediaSource::CMediaSource(void)
: OpQueue<CMediaSource, CSourceOperation>(_critSec.m_criticalSection)
, _cRef(1)
, _eSourceState(SourceState_Invalid)
, _serverPort(0)
, _flRate(1.0f)
{
    ZeroMemory(&_CurrentReceivedOperationHeader, sizeof(_CurrentReceivedOperationHeader));
}

CMediaSource::~CMediaSource(void)
{
}

HRESULT CMediaSource::CreateInstance(CMediaSource **ppNetSource)
{
    HRESULT hr = S_OK;

    try
    {
        if (ppNetSource == nullptr)
        {
            Throw(E_INVALIDARG);
        }

        ComPtr<CMediaSource> spSource;
        spSource.Attach(new (std::nothrow) CMediaSource());
        if (!spSource)
        {
            Throw(E_OUTOFMEMORY);
        }

        spSource->Initialize();

        *ppNetSource = spSource.Detach();
    }
    catch(Exception ^exc)
    {
        hr = exc->HResult;
    }

    TRACEHR_RET(hr);
}

// IUnknown methods

IFACEMETHODIMP CMediaSource::QueryInterface(REFIID riid, void **ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }
    (*ppv) = nullptr;

    HRESULT hr = S_OK;
    if (riid == IID_IUnknown || 
        riid == IID_IMFMediaSource ||
        riid == IID_IMFMediaEventGenerator) 
    {
        (*ppv) = static_cast<IMFMediaSource*>(this);
        AddRef();
    }
    else if (riid == IID_IMFAttributes)
    {
        (*ppv) = static_cast<IMFAttributes*>(this);
        AddRef();
    }
    else if (riid == IID_IMFGetService)
    {
        (*ppv) = static_cast<IMFGetService*>(this);
        AddRef();
    }
    else if (riid == IID_IMFRateControl)
    {
        (*ppv) = static_cast<IMFRateControl*>(this);
        AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

IFACEMETHODIMP_(ULONG) CMediaSource::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CMediaSource::Release()
{
    long cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}

// IMFMediaEventGenerator methods.
// Note: These methods call through to the event queue helper object.
IFACEMETHODIMP CMediaSource::BeginGetEvent(IMFAsyncCallback *pCallback, IUnknown *punkState)
{
    HRESULT hr = S_OK;

    AutoLock lock(_critSec);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = _spEventQueue->BeginGetEvent(pCallback, punkState);
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::EndGetEvent(IMFAsyncResult *pResult, IMFMediaEvent **ppEvent)
{
    HRESULT hr = S_OK;

    AutoLock lock(_critSec);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = _spEventQueue->EndGetEvent(pResult, ppEvent);
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::GetEvent(DWORD dwFlags, IMFMediaEvent **ppEvent)
{
    // NOTE:
    // GetEvent can block indefinitely, so we don't hold the lock.
    // This requires some juggling with the event queue pointer.

    HRESULT hr = S_OK;

    ComPtr<IMFMediaEventQueue> spQueue;

    {
        AutoLock lock(_critSec);

        // Check shutdown
        hr = CheckShutdown();

        // Get the pointer to the event queue.
        if (SUCCEEDED(hr))
        {
            spQueue = _spEventQueue;
        }
    }

    // Now get the event.
    if (SUCCEEDED(hr))
    {
        hr = spQueue->GetEvent(dwFlags, ppEvent);
    }


    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, PROPVARIANT const *pvValue)
{
    HRESULT hr = S_OK;

    AutoLock lock(_critSec);

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = _spEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
    }

    TRACEHR_RET(hr);
}

// IMFMediaSource methods
IFACEMETHODIMP CMediaSource::CreatePresentationDescriptor(
IMFPresentationDescriptor **ppPresentationDescriptor
)
{
    if (ppPresentationDescriptor == NULL)
    {
        return E_POINTER;
    }

    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr) &&
        (_eSourceState == SourceState_Opening || _eSourceState == SourceState_Invalid || !_spPresentationDescriptor))
    {
        hr = MF_E_NOT_INITIALIZED;
    }

    if (SUCCEEDED(hr))
    {
        hr = _spPresentationDescriptor->Clone(ppPresentationDescriptor);
    }

    return hr;
}

IFACEMETHODIMP CMediaSource::GetCharacteristics(DWORD *pdwCharacteristics)
{
    if (pdwCharacteristics == NULL)
    {
        return E_POINTER;
    }

    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        *pdwCharacteristics = MFMEDIASOURCE_IS_LIVE;
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::Pause()
{
    return MF_E_INVALID_STATE_TRANSITION;
}

IFACEMETHODIMP CMediaSource::Shutdown()
{
    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (_spEventQueue != nullptr)
        {
            _spEventQueue->Shutdown();
        }
        if (_networkSender != nullptr)
        {
            _networkSender->Close();
        }

        StreamContainer::POSITION pos = _streams.FrontPosition();
        while (pos != _streams.EndPosition())
        {
            ComPtr<IMFMediaStream> spStream;
            hr = _streams.GetItemPos(pos, &spStream);
            pos = _streams.Next(pos);
            if (SUCCEEDED(hr))
            {
                static_cast<CMediaStream*>(spStream.Get())->Shutdown();
            }
        }

        _eSourceState = SourceState_Shutdown;

        _streams.Clear();

        _spEventQueue.Reset();
        _networkSender = nullptr;
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::Start(
                                   IMFPresentationDescriptor *pPresentationDescriptor,
                                   const GUID *pguidTimeFormat,
                                   const PROPVARIANT *pvarStartPos
                                   )
{
    HRESULT hr = S_OK;

    // Check parameters.

    // Start position and presentation descriptor cannot be NULL.
    if (pvarStartPos == nullptr || pPresentationDescriptor == nullptr)
    {
        return E_INVALIDARG;
    }

    // Check the time format.
    if ((pguidTimeFormat != nullptr) && (*pguidTimeFormat != GUID_NULL))
    {
        // Unrecognized time format GUID.
        return MF_E_UNSUPPORTED_TIME_FORMAT;
    }

    // Check the data type of the start position.
    if (pvarStartPos->vt != VT_EMPTY && pvarStartPos->vt != VT_I8)
    {
        return MF_E_UNSUPPORTED_TIME_FORMAT;
    }

    AutoLock lock(_critSec);
    ComPtr<CStartOperation> spStartOp;

    if (_eSourceState != SourceState_Stopped &&
        _eSourceState != SourceState_Started)
    {
        hr = MF_E_INVALIDREQUEST;
    }

    if (SUCCEEDED(hr))
    {
        // Check if the presentation description is valid.
        hr = ValidatePresentationDescriptor(pPresentationDescriptor);
    }

    if (SUCCEEDED(hr))
    {
        // Prepare asynchronous operation attributes
        spStartOp.Attach(new (std::nothrow) CStartOperation(pPresentationDescriptor));
        if (!spStartOp)
        {
            hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr))
        {
            hr = spStartOp->SetData(*pvarStartPos);
        }
    }

    if (SUCCEEDED(hr))
    {
        // Queue asynchronous operation
        hr = QueueOperation(spStartOp.Get());
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::Stop()
{
    HRESULT hr = S_OK;
    ComPtr<CSourceOperation> spStopOp;
    spStopOp.Attach(new (std::nothrow) CSourceOperation(CSourceOperation::Operation_Stop));
    if (!spStopOp)
    {
        hr = E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        // Queue asynchronous stop
        hr = QueueOperation(spStopOp.Get());
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::GetService( _In_ REFGUID guidService, _In_ REFIID riid, _Out_opt_ LPVOID *ppvObject)
{
    HRESULT hr = MF_E_UNSUPPORTED_SERVICE;

    if (ppvObject == nullptr)
    {
        return E_POINTER;
    }

    if (guidService == MF_RATE_CONTROL_SERVICE)
    {
        hr = QueryInterface(riid, ppvObject);
    }

    return hr;
}

IFACEMETHODIMP CMediaSource::SetRate(BOOL fThin, float flRate)
{
    if (fThin)
    {
        return MF_E_THINNING_UNSUPPORTED;
    }
    if (!IsRateSupported(flRate, &flRate))
    {
        return MF_E_UNSUPPORTED_RATE;
    }

    AutoLock lock(_critSec);
    HRESULT hr = S_OK;

    if (flRate == _flRate)
    {
        return S_OK;
    }

    ComPtr<CSourceOperation> spSetRateOp;
    spSetRateOp.Attach(new (std::nothrow) CSetRateOperation(fThin, flRate));
    if (!spSetRateOp)
    {
        hr = E_OUTOFMEMORY;
    }

    if (SUCCEEDED(hr))
    {
        // Queue asynchronous stop
        hr = QueueOperation(spSetRateOp.Get());
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaSource::GetRate(_Inout_opt_ BOOL *pfThin, _Inout_opt_ float *pflRate)
{
    AutoLock lock(_critSec);
    if (pfThin == nullptr || pflRate == nullptr)
    {
        return E_INVALIDARG;
    }

    *pfThin = FALSE;
    *pflRate = _flRate;

    return S_OK;
}

__override HRESULT CMediaSource::DispatchOperation(_In_ CSourceOperation *pOp)
{
    AutoLock lock(_critSec);

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        switch(pOp->GetOperationType())
        {
        case CSourceOperation::Operation_Start:
            DoStart(static_cast<CStartOperation *>(pOp));
            break;
        case CSourceOperation::Operation_Stop:
            DoStop(pOp);
            break;
        case CSourceOperation::Operation_SetRate:
            DoSetRate(static_cast<CSetRateOperation *>(pOp));
            break;
        default:
            hr = E_UNEXPECTED;
            break;
        }
    }

    TRACEHR_RET(hr);
}

__override HRESULT CMediaSource::ValidateOperation(_In_ CSourceOperation *pOp)
{
    return S_OK;
}

concurrency::task<void> CMediaSource::OpenAsync(LPCWSTR pszUrl)
{
    AutoLock lock(_critSec);

    if (_eSourceState != SourceState_Invalid)
    {
        Throw(MF_E_INVALIDREQUEST);
    }

    if (pszUrl == nullptr)
    {
        Throw(E_INVALIDARG);
    }

    // Get network client
    INetworkClient ^networkClient = safe_cast<INetworkClient^>(_networkSender);

    // Parse url (to obtain host name and port)
    ParseServerUrl(pszUrl);

    concurrency::create_task(networkClient->ConnectAsync(_serverAddress, _serverPort)).then([this](concurrency::task<void>& connectTask)
    {
        AutoLock lock(_critSec);
        try
        {
            connectTask.get();
            ThrowIfError(CheckShutdown());
            if (_eSourceState != SourceState_Opening)
            {
                Throw(MF_E_UNEXPECTED);
            }

            SendDescribeRequest();
        }
        catch(Exception ^exc)
        {
            HandleError(exc->HResult);
            Shutdown();
        }
    });

    // If everything is ok now we are waiting for network client to connect. 
    // Change state to opening.
    _eSourceState = SourceState_Opening;

    return concurrency::task<void>(_openedEvent);
}

_Acquires_lock_(_critSec)
HRESULT CMediaSource::Lock()
{
    _critSec.Lock();
    return S_OK;
}

_Releases_lock_(_critSec)
HRESULT CMediaSource::Unlock()
{
    _critSec.Unlock();
    return S_OK;
}

void CMediaSource::Initialize()
{
    try
    {
        INetworkClient ^networkClient;

        // Create the event queue helper.
        ThrowIfError(MFCreateEventQueue(&_spEventQueue));

        ThrowIfError(CBaseAttributes<>::Initialize());

        // Create network client
        networkClient = CreateNetworkClient();

        _networkSender = safe_cast<INetworkChannel^>(networkClient);
    }
    catch(Exception^)
    {
        Shutdown();
        throw;
    }
}

// Handle errors
void CMediaSource::HandleError(HRESULT hResult)
{
    if (_eSourceState == SourceState_Opening)
    {
        // If we have an error during opening operation complete it and pass the error to client.
        CompleteOpen(hResult);
    }
    else if (_eSourceState != SourceState_Shutdown)
    {
        // If we received an error at any other time (except shutdown) send MEError event.
        QueueEvent(MEError, GUID_NULL, hResult, nullptr);
    }
}

// Returns stream object associated with given identifier
HRESULT CMediaSource::GetStreamById(DWORD dwId, CMediaStream **ppStream)
{
    assert(ppStream);
    HRESULT hr = MF_E_NOT_FOUND;
    StreamContainer::POSITION pos = _streams.FrontPosition();
    StreamContainer::POSITION posEnd = _streams.EndPosition();

    for (; pos != posEnd; pos = _streams.Next(pos))
    {
        ComPtr<IMFMediaStream> spStream;
        HRESULT hErrorCode = _streams.GetItemPos(pos, &spStream);
        if (FAILED(hErrorCode))
        {
            hr = hErrorCode;
            break;
        }
        CMediaStream *pStream = static_cast<CMediaStream*>(spStream.Get());
        if (pStream->GetId() == dwId)
        {
            *ppStream = pStream;
            (*ppStream)->AddRef();
            hr = S_OK;
        }
    }

    TRACEHR_RET(hr);
}

void CMediaSource::ParseServerUrl(LPCWSTR pszUrl)
{
    if (pszUrl == nullptr)
    {
        Throw(E_INVALIDARG);
    }

    auto uri = ref new Windows::Foundation::Uri(ref new String(pszUrl));

    String ^scheme = uri->SchemeName;
    // It is not a scheme we support
    if (wcscmp(scheme->Data(), c_szStspScheme) != 0)
    {
        Throw(MF_E_UNSUPPORTED_SCHEME);
    }

    if (uri->Port == 0)
    {
        _serverPort = c_wStspDefaultPort;
    }
    else if (uri->Port < 0 || uri->Port > WORD_MAX)
    {
        Throw(E_UNEXPECTED);
    }
    else
    {
        _serverPort = (WORD)uri->Port;
    }

    _serverAddress = uri->Host;
}

void CMediaSource::CompleteOpen(HRESULT hResult)
{
    assert(!_openedEvent._IsTriggered());
    if (FAILED(hResult))
    {
        _openedEvent.set_exception(ref new Exception(hResult));
    }
    else
    {
        _openedEvent.set();
    }
}

concurrency::task<void> CMediaSource::SendRequestAsync(StspOperation eOperation)
{
    ComPtr<IMediaBufferWrapper> spBuffer;
    ComPtr<IBufferPacket> spPacket;
    // We just send an operation header this operation contains no payload.
    ThrowIfError(CreateMediaBufferWrapper(sizeof(StspOperationHeader), &spBuffer));

    ThrowIfError(CreateBufferPacket(&spPacket));

    // Prepare operation header.
    StspOperationHeader *pOpHeader = reinterpret_cast<StspOperationHeader *>(spBuffer->GetBuffer());
    pOpHeader->cbDataSize = 0;
    pOpHeader->eOperation = eOperation;
    ThrowIfError(spBuffer->SetCurrentLength(sizeof(StspOperationHeader)));

    ThrowIfError(spPacket->AddBuffer(spBuffer.Get()));

    return concurrency::create_task(_networkSender->SendAsync(spPacket.Get()));
}

// Sending request for media description to the server
void CMediaSource::SendDescribeRequest()
{
    _CurrentReceivedOperationHeader.eOperation = StspOperation_Unknown;
    ComPtr<CMediaSource> spThis = this;
    SendRequestAsync(StspOperation_ClientRequestDescription).then([this, spThis](concurrency::task<void>& task)
    {
        AutoLock lock(_critSec);
        try
        {
            task.get();

            ThrowIfError(CheckShutdown());
            if (_eSourceState != SourceState_Opening)
            {
                Throw(MF_E_UNEXPECTED);
            }

            // Start receiving
            Receive();
        }
        catch(Exception ^exc)
        {
            HandleError(exc->HResult);
        }
    });
}

void CMediaSource::SendStartRequest()
{
    _CurrentReceivedOperationHeader.eOperation = StspOperation_Unknown;
    ComPtr<CMediaSource> spThis = this;
    SendRequestAsync(StspOperation_ClientRequestStart).then([this, spThis](concurrency::task<void>& task)
    {
        AutoLock lock(_critSec);
        try
        {
            task.get();

            ThrowIfError(CheckShutdown());
            if (_eSourceState != SourceState_Starting && _eSourceState != SourceState_Started)
            {
                Throw(MF_E_UNEXPECTED);
            }
        }
        catch(Exception ^exc)
        {
            HandleError(exc->HResult);
        }
    });
}

// Trigger receiving operation
void CMediaSource::Receive()
{
    // We already during receive operation
    if (_spCurrentReceiveBuffer)
    {
        Throw(MF_E_INVALIDREQUEST);
    }

    ComPtr<IMediaBufferWrapper> spBuffer;
    ThrowIfError(CreateMediaBufferWrapper(c_cbReceiveBufferSize, &spBuffer));

    ComPtr<CMediaSource> spThis = this;
    concurrency::create_task(_networkSender->ReceiveAsync(spBuffer.Get())).then([this, spThis](concurrency::task<void>& task)
    {
        AutoLock lock(_critSec);
        try
        {
            task.get();

            ThrowIfError(CheckShutdown());

            ParseCurrentBuffer();

            // Continue receiving
            Receive();
        }
        catch(Exception ^exc)
        {
            HandleError(exc->HResult);
        }
     });

    _spCurrentReceiveBuffer = spBuffer;
}

// Parse data stored in current receive buffer
void CMediaSource::ParseCurrentBuffer()
{
    bool fCompletePackage = false;
    DWORD cbTotalLength = 0;
    // Remember the buffer
    ComPtr<IMediaBufferWrapper> spLastBuffer = _spCurrentReceiveBuffer;

    if (_spCurrentReceivePacket == nullptr)
    {
        // If packet object storing current operation data is not created yet - create it.
        ThrowIfError(CreateBufferPacket(&_spCurrentReceivePacket));
    }

    // Add current receive buffer to the packet.
    ThrowIfError(_spCurrentReceivePacket->AddBuffer(_spCurrentReceiveBuffer.Get()));
    _spCurrentReceiveBuffer.Reset();

    // Parsing new package, we have't read operation just yet
    if (_CurrentReceivedOperationHeader.eOperation == StspOperation_Unknown)
    {
        DWORD cbCurrentLength;
        ThrowIfError(_spCurrentReceivePacket->GetTotalLength(&cbCurrentLength));

        // Check if we have enough data to retrieve operation header
        if(cbCurrentLength >= sizeof(StspOperationHeader))
        {
            ThrowIfError(_spCurrentReceivePacket->MoveLeft(sizeof(_CurrentReceivedOperationHeader), &_CurrentReceivedOperationHeader));
        }
        else
        {
            // We don't have enough data just yet so set data size to maximum packet size (then we will pass validation performed later on)
            _CurrentReceivedOperationHeader.cbDataSize = c_cbMaxPacketSize;
        }
    }

    // Get length of the packet (without operation header which was removed from the packet).
    ThrowIfError(_spCurrentReceivePacket->GetTotalLength(&cbTotalLength)); 

    if (// packet size is too large
        (c_cbMaxPacketSize < _CurrentReceivedOperationHeader.cbDataSize ||
        // Unrecognized operation
        _CurrentReceivedOperationHeader.eOperation >= StspOperation_Last))
    {
        Throw(MF_E_UNSUPPORTED_FORMAT);
    }

    if (// Size of the package already exceeded value described in operation header
        cbTotalLength > _CurrentReceivedOperationHeader.cbDataSize)
    {
        // Trim the right edge of the packet and get buffer representing trimmed data
        ThrowIfError(spLastBuffer->TrimRight(cbTotalLength - _CurrentReceivedOperationHeader.cbDataSize, &_spCurrentReceiveBuffer));
        // Fix total length
        cbTotalLength = _CurrentReceivedOperationHeader.cbDataSize;
    }

    // We've got a whole packet
    if (cbTotalLength == _CurrentReceivedOperationHeader.cbDataSize)
    {
        // Process packet payload
        ProcessPacket(&_CurrentReceivedOperationHeader, _spCurrentReceivePacket.Get());
        _spCurrentReceivePacket.Reset();
        _CurrentReceivedOperationHeader.eOperation =  StspOperation_Unknown;
    }

    if (_spCurrentReceiveBuffer)
    {
        // We've got a buffer which was left from TrimRight operation above, we can keep parsing.
        ParseCurrentBuffer();
    }
}

void CMediaSource::ProcessPacket(StspOperationHeader *pOpHeader, IBufferPacket *pPacket)
{
    assert(pOpHeader);
    assert(pPacket);

    switch(pOpHeader->eOperation)
    {
        // We received server description
    case StspOperation_ServerDescription:
        ProcessServerDescription(pPacket);
        break;
        // We received a media sample
    case StspOperation_ServerSample:
        ProcessServerSample(pPacket);
        break;
    case StspOperation_ServerFormatChange:
        ProcessServerFormatChange(pPacket);
        break;
        // No supported operation
    default:
        Throw(MF_E_UNSUPPORTED_FORMAT);
        break;
    }
}

// Process server description packet
void CMediaSource::ProcessServerDescription(IBufferPacket *pPacket)
{
    DWORD cbTotalLen = 0;
    DWORD cbConstantSize = 0;
    StspDescription desc = {};
    StspDescription *pDescription = nullptr;

    if (_eSourceState != SourceState_Opening)
    {
        // Server description should only be sent during opening state
        Throw(MF_E_UNEXPECTED);
    }

    ThrowIfError(pPacket->GetTotalLength(&cbTotalLen));

    // Minimum size of the operation payload is size of Description structure
    if (cbTotalLen < sizeof(StspDescription))
    {
        ThrowIfError(MF_E_UNSUPPORTED_FORMAT);
    }

    // Copy description.
    ThrowIfError(pPacket->MoveLeft(sizeof(desc), &desc));


    // Size of the packet should match size described in the packet (size of Description structure
    // plus size of attribute blob)
    cbConstantSize = sizeof(desc) + (desc.cNumStreams - 1) * sizeof(StspStreamDescription);
    // Check if the input parameters are valid. We only support 2 streams.
    if (cbConstantSize < sizeof(desc) || desc.cNumStreams == 0 || 
        desc.cNumStreams > 2 || cbTotalLen < cbConstantSize)
    {
        ThrowIfError(MF_E_UNSUPPORTED_FORMAT);
    }

    BYTE *pPtr = new (std::nothrow) BYTE[cbConstantSize];
    if (pPtr == nullptr)
    {
        Throw(E_OUTOFMEMORY);
    }

    try
    {
        // Copy what we've already read
        CopyMemory(pPtr, &desc, sizeof(desc));

        // Add more stream data if necessary at the end
        ThrowIfError(pPacket->MoveLeft(cbConstantSize - sizeof(desc), pPtr + sizeof(desc)));

        // Data is ready
        pDescription = reinterpret_cast<StspDescription *>(pPtr);

        DWORD cbAttributeSize = 0;
        for (DWORD nStream = 0; nStream < desc.cNumStreams; ++nStream)
        {

            if (FAILED(DWordAdd(cbAttributeSize, pDescription->aStreams[nStream].cbAttributesSize, &cbAttributeSize)))
            {
                Throw(MF_E_UNSUPPORTED_FORMAT);
            }
        }

        // Validate the parameters. Limit the total size of attributes to 64kB.
        if ((cbTotalLen != (cbConstantSize + cbAttributeSize)) || (cbAttributeSize > 0x10000))
        {
            Throw(MF_E_UNSUPPORTED_FORMAT);
        }

        // Create stream for every stream description sent by the server.
        for(DWORD nStream = 0; nStream < pDescription->cNumStreams; ++nStream)
        {
            ComPtr<CMediaStream> spStream;

            ThrowIfError(CMediaStream::CreateInstance(&pDescription->aStreams[nStream], pPacket, this, &spStream));

            ThrowIfError(_streams.InsertBack(spStream.Get()));
        }

        InitPresentationDescription();

        // Everything succeeded we are in stopped state now
        _eSourceState = SourceState_Stopped;
        CompleteOpen(S_OK);

        delete[] pPtr;
    }
    catch(Exception ^exc)
    {
        delete[] pPtr;
        throw;
    }
}

// Process a media sample reveived from the server.
void CMediaSource::ProcessServerSample(IBufferPacket *pPacket)
{
    if (_eSourceState == SourceState_Started)
    {
        // Only process samples when we are in started state
        StspSampleHeader sampleHead = {};
        DWORD cbTotalSize;

        // Copy the header object
        ThrowIfError(pPacket->MoveLeft(sizeof(sampleHead), &sampleHead));
        ThrowIfError(pPacket->GetTotalLength(&cbTotalSize));

        // Convert packet to MF sample
        ComPtr<IMFSample> spSample;
        ComPtr<CMediaStream> spStream;

        ThrowIfError(GetStreamById(sampleHead.dwStreamId, &spStream));

        if (spStream->IsActive())
        {
            ThrowIfError(pPacket->ToMFSample(&spSample));
            // Forward sample to a proper stream.
            spStream->ProcessSample(&sampleHead, spSample.Get());
        }
    }
    else
    {
        Throw(MF_E_UNEXPECTED);
    }
}

void CMediaSource::ProcessServerFormatChange(IBufferPacket *pPacket)
{
    BYTE *pAttributes = nullptr;
    Exception ^error;

    try
    {
        if (_eSourceState == SourceState_Started)
        {
            DWORD cbTotalLen = 0;
            StspStreamDescription streamDesc;
            ThrowIfError(pPacket->GetTotalLength(&cbTotalLen));

            // Minimum size of the operation payload is size of Description structure
            if (cbTotalLen < sizeof(StspStreamDescription))
            {
                ThrowIfError(MF_E_UNSUPPORTED_FORMAT);
            }

            ThrowIfError(pPacket->MoveLeft(sizeof(streamDesc), &streamDesc));

            if (cbTotalLen != sizeof(StspStreamDescription) + streamDesc.cbAttributesSize || streamDesc.cbAttributesSize == 0)
            {
                ThrowIfError(MF_E_UNSUPPORTED_FORMAT);
            }

            // Prepare buffer where we will copy attributes to
            pAttributes = new(std::nothrow) BYTE[streamDesc.cbAttributesSize];
            if (pAttributes == nullptr)
            {
                Throw(E_OUTOFMEMORY);
            }

            ThrowIfError(pPacket->MoveLeft(streamDesc.cbAttributesSize, pAttributes));

            ComPtr<IMFMediaType> spMediaType;
            // Create a media type object.
            ThrowIfError(MFCreateMediaType(&spMediaType));
            // Initialize media type's attributes
            ThrowIfError(MFInitAttributesFromBlob(spMediaType.Get(), pAttributes, streamDesc.cbAttributesSize));

        }
        else
        {
            Throw(MF_E_UNEXPECTED);
        }
    }
    catch(Exception ^exc)
    {
        error = exc;
    }

    delete[] pAttributes;

    if (error != nullptr)
    {
        throw error;
    }
}

void CMediaSource::InitPresentationDescription()
{
    ComPtr<IMFPresentationDescriptor> spPresentationDescriptor;
    IMFStreamDescriptor **aStreams = nullptr;

    aStreams = new (std::nothrow) IMFStreamDescriptor*[_streams.GetCount()];
    if (aStreams == nullptr)
    {
        Throw(E_OUTOFMEMORY);
    }

    ZeroMemory(aStreams, sizeof(aStreams[0]) * _streams.GetCount());

    StreamContainer::POSITION pos = _streams.FrontPosition();
    StreamContainer::POSITION posEnd = _streams.EndPosition();

    for (DWORD nStream = 0; pos != posEnd; ++nStream, pos = pos = _streams.Next(pos))
    {
        ComPtr<IMFMediaStream> spStream;
        ThrowIfError(_streams.GetItemPos(pos, &spStream));
        ThrowIfError(spStream->GetStreamDescriptor(&aStreams[nStream]));
    }

    ThrowIfError(MFCreatePresentationDescriptor(_streams.GetCount(), aStreams, &spPresentationDescriptor));

    for (DWORD nStream = 0; nStream < _streams.GetCount(); ++nStream)
    {
        ThrowIfError(spPresentationDescriptor->SelectStream(nStream));
    }

    _spPresentationDescriptor = spPresentationDescriptor;

    if (aStreams != nullptr)
    {
        for (DWORD nStream = 0; nStream < _streams.GetCount(); ++nStream)
        {
            if (aStreams[nStream] != nullptr)
            {
                aStreams[nStream]->Release();
            }
        }

        delete[] aStreams;
    }
}

HRESULT CMediaSource::ValidatePresentationDescriptor(IMFPresentationDescriptor *pPD)
{
    HRESULT hr = S_OK;
    BOOL fSelected = FALSE;
    DWORD cStreams = 0;

    if (_streams.GetCount() == 0)
    {
        return E_UNEXPECTED;
    }

    // The caller's PD must have the same number of streams as ours.
    hr = pPD->GetStreamDescriptorCount(&cStreams);

    if (SUCCEEDED(hr))
    {
        if (cStreams != _streams.GetCount())
        {
            hr = E_INVALIDARG;
        }
    }

    // The caller must select at least one stream.
    if (SUCCEEDED(hr))
    {
        for (DWORD i = 0; i < cStreams; i++)
        {
            ComPtr<IMFStreamDescriptor> spSD;
            hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &spSD);
            if (FAILED(hr))
            {
                break;
            }
        }
    }

    TRACEHR_RET(hr);
}

void CMediaSource::SelectStreams(IMFPresentationDescriptor *pPD)
{
    for (DWORD nStream = 0; nStream < _streams.GetCount(); ++nStream)
    {
        ComPtr<IMFStreamDescriptor> spSD;
        ComPtr<CMediaStream> spStream;
        DWORD nStreamId;
        BOOL fSelected;

        // Get next stream descriptor
        ThrowIfError(pPD->GetStreamDescriptorByIndex(nStream, &fSelected, &spSD));

        // Get stream id
        ThrowIfError(spSD->GetStreamIdentifier(&nStreamId));

        // Get simple net media stream
        ThrowIfError(GetStreamById(nStreamId, &spStream));

        // Remember if stream was selected
        bool fWasSelected = spStream->IsActive();
        ThrowIfError(spStream->SetActive(!!fSelected));

        if (fSelected)
        {
            // Choose event type to send
            MediaEventType met = (fWasSelected) ? MEUpdatedStream : MENewStream;
            ComPtr<IUnknown> spStreamUnk;

            spStream.As(&spStreamUnk);

            ThrowIfError(_spEventQueue->QueueEventParamUnk(met, GUID_NULL, S_OK, spStreamUnk.Get()));

            // Start the stream. The stream will send the appropriate event.
            ThrowIfError(spStream->Start());
        }
    }
}

void CMediaSource::DoStart(CStartOperation *pOp)
{
    assert(pOp->GetOperationType() == CSourceOperation::Operation_Start);
    ComPtr<IMFPresentationDescriptor> spPD = pOp->GetPresentationDescriptor();

    try
    {
        SelectStreams(spPD.Get());

        _eSourceState = SourceState_Starting;
        SendStartRequest();

        _eSourceState = SourceState_Started;
        ThrowIfError(_spEventQueue->QueueEventParamVar(MESourceStarted, GUID_NULL, S_OK, &pOp->GetData()));
    }
    catch(Exception ^exc)
    {
        _spEventQueue->QueueEventParamVar(MESourceStarted, GUID_NULL, exc->HResult, nullptr);
    }
}

void CMediaSource::DoStop(CSourceOperation *pOp)
{
    assert(pOp->GetOperationType() == CSourceOperation::Operation_Stop);

    HRESULT hr = S_OK;
    try
    {
        StreamContainer::POSITION pos = _streams.FrontPosition();
        StreamContainer::POSITION posEnd = _streams.EndPosition();

        for (; pos != posEnd; pos = _streams.Next(pos))
        {
            ComPtr<IMFMediaStream> spStream;
            ThrowIfError(_streams.GetItemPos(pos, &spStream));
            CMediaStream *pStream = static_cast<CMediaStream*>(spStream.Get());
            if (pStream->IsActive())
            {
                ThrowIfError(pStream->Flush());
                ThrowIfError(pStream->Stop());
            }
        }
    }
    catch(Exception ^exc)
    {
        hr = exc->HResult;
    }
    // Send the "stopped" event. This might include a failure code.
    (void)_spEventQueue->QueueEventParamVar(MESourceStopped, GUID_NULL, hr, nullptr);
}

void CMediaSource::DoSetRate(CSetRateOperation *pOp)
{
    assert(pOp->GetOperationType() == CSourceOperation::Operation_SetRate);

    HRESULT hr = S_OK;
    try
    {
        StreamContainer::POSITION pos = _streams.FrontPosition();
        StreamContainer::POSITION posEnd = _streams.EndPosition();

        for (; pos != posEnd; pos = _streams.Next(pos))
        {
            ComPtr<IMFMediaStream> spStream;
            ThrowIfError(_streams.GetItemPos(pos, &spStream));

            CMediaStream *pStream = static_cast<CMediaStream*>(spStream.Get());

            if (pStream->IsActive())
            {
                ThrowIfError(pStream->Flush());
                ThrowIfError(pStream->SetRate(pOp->GetRate()));
            }
        }

        _flRate = pOp->GetRate();
    }
    catch(Exception ^exc)
    {
        hr = exc->HResult;
    }
    // Send the "rate changed" event. This might include a failure code.
    (void)_spEventQueue->QueueEventParamVar(MESourceRateChanged, GUID_NULL, hr, nullptr);
}

bool CMediaSource::IsRateSupported(float flRate, float *pflAdjustedRate)
{
    if (flRate < 0.00001f && flRate > -0.00001f)
    {
        *pflAdjustedRate = 0.0f;
        return true;
    }
    else if(flRate < 1.0001f && flRate > 0.9999f)
    {
        *pflAdjustedRate = 1.0f;
        return true;
    }
    return false;
}
