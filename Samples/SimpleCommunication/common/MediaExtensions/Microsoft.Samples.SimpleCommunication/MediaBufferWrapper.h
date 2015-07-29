//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include <StspNetwork.h>
#include <robuffer.h>
#include <windows.storage.streams.h>

namespace Microsoft { namespace Samples { namespace SimpleCommunication { namespace Network {

    class CMediaBufferWrapper : 
        public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
        ABI::Windows::Storage::Streams::IBuffer,
        Microsoft::WRL::CloakedIid<Windows::Storage::Streams::IBufferByteAccess>,
        Microsoft::WRL::CloakedIid<IMarshal>,
        Microsoft::WRL::CloakedIid<IMediaBufferWrapper>,
        Microsoft::WRL::FtmBase>
    {
        InspectableClass(L"Microsoft.Samples.SimpleCommunication.Buffer", BaseTrust);

    public:
        static HRESULT CreateInstance(DWORD dwMaxLength, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper);
        static HRESULT CreateInstance(_In_ IMFMediaBuffer *pMediaBuffer, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper);

        CMediaBufferWrapper(void);
        ~CMediaBufferWrapper(void);

        HRESULT RuntimeClassInitialize(IMFMediaBuffer *pMediaBuffer);

        // IBuffer
        IFACEMETHOD (get_Capacity) (UINT *pcbCapacity);
        IFACEMETHOD (get_Length) (UINT *pcbLength);
        IFACEMETHOD (put_Length) (UINT cbLength);

        // IBufferByteAccess
        IFACEMETHOD (Buffer) (_Out_ BYTE **ppBuffer);

        // IMarshal
        IFACEMETHOD (GetUnmarshalClass) (REFIID riid, _In_opt_ void *pv, DWORD dwDestContext, 
            _Reserved_ void *pvDestContext, DWORD mshlflags, _Out_ CLSID *pclsid);
        IFACEMETHOD (GetMarshalSizeMax) (REFIID riid, _In_opt_ void *pv, DWORD dwDestContext,
            _Reserved_ void *pvDestContext, DWORD mshlflags, _Out_ DWORD *pcbSize);
        IFACEMETHOD (MarshalInterface) (_In_ IStream *pStm, REFIID riid, _In_opt_ void *pv, DWORD dwDestContext,
            _Reserved_ void *pvDestContext, DWORD mshlflags);
        IFACEMETHOD (UnmarshalInterface) (_In_ IStream *, _In_ REFIID, _Outptr_ void **);
        IFACEMETHOD (ReleaseMarshalData) (_In_ IStream *);
        IFACEMETHOD (DisconnectObject) (DWORD);

        // IMediaBufferWrapper
        IFACEMETHOD_(IMFMediaBuffer *, GetMediaBuffer) () const {return _spMediaBuffer.Get();}
        IFACEMETHOD_(BYTE *, GetBuffer) () const {return _pBuffer + _nOffset;}

        IFACEMETHOD (SetOffset) (DWORD nOffset);
        IFACEMETHOD_(DWORD, GetOffset) () const {return _nOffset;}

        IFACEMETHOD (GetCurrentLength) (DWORD *pcbCurrentLength);
        IFACEMETHOD (SetCurrentLength) (DWORD cbCurrentLength);

        IFACEMETHOD (TrimLeft) (DWORD cbSize);
        IFACEMETHOD (TrimRight) (DWORD cbSize, _Out_ IMediaBufferWrapper **pWrapper);

        IFACEMETHOD (Reset) ();

    protected:
        HRESULT CheckMarshal();

    private:
        ComPtr<IMFMediaBuffer>  _spMediaBuffer;
        ComPtr<IMF2DBuffer>     _sp2DBuffer;
        BYTE                    *_pBuffer;
        DWORD                   _nOffset;
        ComPtr<IMarshal>        _spBufferMarshal;
    };

}}}}
