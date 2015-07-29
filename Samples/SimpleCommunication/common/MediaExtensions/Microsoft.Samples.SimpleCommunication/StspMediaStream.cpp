//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "StspMediaStream.h"
#include "StspMediaSource.h"

using namespace Microsoft::Samples::SimpleCommunication;
using namespace Microsoft::Samples::SimpleCommunication::Network;

#define SET_SAMPLE_ATTRIBUTE(flag, mask, pSample, flagName) \
    if ((StspSampleFlag_##flagName & mask) == StspSampleFlag_##flagName) \
{ \
    ThrowIfError(pSample->SetUINT32(MFSampleExtension_##flagName, (StspSampleFlag_##flagName & flag) == StspSampleFlag_##flagName)); \
}

// RAII object locking the the source on initialization and unlocks it on deletion
class CMediaStream::CSourceLock
{
public:
    CSourceLock(CMediaSource *pSource)
        : _spSource(pSource)
    {
        if (_spSource)
        {
            _spSource->Lock();
        }
    }

    ~CSourceLock()
    {
        if (_spSource)
        {
            _spSource->Unlock();
        }
    }

private:
    ComPtr<CMediaSource> _spSource;
};

HRESULT CMediaStream::CreateInstance(StspStreamDescription *pStreamDescription, IBufferPacket *pAttributesBuffer, CMediaSource *pSource, CMediaStream **ppStream)
{
    if (pStreamDescription == nullptr || pSource == nullptr || ppStream == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    try
    {
        ComPtr<CMediaStream> spStream;
        spStream.Attach(new (std::nothrow) CMediaStream(pSource));
        if (!spStream)
        {
            Throw(E_OUTOFMEMORY);
        }

        spStream->Initialize(pStreamDescription, pAttributesBuffer);


        (*ppStream) = spStream.Detach();
    }
    catch(Exception ^exc)
    {
        hr = exc->HResult;
    }

    TRACEHR_RET(hr);
}

CMediaStream::CMediaStream(CMediaSource *pSource)
    : _cRef(1)
    , _spSource(pSource)
    , _eSourceState(SourceState_Invalid)
    , _fActive(false)
    , _flRate(1.0f)
    , _fVideo(false)
    , _eDropMode(MF_DROP_MODE_NONE)
    , _fDiscontinuity(false)
    , _fDropTime(false)
    , _fInitDropTime(false)
    , _fWaitingForCleanPoint(true)
    , _hnsStartDroppingAt(0)
    , _hnsAmountToDrop(0)
{
}

CMediaStream::~CMediaStream(void)
{
}

// IUnknown methods

IFACEMETHODIMP CMediaStream::QueryInterface(REFIID riid, void **ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }
    (*ppv) = nullptr;

    HRESULT hr = S_OK;
    if (riid == IID_IUnknown || 
        riid == IID_IMFMediaStream ||
        riid == IID_IMFMediaEventGenerator) 
    {
        (*ppv) = static_cast<IMFMediaStream*>(this);
        AddRef();
    }
    else if (riid == IID_IMFQualityAdvise || riid == IID_IMFQualityAdvise2)
    {
        (*ppv) = static_cast<IMFQualityAdvise2*>(this);
        AddRef();
    }
    else if (riid == IID_IMFGetService)
    {
        (*ppv) = static_cast<IMFGetService*>(this);
        AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

IFACEMETHODIMP_(ULONG) CMediaStream::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CMediaStream::Release()
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

IFACEMETHODIMP CMediaStream::BeginGetEvent(IMFAsyncCallback *pCallback, IUnknown *punkState)
{
    HRESULT hr = S_OK;

    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = _spEventQueue->BeginGetEvent(pCallback, punkState);
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaStream::EndGetEvent(IMFAsyncResult *pResult, IMFMediaEvent **ppEvent)
{
    HRESULT hr = S_OK;

    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = _spEventQueue->EndGetEvent(pResult, ppEvent);
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaStream::GetEvent(DWORD dwFlags, IMFMediaEvent **ppEvent)
{
    // NOTE:
    // GetEvent can block indefinitely, so we don't hold the lock.
    // This requires some juggling with the event queue pointer.

    HRESULT hr = S_OK;

    ComPtr<IMFMediaEventQueue> spQueue;

    {
        CSourceLock lock(_spSource.Get());

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

IFACEMETHODIMP CMediaStream::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, PROPVARIANT const *pvValue)
{
    HRESULT hr = S_OK;

    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = _spEventQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
    }

    TRACEHR_RET(hr);
}

// IMFMediaStream methods

IFACEMETHODIMP CMediaStream::GetMediaSource(IMFMediaSource **ppMediaSource)
{
    if (ppMediaSource == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        *ppMediaSource = _spSource.Get();
        (*ppMediaSource)->AddRef();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaStream::GetStreamDescriptor(IMFStreamDescriptor **ppStreamDescriptor)
{
    if (ppStreamDescriptor == nullptr)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        *ppStreamDescriptor = _spStreamDescriptor.Get();
        (*ppStreamDescriptor)->AddRef();
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaStream::RequestSample(IUnknown *pToken)
{
    CSourceLock lock(_spSource.Get());

    try
    {
        ThrowIfError(CheckShutdown());

        if (_eSourceState != SourceState_Started)
        {
            // We cannot be asked for a sample unless we are in started state
            Throw(MF_E_INVALIDREQUEST);
        }

        // Put token onto the list to return it when we have a sample ready
        ThrowIfError(_tokens.InsertBack(pToken));

        // Trigger sample delivery
        DeliverSamples();
    }
    catch(Exception ^exc)
    {
        HandleError(exc->HResult);
    }

    TRACEHR_RET(S_OK);
}

// IMFQualityAdvise methods

IFACEMETHODIMP CMediaStream::SetDropMode(
    _In_ MF_QUALITY_DROP_MODE eDropMode )
{
    HRESULT hr = S_OK;
    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    //
    // Only support one drop mode
    //
    if( SUCCEEDED(hr) &&
        ((eDropMode < MF_DROP_MODE_NONE) || (eDropMode >= MF_DROP_MODE_2)))
    {
        hr = MF_E_NO_MORE_DROP_MODES;
    }

    if (SUCCEEDED(hr) && _eDropMode != eDropMode)
    {
        _eDropMode = eDropMode;
        _fWaitingForCleanPoint = true;
        
        TRACE(TRACE_LEVEL_NORMAL, L"Setting drop mode to %d\n", _eDropMode);
    }

    return( hr );
}

IFACEMETHODIMP CMediaStream::SetQualityLevel(
    _In_ MF_QUALITY_LEVEL eQualityLevel )
{
    return( MF_E_NO_MORE_QUALITY_LEVELS );
}

IFACEMETHODIMP CMediaStream::GetDropMode(
    _Out_  MF_QUALITY_DROP_MODE *peDropMode )
{
    HRESULT hr = S_OK;

    if (peDropMode == nullptr)
    {
        return E_POINTER;
    }

    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        *peDropMode = _eDropMode;
    }

    TRACEHR_RET(hr);
}

IFACEMETHODIMP CMediaStream::GetQualityLevel(
    _Out_  MF_QUALITY_LEVEL *peQualityLevel )
{
    return( E_NOTIMPL );
}

IFACEMETHODIMP CMediaStream::DropTime( _In_ LONGLONG hnsAmountToDrop )
{
    HRESULT hr = S_OK;
    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (hnsAmountToDrop > 0)
        {
            _fDropTime = true;
            _fInitDropTime = true;
            _hnsAmountToDrop = hnsAmountToDrop;
            TRACE(TRACE_LEVEL_NORMAL, L"Dropping time hnsAmountToDrop=%I64d\n", hnsAmountToDrop);
        }
        else if (hnsAmountToDrop == 0)
        {
            // Reset time dropping
            TRACE(TRACE_LEVEL_NORMAL, L"Disabling dropping time\n");
            ResetDropTime();
        }
        else
        {
            hr = E_INVALIDARG;
        }
    }

    TRACEHR_RET(hr);
}

// IMFQualityAdvise2 methods

IFACEMETHODIMP CMediaStream::NotifyQualityEvent(_In_opt_ IMFMediaEvent *pEvent, _Out_ DWORD *pdwFlags)
{
    HRESULT hr = S_OK;
    CSourceLock lock(_spSource.Get());
    
    if (pdwFlags == nullptr || pEvent == nullptr)
    {
        return E_POINTER;
    }

    *pdwFlags = 0;
    
    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        MediaEventType met;
        hr = pEvent->GetType(&met);

        if (SUCCEEDED(hr) && met == MEQualityNotify)
        {
            GUID guiExtendedType;
            hr = pEvent->GetExtendedType(&guiExtendedType);

            if (SUCCEEDED(hr) && guiExtendedType == MF_QUALITY_NOTIFY_SAMPLE_LAG)
            {
                PROPVARIANT var;
                PropVariantInit(&var);

                hr = pEvent->GetValue(&var);
                LONGLONG hnsSampleLatency = var.hVal.QuadPart;

                if (SUCCEEDED(hr))
                {
                    if (_eDropMode == MF_DROP_MODE_NONE && hnsSampleLatency > 30000000)
                    {
                        TRACE(TRACE_LEVEL_NORMAL, L"Entering drop mode\n");
                        hr = SetDropMode(MF_DROP_MODE_1);
                    }
                    else if (_eDropMode == MF_DROP_MODE_1 && hnsSampleLatency < 0)
                    {
                        TRACE(TRACE_LEVEL_NORMAL, L": Leaving drop mode\n");
                        hr = SetDropMode(MF_DROP_MODE_NONE);
                    }
                    else
                    {
                        TRACE(TRACE_LEVEL_NORMAL, L": Sample latency = %I64d\n", hnsSampleLatency);
                    }
                }

                PropVariantClear(&var);
            }
        }
    }

    TRACEHR_RET(hr);
}

// IMFGetService methods

IFACEMETHODIMP CMediaStream::GetService(
    _In_ REFGUID guidService,
    _In_ REFIID riid,
    _Out_ LPVOID *ppvObject )
{
    HRESULT hr = S_OK;
    CSourceLock lock(_spSource.Get());

    if (ppvObject == nullptr)
    {
        return E_POINTER;
    }
    *ppvObject = NULL;

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if( MF_QUALITY_SERVICES == guidService )
        {
            hr = QueryInterface(riid, ppvObject);
        }
        else
        {
            hr = MF_E_UNSUPPORTED_SERVICE;
        }
    }

    TRACEHR_RET(hr);
}

HRESULT CMediaStream::Start()
{
    HRESULT hr = S_OK;
    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (_eSourceState == SourceState_Stopped ||
            _eSourceState == SourceState_Started)
        {
            _eSourceState = SourceState_Started;
            // Inform the client that we've started
            hr = QueueEvent(MEStreamStarted, GUID_NULL, S_OK, nullptr);
        }
        else
        {
            hr = MF_E_INVALID_STATE_TRANSITION;    
        }
    }

    if (FAILED(hr))
    {
        HandleError(hr);
    }

    return S_OK;
}

HRESULT CMediaStream::Stop()
{
    HRESULT hr = S_OK;
    CSourceLock lock(_spSource.Get());

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (_eSourceState == SourceState_Started)
        {
            _eSourceState = SourceState_Stopped;
            _tokens.Clear();
            _samples.Clear();
            // Inform the client that we've stopped.
            hr = QueueEvent(MEStreamStopped, GUID_NULL, S_OK, nullptr);
        }
        else
        {
            hr = MF_E_INVALID_STATE_TRANSITION;    
        }
    }

    if (FAILED(hr))
    {
        HandleError(hr);
    }

    return S_OK;
}

HRESULT CMediaStream::SetRate(float flRate)
{
    HRESULT hr = S_OK;
    CSourceLock lock(_spSource.Get());

    try
    {
        ThrowIfError(CheckShutdown());

        _flRate = flRate;
        if (_flRate != 1.0f)
        {
            CleanSampleQueue();
        }
    }
    catch(Exception ^exc)
    {
        HandleError(exc->HResult);
    }

    return S_OK;
}

HRESULT CMediaStream::Flush()
{
    CSourceLock lock(_spSource.Get());

    _tokens.Clear();
    _samples.Clear();

    _fDiscontinuity = false;
    _eDropMode = MF_DROP_MODE_NONE;
    ResetDropTime();

    return S_OK;
}

HRESULT CMediaStream::Shutdown()
{
    CSourceLock lock(_spSource.Get());

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        Flush();

        if (_spEventQueue)
        {
            _spEventQueue->Shutdown();
            _spEventQueue.Reset();
        }

        _spStreamDescriptor.Reset();
        _eSourceState = SourceState_Shutdown;
    }

    TRACEHR_RET(hr);
}

// Processes media sample received from the header
void CMediaStream::ProcessSample(StspSampleHeader *pSampleHeader, IMFSample *pSample)
{
    assert(pSample);

    try
    {
        ThrowIfError(CheckShutdown());
        // Set sample attributes
        SetSampleAttributes(pSampleHeader, pSample);

        // Check if we are in propper state if so deliver the sample otherwise just skip it and don't treat it as an error.
        if (_eSourceState == SourceState_Started)
        {
            // Put sample on the list
            ThrowIfError(_samples.InsertBack(pSample));
            // Deliver samples
            DeliverSamples();
        }
        else
        {
            Throw(MF_E_UNEXPECTED);
        }
    }
    catch(Exception ^exc)
    {
        HandleError(exc->HResult);
    }
}

void CMediaStream::ProcessFormatChange(IMFMediaType *pMediaType)
{
    assert(pMediaType);

    try
    {
        ThrowIfError(CheckShutdown());

        // Check if we are in proper state if so deliver the sample otherwise just skip it and don't treat it as an error.
        if (_eSourceState == SourceState_Started)
        {
            // Put sample on the list
            ThrowIfError(_samples.InsertBack(pMediaType));
            // Deliver samples
            DeliverSamples();
        }
        else
        {
            Throw(MF_E_UNEXPECTED);
        }
    }
    catch(Exception ^exc)
    {
        HandleError(exc->HResult);
    }
}

HRESULT CMediaStream::SetActive(bool fActive)
{
    CSourceLock lock(_spSource.Get());

    HRESULT hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (_eSourceState != SourceState_Stopped &&
            _eSourceState != SourceState_Started)
        {
            hr = MF_E_INVALIDREQUEST;
        }
    }

    if (SUCCEEDED(hr))
    {
        _fActive = fActive;
    }

    TRACEHR_RET(hr);
}

void CMediaStream::Initialize(StspStreamDescription *pStreamDescription, IBufferPacket *pAttributesBuffer)
{
    // Create the media event queue.
    Exception ^error = nullptr;
    BYTE *pAttributes = nullptr;
    
    try
    {
        ThrowIfError(MFCreateEventQueue(&_spEventQueue));
        ComPtr<IMFMediaType> spMediaType;
        ComPtr<IMFStreamDescriptor> spSD;
        ComPtr<IMFMediaTypeHandler> spMediaTypeHandler;
        DWORD cbAttributesSize = 0;

        _fVideo = (pStreamDescription->guiMajorType == MFMediaType_Video);

        // Create a media type object.
        ThrowIfError(MFCreateMediaType(&spMediaType));
        ThrowIfError(pAttributesBuffer->GetTotalLength(&cbAttributesSize));

        if (cbAttributesSize < pStreamDescription->cbAttributesSize || pStreamDescription->cbAttributesSize == 0)
        {
            // Invalid stream description
            Throw(MF_E_UNSUPPORTED_FORMAT);
        }

        // Prepare buffer where we will copy attributes to
        pAttributes = new(std::nothrow) BYTE[pStreamDescription->cbAttributesSize];
        if (pAttributes == nullptr)
        {
            Throw(E_OUTOFMEMORY);
        }

        // Move the memory
        ThrowIfError(pAttributesBuffer->MoveLeft(pStreamDescription->cbAttributesSize, pAttributes));
        // Initialize media type's attributes
        ThrowIfError(MFInitAttributesFromBlob(spMediaType.Get(), pAttributes, pStreamDescription->cbAttributesSize));
        ValidateInputMediaType(pStreamDescription->guiMajorType, pStreamDescription->guiSubType, spMediaType.Get());
        ThrowIfError(spMediaType->SetGUID(MF_MT_MAJOR_TYPE, pStreamDescription->guiMajorType));

        ThrowIfError(spMediaType->SetGUID(MF_MT_SUBTYPE, pStreamDescription->guiSubType));

        // Now we can create MF stream descriptor.
        ThrowIfError(MFCreateStreamDescriptor(pStreamDescription->dwStreamId, 1, spMediaType.GetAddressOf(), &spSD));
        ThrowIfError(spSD->GetMediaTypeHandler(&spMediaTypeHandler));

        // Set current media type
        ThrowIfError(spMediaTypeHandler->SetCurrentMediaType(spMediaType.Get()));

        _spStreamDescriptor = spSD;
        _dwId = pStreamDescription->dwStreamId;
        // State of the stream is started.
        _eSourceState = SourceState_Stopped;
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

// Deliver samples for every request client made
void CMediaStream::DeliverSamples()
{
    // Check if we have both: samples available in the queue and requests in request list.
    while (!_samples.IsEmpty() && !_tokens.IsEmpty())
    {
        ComPtr<IUnknown> spEntry;
        ComPtr<IMFSample> spSample;
        ComPtr<IUnknown> spToken;
        BOOL fDrop = FALSE;
        // Get the entry
        ThrowIfError(_samples.RemoveFront(&spEntry));

        if (SUCCEEDED(spEntry.As(&spSample)))
        {
            fDrop = ShouldDropSample(spSample.Get());

            if (!fDrop)
            {
                // Get the request token
                ThrowIfError(_tokens.RemoveFront(&spToken));

                if (spToken)
                {
                    // If token was not null set the sample attribute.
                    ThrowIfError(spSample->SetUnknown(MFSampleExtension_Token, spToken.Get()));
                }

                if (_fDiscontinuity)
                {
                    // If token was not null set the sample attribute.
                    ThrowIfError(spSample->SetUINT32(MFSampleExtension_Discontinuity, TRUE));
                    _fDiscontinuity = false;
                }

                // Send a sample event.
                ThrowIfError(_spEventQueue->QueueEventParamUnk(MEMediaSample, GUID_NULL, S_OK, spSample.Get()));
            }
            else
            {
                _fDiscontinuity = true;
            }
        }
        else
        {
            ComPtr<IMFMediaType> spMediaType;
            ThrowIfError(spEntry.As(&spMediaType));
            // Send a sample event.
            ThrowIfError(_spEventQueue->QueueEventParamUnk(MEStreamFormatChanged, GUID_NULL, S_OK, spMediaType.Get()));
        }
    }
}

void CMediaStream::HandleError(HRESULT hErrorCode)
{
    if (hErrorCode != MF_E_SHUTDOWN)
    {
        // Send MEError to the client
        hErrorCode = QueueEvent(MEError, GUID_NULL, hErrorCode, nullptr);
    }
}

void CMediaStream::SetSampleAttributes(StspSampleHeader *pSampleHeader, IMFSample *pSample)
{
    ThrowIfError(pSample->SetSampleTime(pSampleHeader->ullTimestamp));
    ThrowIfError(pSample->SetSampleDuration(pSampleHeader->ullDuration));

    SET_SAMPLE_ATTRIBUTE(pSampleHeader->dwFlags, pSampleHeader->dwFlagMasks, pSample, BottomFieldFirst);
    SET_SAMPLE_ATTRIBUTE(pSampleHeader->dwFlags, pSampleHeader->dwFlagMasks, pSample, CleanPoint);
    SET_SAMPLE_ATTRIBUTE(pSampleHeader->dwFlags, pSampleHeader->dwFlagMasks, pSample, DerivedFromTopField);
    SET_SAMPLE_ATTRIBUTE(pSampleHeader->dwFlags, pSampleHeader->dwFlagMasks, pSample, Discontinuity);
    SET_SAMPLE_ATTRIBUTE(pSampleHeader->dwFlags, pSampleHeader->dwFlagMasks, pSample, Interlaced);
    SET_SAMPLE_ATTRIBUTE(pSampleHeader->dwFlags, pSampleHeader->dwFlagMasks, pSample, RepeatFirstField);
    SET_SAMPLE_ATTRIBUTE(pSampleHeader->dwFlags, pSampleHeader->dwFlagMasks, pSample, SingleField);

    DWORD cbTotalLen;
    pSample->GetTotalLength(&cbTotalLen);
    TRACE(TRACE_LEVEL_LOW, L"Received sample TS-%I64d Duration-%I64d Length-%d %S\n", pSampleHeader->ullTimestamp, pSampleHeader->ullDuration, cbTotalLen, (pSampleHeader->dwFlags & StspSampleFlag_CleanPoint) ? "key frame" : "");
}

bool CMediaStream::ShouldDropSample(IMFSample *pSample) 
{
    if (!_fVideo)
    {
        return false;
    }

    bool fCleanPoint = MFGetAttributeUINT32( pSample, MFSampleExtension_CleanPoint, 0 ) > 0;
    bool fDrop = _flRate != 1.0f && !fCleanPoint;

    LONGLONG hnsTimeStamp = 0;
    ThrowIfError(pSample->GetSampleTime(&hnsTimeStamp));

    if (!fDrop && _fDropTime)
    {
        if (_fInitDropTime)
        {
            _hnsStartDroppingAt = hnsTimeStamp;
            _fInitDropTime = false;
        }

        fDrop = hnsTimeStamp < (_hnsStartDroppingAt + _hnsAmountToDrop);
        if (!fDrop)
        {
            TRACE(TRACE_LEVEL_LOW, L"Ending dropping time on sample ts=%I64d _hnsStartDroppingAt=%I64d _hnsAmountToDrop=%I64d\n", 
                hnsTimeStamp, _hnsStartDroppingAt, _hnsAmountToDrop);
            ResetDropTime();
        }
        else
        {
            TRACE(TRACE_LEVEL_LOW, L": Dropping sample ts=%I64d _hnsStartDroppingAt=%I64d _hnsAmountToDrop=%I64d\n", 
                hnsTimeStamp, _hnsStartDroppingAt, _hnsAmountToDrop);
        }
    }

    if (!fDrop && (_eDropMode == MF_DROP_MODE_1 || _fWaitingForCleanPoint))
    {
        // Only key frames
        fDrop = !fCleanPoint;
        if (fCleanPoint)
        {
            _fWaitingForCleanPoint = false;
        }

        if (fDrop)
        {
            TRACE(TRACE_LEVEL_LOW, L": Dropping sample ts=%I64d\n", 
                hnsTimeStamp, _hnsStartDroppingAt, _hnsAmountToDrop);
        }
    }

    return fDrop;
}

void CMediaStream::CleanSampleQueue()
{
    auto pos = _samples.FrontPosition();
    ComPtr<IUnknown> spEntry;
    ComPtr<IMFSample> spSample;

    if (_fVideo)
    {
        // For video streams leave first key frame.
        for (;SUCCEEDED(_samples.GetItemPos(pos, spEntry.ReleaseAndGetAddressOf())); pos = _samples.Next(pos))
        {
            if (SUCCEEDED(spEntry.As(&spSample)) && MFGetAttributeUINT32(spSample.Get(), MFSampleExtension_CleanPoint, 0))
            {
                break;
            }
        }
    }

    _samples.Clear();

    if (spSample != nullptr)
    {
        ThrowIfError(_samples.InsertFront(spSample.Get()));
    }
}

void CMediaStream::ResetDropTime()
{
    _fDropTime = false;
    _fInitDropTime = false;
    _hnsStartDroppingAt = 0;
    _hnsAmountToDrop = 0;
    _fWaitingForCleanPoint = true;
}
