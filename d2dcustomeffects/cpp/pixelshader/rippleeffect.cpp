//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include <initguid.h>
#include "RippleEffect.h"
#include "BasicReaderWriter.h"

#define XML(X) TEXT(#X)

RippleEffect::RippleEffect() :
    m_refCount(1)
{
    m_constants.amplitude = 0;
    m_constants.center = D2D1::Point2F(0, 0);
    m_constants.frequency = 0;
    m_constants.phase = 0;
    m_constants.spread = 0;
}

HRESULT __stdcall RippleEffect::CreateRippleImpl(_Outptr_ IUnknown** ppEffectImpl)
{
    // Since the object's refcount is initialized to 1, we don't need to AddRef here.
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new (std::nothrow) RippleEffect());

    if (*ppEffectImpl == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    else
    {
        return S_OK;
    }
}

HRESULT RippleEffect::Register(_In_ ID2D1Factory1* pFactory)
{
    // The inspectable metadata of an effect is defined in XML. This can be passed in from an external source
    // as well, however for simplicity we just inline the XML.
    PCWSTR pszXml =
        XML(
            <?xml version='1.0'?>
            <Effect>
                <!-- System Properties -->
                <Property name='DisplayName' type='string' value='Ripple'/>
                <Property name='Author' type='string' value='Microsoft Corporation'/>
                <Property name='Category' type='string' value='Stylize'/>
                <Property name='Description' type='string' value='Adds a ripple effect that can be animated'/>
                <Inputs>
                    <Input name='Source'/>
                </Inputs>
                <!-- Custom Properties go here -->
                <Property name='Frequency' type='float'>
                    <Property name='DisplayName' type='string' value='Frequency'/>
                    <Property name='Min' type='float' value='0.0' />
                    <Property name='Max' type='float' value='1000.0' />
                    <Property name='Default' type='float' value='0.0' />
                </Property>
                <Property name='Phase' type='float'>
                    <Property name='DisplayName' type='string' value='Phase'/>
                    <Property name='Min' type='float' value='-100.0' />
                    <Property name='Max' type='float' value='100.0' />
                    <Property name='Default' type='float' value='0.0' />
                </Property>
                <Property name='Amplitude' type='float'>
                    <Property name='DisplayName' type='string' value='Amplitude'/>
                    <Property name='Min' type='float' value='0.0001' />
                    <Property name='Max' type='float' value='1000.0' />
                    <Property name='Default' type='float' value='0.0' />
                </Property>
                <Property name='Spread' type='float'>
                    <Property name='DisplayName' type='string' value='Spread'/>
                    <Property name='Min' type='float' value='0.0001' />
                    <Property name='Max' type='float' value='1000.0' />
                    <Property name='Default' type='float' value='0.0' />
                </Property>
                <Property name='Center' type='vector2'>
                    <Property name='DisplayName' type='string' value='Center'/>
                    <Property name='Default' type='vector2' value='(0.0, 0.0)' />
                </Property>
            </Effect>
            );

    // This defines the bindings from specific properties to the callback functions
    // on the class that ID2D1Effect::SetValue() & GetValue() will call.
    const D2D1_PROPERTY_BINDING bindings[] =
    {
        D2D1_VALUE_TYPE_BINDING(L"Frequency", &SetFrequency, &GetFrequency),
        D2D1_VALUE_TYPE_BINDING(L"Phase", &SetPhase, &GetPhase),
        D2D1_VALUE_TYPE_BINDING(L"Amplitude", &SetAmplitude, &GetAmplitude),
        D2D1_VALUE_TYPE_BINDING(L"Spread", &SetSpread, &GetSpread),
        D2D1_VALUE_TYPE_BINDING(L"Center", &SetCenter, &GetCenter),
    };

    // This registers the effect with the factory, which will make the effect
    // instantiatable.
    return pFactory->RegisterEffectFromString(
        CLSID_CustomRippleEffect,
        pszXml,
        bindings,
        ARRAYSIZE(bindings),
        CreateRippleImpl
        );
}

IFACEMETHODIMP RippleEffect::Initialize(
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
        data = reader->ReadData("RippleEffect.cso");
    }
    catch (Platform::Exception^ e)
    {
        // Return error if file can not be read.
        return e->HResult;
    }

    HRESULT hr = pEffectContext->LoadPixelShader(GUID_RipplePixelShader, data->Data, data->Length);

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

