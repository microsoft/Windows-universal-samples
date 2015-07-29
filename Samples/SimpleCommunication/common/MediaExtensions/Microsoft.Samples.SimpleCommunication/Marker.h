//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include "StspDefs.h"

namespace Microsoft { namespace Samples { namespace SimpleCommunication {
class CMarker : public IMarker
{
public:
    static HRESULT Create(
        MFSTREAMSINK_MARKER_TYPE eMarkerType,
        const PROPVARIANT *pvarMarkerValue,
        const PROPVARIANT *pvarContextValue,
        IMarker **ppMarker
        );

    // IUnknown methods.
    IFACEMETHOD (QueryInterface) (REFIID riid, void **ppv);
    IFACEMETHOD_(ULONG, AddRef) ();
    IFACEMETHOD_(ULONG, Release) ();

    IFACEMETHOD (GetMarkerType) (MFSTREAMSINK_MARKER_TYPE *pType);
    IFACEMETHOD (GetMarkerValue) (PROPVARIANT *pvar);
    IFACEMETHOD (GetContext) (PROPVARIANT *pvar);

protected:
    MFSTREAMSINK_MARKER_TYPE _eMarkerType;
    PROPVARIANT _varMarkerValue;
    PROPVARIANT _varContextValue;

private:
    long    _cRef;

    CMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType);
    virtual ~CMarker();
};
}}}