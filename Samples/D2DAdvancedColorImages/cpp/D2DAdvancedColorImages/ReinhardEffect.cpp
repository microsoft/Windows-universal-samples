//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#include "pch.h"
#include <initguid.h>
#include "ReinhardEffect.h"
#include "BasicReaderWriter.h"

#define XML(X) TEXT(#X)

ReinhardEffect::ReinhardEffect() :
    m_refCount(1)
{
}

HRESULT __stdcall ReinhardEffect::CreateReinhardImpl(_Outptr_ IUnknown** ppEffectImpl)
{
    // Since the object's refcount is initialized to 1, we don't need to AddRef here.
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new (std::nothrow) ReinhardEffect());

    if (*ppEffectImpl == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    else
    {
        return S_OK;
    }
}

HRESULT ReinhardEffect::Register(_In_ ID2D1Factory1* pFactory)
{
    // The inspectable metadata of an effect is defined in XML. This can be passed in from an external source
    // as well, however for simplicity we just inline the XML.
    PCWSTR pszXml =
        XML(
            <?xml version='1.0'?>
            <Effect>
                <!-- System Properties -->
                <Property name='DisplayName' type='string' value='Reinhard Tonemapper' />
                <Property name='Author' type='string' value='Microsoft Corporation' />
                <Property name='Category' type='string' value='Stylize' />
                <Property name='Description' type='string' value='Simplified (RGB) version of the Reinhard HDR tonemapper' />
                <Inputs>
                    <Input name='Source' />
                </Inputs>
                <!-- Custom Properties go here -->
            </Effect>
            );

    // This registers the effect with the factory, which will make the effect
    // instantiatable.
    return pFactory->RegisterEffectFromString(
        CLSID_CustomReinhardEffect,
        pszXml,
        nullptr,
        0,
        CreateReinhardImpl
        );
}

IFACEMETHODIMP ReinhardEffect::Initialize(
    _In_ ID2D1EffectContext* pEffectContext,
    _In_ ID2D1TransformGraph* pTransformGraph
    )
{
    // To maintain consistency across different DPIs, this effect needs to cover more pixels at
    // higher than normal DPIs. The context is saved here so the effect can later retrieve the DPI.
    m_effectContext = pEffectContext;
    BasicReaderWriter^ reader = ref new BasicReaderWriter();
    Platform::Array<unsigned char, 1U>^ data;

    try
    {
        data = reader->ReadData("ReinhardEffect.cso");
    }
    catch (Platform::Exception^ e)
    {
        // Return error if file can not be read.
        return e->HResult;
    }

    HRESULT hr = pEffectContext->LoadPixelShader(GUID_ReinhardEffectPixelShader, data->Data, data->Length);

    // This loads the shader into the Direct2D image effects system and associates it with the GUID passed in.
    // If this method is called more than once (say by other instances of the effect) with the same GUID,
    // the system will simply do nothing, ensuring that only one instance of a shader is stored regardless of how
    // many time it is used.
    if (SUCCEEDED(hr))
    {
        // The graph consists of a single transform. In fact, this class is the transform,
        // reducing the complexity of implementing an effect when all we need to
        // do is use a single pixel shader.
        hr = pTransformGraph->SetSingleTransformNode(this);
    }

    return hr;
}

HRESULT ReinhardEffect::UpdateConstants()
{
    // Update the DPI if it has changed. This allows the effect to scale across different DPIs automatically.
    m_effectContext->GetDpi(&m_dpi, &m_dpi);
    m_constants.dpi = m_dpi;

    return m_drawInfo->SetPixelShaderConstantBuffer(reinterpret_cast<BYTE*>(&m_constants), sizeof(m_constants));
}

IFACEMETHODIMP ReinhardEffect::PrepareForRender(D2D1_CHANGE_TYPE changeType)
{
    return UpdateConstants();
}

// SetGraph is only called when the number of inputs changes. This never happens as we publish this effect
// as a single input effect.
IFACEMETHODIMP ReinhardEffect::SetGraph(_In_ ID2D1TransformGraph* pGraph)
{
    return E_NOTIMPL;
}

