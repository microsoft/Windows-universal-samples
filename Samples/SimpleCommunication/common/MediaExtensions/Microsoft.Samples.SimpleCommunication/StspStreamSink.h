//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include <CritSec.h>
#include <AsyncCB.h>
#include <linklist.h>
#include <StspNetwork.h>
#include <StspDefs.h>

namespace Microsoft { namespace Samples { namespace SimpleCommunication {

class CMediaSink;

class CStreamSink : public IMFStreamSink, public IMFMediaTypeHandler
{
public:
    // State enum: Defines the current state of the stream.
    enum State
    {
        State_TypeNotSet = 0,    // No media type is set
        State_Ready,             // Media type is set, Start has never been called.
        State_Started,
        State_Stopped,
        State_Paused,
        State_Count              // Number of states
    };

    // StreamOperation: Defines various operations that can be performed on the stream.
    enum StreamOperation
    {
        OpSetMediaType = 0,
        OpStart,
        OpRestart,
        OpPause,
        OpStop,
        OpProcessSample,
        OpPlaceMarker,

        Op_Count                // Number of operations
    };

    // CAsyncOperation:
    // Used to queue asynchronous operations. When we call MFPutWorkItem, we use this
    // object for the callback state (pState). Then, when the callback is invoked,
    // we can use the object to determine which asynchronous operation to perform.

    class CAsyncOperation : public IUnknown
    {
    public:
        CAsyncOperation(StreamOperation op);

        StreamOperation m_op;   // The operation to perform.

        // IUnknown methods.
        STDMETHODIMP QueryInterface(REFIID iid, void **ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();

    private:
        long    _cRef;
        virtual ~CAsyncOperation();
    };

public:
    // IUnknown
    IFACEMETHOD (QueryInterface) (REFIID riid, void **ppv);
    IFACEMETHOD_(ULONG, AddRef) ();
    IFACEMETHOD_(ULONG, Release) ();

    // IMFMediaEventGenerator
    IFACEMETHOD (BeginGetEvent)(IMFAsyncCallback *pCallback,IUnknown *punkState);
    IFACEMETHOD (EndGetEvent) (IMFAsyncResult *pResult, IMFMediaEvent **ppEvent);
    IFACEMETHOD (GetEvent) (DWORD dwFlags, IMFMediaEvent **ppEvent);
    IFACEMETHOD (QueueEvent) (MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, PROPVARIANT const *pvValue);

    // IMFStreamSink
    IFACEMETHOD (GetMediaSink) (IMFMediaSink **ppMediaSink);
    IFACEMETHOD (GetIdentifier) (DWORD *pdwIdentifier);
    IFACEMETHOD (GetMediaTypeHandler) (IMFMediaTypeHandler **ppHandler);
    IFACEMETHOD (ProcessSample) (IMFSample *pSample);

    IFACEMETHOD (PlaceMarker) (
        /* [in] */ MFSTREAMSINK_MARKER_TYPE eMarkerType,
        /* [in] */ PROPVARIANT const *pvarMarkerValue,
        /* [in] */ PROPVARIANT const *pvarContextValue);

    IFACEMETHOD (Flush)();

    // IMFMediaTypeHandler
    IFACEMETHOD (IsMediaTypeSupported) (IMFMediaType *pMediaType, IMFMediaType **ppMediaType);
    IFACEMETHOD (GetMediaTypeCount) (DWORD *pdwTypeCount);
    IFACEMETHOD (GetMediaTypeByIndex) (DWORD dwIndex, IMFMediaType **ppType);
    IFACEMETHOD (SetCurrentMediaType) (IMFMediaType *pMediaType);
    IFACEMETHOD (GetCurrentMediaType) (IMFMediaType **ppMediaType);
    IFACEMETHOD (GetMajorType) (GUID *pguidMajorType);

    // ValidStateMatrix: Defines a look-up table that says which operations
    // are valid from which states.
    static BOOL ValidStateMatrix[State_Count][Op_Count];


    CStreamSink(DWORD dwIdentifier);
    virtual ~CStreamSink();

    HRESULT Initialize(CMediaSink *pParent, Network::INetworkChannel ^sender);

    HRESULT CheckShutdown() const
    {
        if (_IsShutdown)
        {
            return MF_E_SHUTDOWN;
        }
        else
        {
            return S_OK;
        }
    }


    HRESULT     Start(MFTIME start);
    HRESULT     Restart();
    HRESULT     Stop();
    HRESULT     Pause();
    HRESULT     Shutdown();
    bool        IsVideo() const {return _fIsVideo;}
    HRESULT     SetConnected(bool fConnected, LONGLONG llCurrentTime);
    ComPtr<Network::IMediaBufferWrapper>  FillStreamDescription(IMFMediaType *pMediaType, StspStreamDescription *pStreamDescription);

private:
    HRESULT     ValidateOperation(StreamOperation op);

    HRESULT     QueueAsyncOperation(StreamOperation op);

    HRESULT     OnDispatchWorkItem(IMFAsyncResult *pAsyncResult);
    void        DispatchProcessSample(CAsyncOperation *pOp);

    bool        DropSamplesFromQueue();
    bool        SendSampleFromQueue();
    bool        ProcessSamplesFromQueue(bool fFlush);
    void        ProcessFormatChange(IMFMediaType *pMediaType);

    ComPtr<Network::IBufferPacket> PrepareSample(IMFSample *pSample, bool fForce);
    ComPtr<Network::IBufferPacket> PrepareFormatChange(IMFMediaType *pMediaType);

    HRESULT     AppendParameterSets(IMFSample *pCopyFrom, Network::IBufferPacket *pPacket);
    void        HandleError(HRESULT hr);

private:
    long                        _cRef;                      // reference count
    CritSec                     _critSec;                   // critical section for thread safety

    DWORD                       _dwIdentifier;
    State                       _state;
    bool                        _IsShutdown;                // Flag to indicate if Shutdown() method was called.
    bool                        _Connected;
    bool                        _fGetStartTimeFromSample;
    bool                        _fIsVideo;
    bool                        _fWaitingForFirstSample;
    bool                        _fFirstSampleAfterConnect;
    GUID                        _guiCurrentSubtype;

    DWORD                       _WorkQueueId;               // ID of the work queue for asynchronous operations.
    MFTIME                      _StartTime;                 // Presentation time when the clock started.

    ComPtr<IMFMediaSink>        _spSink;                    // Parent media sink
    CMediaSink                  *_pParent;

    ComPtr<IMFMediaEventQueue>  _spEventQueue;              // Event queue
    ComPtr<IMFByteStream>       _spByteStream;              // Bytestream where we write the data.
    ComPtr<IMFMediaType>        _spCurrentType;
    ComPtr<IMFSample>           _spFirstVideoSample;

    ComPtrList<IUnknown>        _SampleQueue;               // Queue to hold samples and markers.
                                                            // Applies to: ProcessSample, PlaceMarker

    Network::INetworkChannel^    _networkSender;

    AsyncCallback<CStreamSink>  _WorkQueueCB;              // Callback for the work queue.

    ComPtr<IUnknown>            _spFTM;
};

}}} // namespace Microsoft::Samples::SimpleCommunication
