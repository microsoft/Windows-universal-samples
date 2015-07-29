//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#include "pch.h"

#include "Marker.h"

using namespace Microsoft::Samples::SimpleCommunication;

HRESULT Microsoft::Samples::SimpleCommunication::CreateMarker(    
    MFSTREAMSINK_MARKER_TYPE eMarkerType,
    const PROPVARIANT *pvarMarkerValue,     // Can be NULL.
    const PROPVARIANT *pvarContextValue,    // Can be NULL.
    IMarker **ppMarker
    )
{
    return CMarker::Create(eMarkerType, pvarMarkerValue, pvarContextValue, ppMarker);
}

CMarker::CMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType) : _cRef(1), _eMarkerType(eMarkerType)
{
    ZeroMemory(&_varMarkerValue, sizeof(_varMarkerValue));
    ZeroMemory(&_varContextValue, sizeof(_varContextValue));
}

CMarker::~CMarker()
{
    assert(_cRef == 0);

    PropVariantClear(&_varMarkerValue);
    PropVariantClear(&_varContextValue);
}

/* static */
HRESULT CMarker::Create(
    MFSTREAMSINK_MARKER_TYPE eMarkerType,
    const PROPVARIANT *pvarMarkerValue,     // Can be NULL.
    const PROPVARIANT *pvarContextValue,    // Can be NULL.
    IMarker **ppMarker
    )
{
    if (ppMarker == nullptr)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    ComPtr<CMarker> spMarker;
        
    spMarker.Attach(new (std::nothrow) CMarker(eMarkerType));

    if (spMarker == nullptr)
    {
        hr = E_OUTOFMEMORY;
    }

    // Copy the marker data.
    if (SUCCEEDED(hr))
    {
        if (pvarMarkerValue)
        {
            hr = PropVariantCopy(&spMarker->_varMarkerValue, pvarMarkerValue);
        }
    }

    if (SUCCEEDED(hr))
    {
        if (pvarContextValue)
        {
            hr = PropVariantCopy(&spMarker->_varContextValue, pvarContextValue);
        }
    }

    if (SUCCEEDED(hr))
    {
        *ppMarker = spMarker.Detach();
    }

    return hr;
}

// IUnknown methods.

IFACEMETHODIMP_(ULONG) CMarker::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

IFACEMETHODIMP_(ULONG) CMarker::Release()
{
    ULONG cRef = InterlockedDecrement(&_cRef);
    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

IFACEMETHODIMP CMarker::QueryInterface(REFIID riid, void **ppv)
{
    if (ppv == nullptr)
    {
        return E_POINTER;
    }
    (*ppv) = nullptr;

    HRESULT hr = S_OK;
    if (riid == IID_IUnknown || riid == __uuidof(IMarker))
    {
        (*ppv) = static_cast<IMarker*>(this);
        AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

// IMarker methods
IFACEMETHODIMP CMarker::GetMarkerType(MFSTREAMSINK_MARKER_TYPE *pType)
{
    if (pType == NULL)
    {
        return E_POINTER;
    }

    *pType = _eMarkerType;
    return S_OK;
}

IFACEMETHODIMP CMarker::GetMarkerValue(PROPVARIANT *pvar)
{
    if (pvar == NULL)
    {
        return E_POINTER;
    }
    return PropVariantCopy(pvar, &_varMarkerValue);

}
IFACEMETHODIMP CMarker::GetContext(PROPVARIANT *pvar)
{
    if (pvar == NULL)
    {
        return E_POINTER;
    }
    return PropVariantCopy(pvar, &_varContextValue);
}
