//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "DftTransform.h"

// Define a unique GUID for the effect.
DEFINE_GUID(CLSID_CustomDftEffect, 0x88F0F871, 0x18A3, 0x4C55, 0xB5, 0x28, 0xE7, 0x38, 0xA7, 0x81, 0x35, 0x1F);

class DftEffect : public ID2D1EffectImpl
{
public:
    // Effect Registration
    static HRESULT Register(_In_ ID2D1Factory1* pFactory);
    static HRESULT __stdcall Create(_Outptr_ IUnknown** ppEffectImpl);

    // ID2D1EffectImpl Methods:
    IFACEMETHODIMP Initialize(
        _In_ ID2D1EffectContext* pEffectContext,
        _In_ ID2D1TransformGraph* pTransformGraph
        );

    IFACEMETHODIMP PrepareForRender(D2D1_CHANGE_TYPE changeType);
    IFACEMETHODIMP SetGraph(_In_ ID2D1TransformGraph* pTransformGraph);

    // IUnknown Methods:
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
    IFACEMETHODIMP QueryInterface(REFIID riid, _Outptr_ void** ppOutput);

private:
    DftEffect();

    LONG m_cRef;
};