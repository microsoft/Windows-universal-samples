//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once

namespace Microsoft { namespace Samples { namespace SimpleCommunication { namespace Network {
    interface DECLSPEC_UUID("D30E9A4E-31BD-484C-940D-0E5FF536036D") DECLSPEC_NOVTABLE IMediaBufferWrapper : public IUnknown
    {
        IFACEMETHOD_(IMFMediaBuffer *, GetMediaBuffer) () const = 0;
        IFACEMETHOD_(BYTE *, GetBuffer) () const = 0;

        IFACEMETHOD(SetOffset) (DWORD nOffset) = 0;
        IFACEMETHOD_(DWORD, GetOffset) () const = 0;

        IFACEMETHOD(GetCurrentLength) (DWORD *pcbCurrentLength) = 0;
        IFACEMETHOD(SetCurrentLength) (DWORD cbCurrentLength) = 0;

        IFACEMETHOD(TrimLeft) (DWORD cbSize) = 0;
        IFACEMETHOD(TrimRight) (DWORD cbSize, _Out_ IMediaBufferWrapper **pWrapper) = 0;

        IFACEMETHOD(Reset) () = 0;
    };

    interface DECLSPEC_UUID("3FEBFE65-E515-4448-8DB4-10D7DBC030A8") DECLSPEC_NOVTABLE IBufferEnumerator : public IUnknown
    {
        IFACEMETHOD_(bool, IsValid) ();
        IFACEMETHOD(GetCurrent) (_Out_ IMediaBufferWrapper **ppBuffer);
        IFACEMETHOD(MoveNext) ();
        IFACEMETHOD(Reset) ();
    };

    interface DECLSPEC_UUID("E6D1193E-8B68-4526-885A-9C858613807D") DECLSPEC_NOVTABLE IBufferPacket : public IUnknown
    {
        IFACEMETHOD(AddBuffer) (IMediaBufferWrapper *pBuffer) = 0;
        IFACEMETHOD(InsertBuffer) (unsigned int nIndex, _In_ IMediaBufferWrapper *pBuffer) = 0;
        IFACEMETHOD(RemoveBuffer) (unsigned int nIndex, _Outptr_ IMediaBufferWrapper **ppBuffer) = 0;
        IFACEMETHOD(Clear) () = 0;
        IFACEMETHOD_(size_t, GetBufferCount) () const = 0;

        IFACEMETHOD(GetTotalLength) (_Out_ DWORD *pcbTotalLength) = 0;
        IFACEMETHOD(CopyTo) (DWORD nOffset, DWORD cbSize, _In_reads_bytes_(cbSize) void *pDest, _Out_ DWORD *pcbCopied) = 0;
        IFACEMETHOD(MoveLeft) (DWORD cbSize, _Out_writes_bytes_(cbSize) void *pDest) = 0;
        IFACEMETHOD(TrimLeft) (DWORD cbSize) = 0;
        IFACEMETHOD(ToMFSample) (IMFSample **ppSample) = 0;

        IFACEMETHOD(GetEnumerator) (_Out_ IBufferEnumerator **pEnumerator) = 0;
    };

    ref class INetworkChannel abstract
    {
    internal:
        virtual Windows::Foundation::IAsyncAction ^SendAsync(_In_ IBufferPacket *pPacket) = 0;
        virtual Windows::Foundation::IAsyncAction ^ReceiveAsync(_In_ IMediaBufferWrapper *pBuffer) = 0;
        virtual void Close() = 0;
        virtual void Disconnect() = 0;
    };

    interface class INetworkServer
    {
        Windows::Foundation::IAsyncOperation<Windows::Networking::Sockets::StreamSocketInformation^>^ AcceptAsync();
    };

    interface class INetworkClient
    {
        Windows::Foundation::IAsyncAction ^ConnectAsync(String ^url, WORD wPort);
    };

    HRESULT CreateMediaBufferWrapper(DWORD dwMaxLength, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper);
    HRESULT CreateMediaBufferWrapper(_In_ IMFMediaBuffer *pMediaBuffer, _Outptr_ IMediaBufferWrapper **ppMediaBufferWrapper);
    HRESULT CreateBufferPacketFromMFSample(_In_ IMFSample *pSample, _Outptr_ IBufferPacket **ppBufferPacket);
    HRESULT CreateBufferPacket(_Outptr_ IBufferPacket **ppBufferPacket);

    INetworkServer ^CreateNetworkServer(unsigned short listeningPort);
    INetworkClient ^CreateNetworkClient();
}}}}