//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#ifndef DECLSPEC_UUID
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#endif

#ifndef DECLSPEC_NOVTABLE
#define DECLSPEC_NOVTABLE   __declspec(novtable)
#endif

namespace Microsoft { namespace Samples { namespace SimpleCommunication {

interface DECLSPEC_UUID("09CFFBBF-67AC-4AD6-8463-9CF82CA3E52F") DECLSPEC_NOVTABLE IStspSinkInternal : public IUnknown
{
    virtual void TriggerAcceptConnection(DWORD connectionId) = 0;
    virtual void TriggerRefuseConnection(DWORD connectionId) = 0;
};

interface DECLSPEC_UUID("3AC82233-933C-43a9-AF3D-ADC94EABF406") DECLSPEC_NOVTABLE IMarker : public IUnknown
{
    IFACEMETHOD (GetMarkerType) (MFSTREAMSINK_MARKER_TYPE *pType) = 0;
    IFACEMETHOD (GetMarkerValue) (PROPVARIANT *pvar) = 0;
    IFACEMETHOD (GetContext) (PROPVARIANT *pvar) = 0;
};

enum StspOperation
{
    StspOperation_Unknown,
    StspOperation_ClientRequestDescription,
    StspOperation_ClientRequestStart,
    StspOperation_ClientRequestStop,
    StspOperation_ServerDescription,
    StspOperation_ServerSample,
    StspOperation_ServerFormatChange,
    StspOperation_Last,
};

struct StspOperationHeader
{
    DWORD cbDataSize;
    StspOperation eOperation;
};

struct StspStreamDescription
{
    GUID guiMajorType;
    GUID guiSubType;
    DWORD dwStreamId;
    UINT32 cbAttributesSize;
};

struct StspDescription
{
    UINT32 cNumStreams;
    StspStreamDescription aStreams[1];
};

enum StspSampleFlags
{
    StspSampleFlag_BottomFieldFirst = 1,
    StspSampleFlag_CleanPoint = 2,
    StspSampleFlag_DerivedFromTopField = 4,
    StspSampleFlag_Discontinuity = 8,
    StspSampleFlag_Interlaced = 16,
    StspSampleFlag_RepeatFirstField = 32,
    StspSampleFlag_SingleField = 64,
};

struct StspSampleHeader
{
    DWORD dwStreamId;
    LONGLONG ullTimestamp;
    LONGLONG ullDuration;
    DWORD dwFlags;
    DWORD dwFlagMasks;
};

enum StspNetworkType
{
    StspNetworkType_IPv4,
    StspNetworkType_IPv6,
};

// Possible states of the stsp source object
enum SourceState
{
    // Invalid state, source cannot be used 
    SourceState_Invalid,
    // Opening the connection
    SourceState_Opening,
    // Streaming started
    SourceState_Starting,
    // Streaming started
    SourceState_Started,
    // Streanung stopped
    SourceState_Stopped,
    // Source is shut down
    SourceState_Shutdown,
};

extern wchar_t const __declspec(selectany) c_szStspScheme[] = L"stsp";
extern wchar_t const __declspec(selectany) c_szStspSchemeWithColon[] = L"stsp:";
unsigned short const c_wStspDefaultPort = 10010;

void FilterOutputMediaType(IMFMediaType *pSourceMediaType, IMFMediaType *pDestinationMediaType);
void ValidateInputMediaType(REFGUID guidMajorType, REFGUID guidSubtype, IMFMediaType *pMediaType);
HRESULT CreateMarker(    
    MFSTREAMSINK_MARKER_TYPE eMarkerType,
    const PROPVARIANT *pvarMarkerValue,     // Can be NULL.
    const PROPVARIANT *pvarContextValue,    // Can be NULL.
    IMarker **ppMarker
    );
}}} // namespace Microsoft::Samples::SimpleCommunication