// Called to assign a new render info class, which is used to inform D2D on
// how to set the state of the GPU.
IFACEMETHODIMP ReinhardEffect::SetDrawInfo(_In_ ID2D1DrawInfo* pDrawInfo)
{
    m_drawInfo = pDrawInfo;

    return m_drawInfo->SetPixelShader(GUID_ReinhardEffectPixelShader);
}

// Calculates the mapping between the output and input rects.
IFACEMETHODIMP ReinhardEffect::MapOutputRectToInputRects(
    _In_ const D2D1_RECT_L* pOutputRect,
    _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
    UINT32 inputRectCount
    ) const
{
    // This effect has exactly one input, so if there is more than one input rect,
    // something is wrong.
    if (inputRectCount != 1)
    {
        return E_INVALIDARG;
    }

    pInputRects[0].left    = pOutputRect->left;
    pInputRects[0].top     = pOutputRect->top;
    pInputRects[0].right   = pOutputRect->right;
    pInputRects[0].bottom  = pOutputRect->bottom;

    return S_OK;
}

IFACEMETHODIMP ReinhardEffect::MapInputRectsToOutputRect(
    _In_reads_(inputRectCount) CONST D2D1_RECT_L* pInputRects,
    _In_reads_(inputRectCount) CONST D2D1_RECT_L* pInputOpaqueSubRects,
    UINT32 inputRectCount,
    _Out_ D2D1_RECT_L* pOutputRect,
    _Out_ D2D1_RECT_L* pOutputOpaqueSubRect
    )
{
    // This effect has exactly one input, so if there is more than one input rect,
    // something is wrong.
    if (inputRectCount != 1)
    {
        return E_INVALIDARG;
    }

    *pOutputRect = pInputRects[0];
    m_inputRect = pInputRects[0];

    // Indicate that entire output might contain transparency.
    ZeroMemory(pOutputOpaqueSubRect, sizeof(*pOutputOpaqueSubRect));

    return S_OK;
}

IFACEMETHODIMP ReinhardEffect::MapInvalidRect(
    UINT32 inputIndex,
    D2D1_RECT_L invalidInputRect,
    _Out_ D2D1_RECT_L* pInvalidOutputRect
    ) const
{
    HRESULT hr = S_OK;

    // Indicate that the entire output may be invalid.
    *pInvalidOutputRect = m_inputRect;

    return hr;
}

IFACEMETHODIMP_(UINT32) ReinhardEffect::GetInputCount() const
{
    return 1;
}

// D2D ensures that that effects are only referenced from one thread at a time.
// To improve performance, we simply increment/decrement our reference count
// rather than use atomic InterlockedIncrement()/InterlockedDecrement() functions.
IFACEMETHODIMP_(ULONG) ReinhardEffect::AddRef()
{
    m_refCount++;
    return m_refCount;
}

IFACEMETHODIMP_(ULONG) ReinhardEffect::Release()
{
    m_refCount--;

    if (m_refCount == 0)
    {
        delete this;
        return 0;
    }
    else
    {
        return m_refCount;
    }
}

// This enables the stack of parent interfaces to be queried.
IFACEMETHODIMP ReinhardEffect::QueryInterface(
    _In_ REFIID riid,
    _Outptr_ void** ppOutput
    )
{
    *ppOutput = nullptr;
    HRESULT hr = S_OK;

    if (riid == __uuidof(ID2D1EffectImpl))
    {
        *ppOutput = reinterpret_cast<ID2D1EffectImpl*>(this);
    }
    else if (riid == __uuidof(ID2D1DrawTransform))
    {
        *ppOutput = static_cast<ID2D1DrawTransform*>(this);
    }
    else if (riid == __uuidof(ID2D1Transform))
    {
        *ppOutput = static_cast<ID2D1Transform*>(this);
    }
    else if (riid == __uuidof(ID2D1TransformNode))
    {
        *ppOutput = static_cast<ID2D1TransformNode*>(this);
    }
    else if (riid == __uuidof(IUnknown))
    {
        *ppOutput = this;
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    if (*ppOutput != nullptr)
    {
        AddRef();
    }

    return hr;
}