HRESULT RippleEffect::SetFrequency(float frequency)
{
    // Limit to the published possible values in the XML.
    m_constants.frequency = Clamp(frequency, 0.0f, 1000.0f);
    return S_OK;
}

float RippleEffect::GetFrequency() const
{
    return m_constants.frequency;
}

HRESULT RippleEffect::SetPhase(float phase)
{
    // Limit to the published possible values in the XML.
    m_constants.phase = Clamp(phase, -100.0f, 100.0f);
    return S_OK;
}

float RippleEffect::GetPhase() const
{
    return m_constants.phase;
}

HRESULT RippleEffect::SetAmplitude(float amplitude)
{
    // Limit to the published possible values in the XML.
    m_constants.amplitude = Clamp(amplitude, 0.0001f, 1000.0f);
    return S_OK;
}

float RippleEffect::GetSpread() const
{
    return m_constants.spread;
}

HRESULT RippleEffect::SetSpread(float spread)
{
    // Limit to the published possible values in the XML.
    m_constants.spread = Clamp(spread, 0.0001f, 1000.0f);
    return S_OK;
}

float RippleEffect::GetAmplitude() const
{
    return m_constants.amplitude;
}

HRESULT RippleEffect::SetCenter(D2D1_POINT_2F center)
{
    // The valid range is all possible point positions, so no clamping is needed.
    m_constants.center = center;
    return S_OK;
}

D2D1_POINT_2F RippleEffect::GetCenter() const
{
    return m_constants.center;
}

HRESULT RippleEffect::UpdateConstants()
{
    // Update the DPI if it has changed. This allows the effect to scale across different DPIs automatically.
    m_effectContext->GetDpi(&m_dpi, &m_dpi);
    m_constants.dpi = m_dpi;

    return m_drawInfo->SetPixelShaderConstantBuffer(reinterpret_cast<BYTE*>(&m_constants), sizeof(m_constants));
}

IFACEMETHODIMP RippleEffect::PrepareForRender(D2D1_CHANGE_TYPE changeType)
{
    return UpdateConstants();
}

// SetGraph is only called when the number of inputs changes. This never happens as we publish this effect
// as a single input effect.
IFACEMETHODIMP RippleEffect::SetGraph(_In_ ID2D1TransformGraph* pGraph)
{
    return E_NOTIMPL;
}

// Called to assign a new render info class, which is used to inform D2D on
// how to set the state of the GPU.
IFACEMETHODIMP RippleEffect::SetDrawInfo(_In_ ID2D1DrawInfo* pDrawInfo)
{
    m_drawInfo = pDrawInfo;

    return m_drawInfo->SetPixelShader(GUID_RipplePixelShader);
}

// Calculates the mapping between the output and input rects. In this case,
// we want to request an expanded region to account for pixels that the ripple
// may need outside of the bounds of the destination.
IFACEMETHODIMP RippleEffect::MapOutputRectToInputRects(
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

    long expansion = static_cast<long>(Round(m_constants.amplitude));

    // Expand the rect out by the amplitude of the ripple animation. SafeAdd
    // is a helper function that prevents long over/underflows.
    pInputRects[0].left    = SafeAdd(pOutputRect->left, -expansion);
    pInputRects[0].top     = SafeAdd(pOutputRect->top, -expansion);
    pInputRects[0].right   = SafeAdd(pOutputRect->right, expansion);
    pInputRects[0].bottom  = SafeAdd(pOutputRect->bottom, expansion);

    return S_OK;
}

IFACEMETHODIMP RippleEffect::MapInputRectsToOutputRect(
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

IFACEMETHODIMP RippleEffect::MapInvalidRect(
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

IFACEMETHODIMP_(UINT32) RippleEffect::GetInputCount() const
{
    return 1;
}

// D2D ensures that that effects are only referenced from one thread at a time.
// To improve performance, we simply increment/decrement our reference count
// rather than use atomic InterlockedIncrement()/InterlockedDecrement() functions.
IFACEMETHODIMP_(ULONG) RippleEffect::AddRef()
{
    m_refCount++;
    return m_refCount;
}

IFACEMETHODIMP_(ULONG) RippleEffect::Release()
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

// This enables the stack of parent interfaces to be queried. In the instance
// of the Ripple interface, this method simply enables the developer
// to cast a Ripple instance to an ID2D1EffectImpl or IUnknown instance.
IFACEMETHODIMP RippleEffect::QueryInterface(
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
