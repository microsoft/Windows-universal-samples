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
#include "DftEffect.h"

// Helper macro for defining XML strings.
#define XML(X) TEXT(#X)

// This effect demonstrates how to implement a basic Discrete
// Fourier Transform (DFT) custom Direct2D effect using DirectCompute.
DftEffect::DftEffect() :
    m_cRef(0)
{
}

// Register the effect, its properties, and its creation method with D2D.
HRESULT DftEffect::Register(_In_ ID2D1Factory1* pFactory)
{
    PCWSTR pszXml =
        XML(
            <?xml version='1.0'?>
            <Effect>
                <!-- System Properties -->
                <Property name='DisplayName' type='string' value='Sample Discrete Fourier Transform (DFT) Compute Effect'/>
                <Property name='Author' type='string' value='Microsoft Corporation'/>
                <Property name='Category' type='string' value='Transform'/>
                <Property name='Description' type='string'
                    value='A sample compute shader effect that computes the Discrete Fourier Transform of a given input.'/>
                <Inputs>
                    <Input name='Source'/>
                </Inputs>
            </Effect>
            );

    return pFactory->RegisterEffectFromString(CLSID_CustomDftEffect, pszXml, nullptr, 0, Create);
}

// A static method to create and return an instance of the effect.
HRESULT __stdcall DftEffect::Create(_Outptr_ IUnknown** ppEffectImpl)
{
    // Use the nothrow syntax so that we can return an HRESULT in the event of an allocation failure.
    // No try/catch is necessary here since there is no code / no possible exceptions in the constructor.
    *ppEffectImpl = static_cast<ID2D1EffectImpl *>(new (std::nothrow) DftEffect());

    if (*ppEffectImpl == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    else
    {
        (*ppEffectImpl)->AddRef();
        return S_OK;
    }
}

// This effect method is called first, after the effect is instantiated with Create().
IFACEMETHODIMP DftEffect::Initialize(
    _In_ ID2D1EffectContext* pEffectContext,
    _In_ ID2D1TransformGraph* pTransformGraph
    )
{
    // Check for compute shader support - DX Feature Level 10_0 is not guaranteed to support compute shaders.
    // Immediately return if compute is not supported.
    D2D1_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hardwareOptions;
    HRESULT hr = pEffectContext->CheckFeatureSupport(
        D2D1_FEATURE_D3D10_X_HARDWARE_OPTIONS,
        &hardwareOptions,
        sizeof(hardwareOptions)
        );

    // As stated above, not all DX Feature Level 10_0 parts support compute shaders. In this sample's case,
    // it checks for compute shader support at device creation in DeviceResources. If support is missing,
    // it uses the WARP software fallback, which is guaranteed to support compute.
    // All effects that use compute shaders should perform this check at instantiation.
    if (SUCCEEDED(hr))
    {
        if (!hardwareOptions.computeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
        {
            return D2DERR_INSUFFICIENT_DEVICE_CAPABILITIES;
        }
    }
    else
    {
        return hr;
    }

    // Create new Transform nodes. The vertical transform must run after all of the horizontal code
    // has run, so it is performed in its own transform. Smart pointers are used here to avoid memory leaks
    // in the event of a failure.
    Microsoft::WRL::ComPtr<DftTransform> dftHorizontalTransform;
    Microsoft::WRL::ComPtr<DftTransform> dftVerticalTransform;

    if (SUCCEEDED(hr))
    {
        try
        {
            dftHorizontalTransform = new (std::nothrow) DftTransform(pEffectContext, TransformType::Horizontal);
            dftVerticalTransform = new (std::nothrow) DftTransform(pEffectContext, TransformType::Vertical);
            if (dftHorizontalTransform == nullptr || dftVerticalTransform == nullptr)
            {
                return E_OUTOFMEMORY;
            }
        }
        catch (Platform::Exception^ e)
        {
            // Return HRESULT if transform throws exception during creation.
            return e->HResult;
        }
    }

    if (SUCCEEDED(hr))
    {
        // Add horizontal transform node to the graph. Still need to connect its inputs/outputs (done below).
        hr = pTransformGraph->AddNode(dftHorizontalTransform.Get());
    }

    if (SUCCEEDED(hr))
    {
        // Connect the input of this transform node to the parent effect's input.
        hr = pTransformGraph->ConnectToEffectInput(0, dftHorizontalTransform.Get(), 0);
    }

    if (SUCCEEDED(hr))
    {
        // Add vertical transform node to the graph. Still need to connect its inputs/outputs (done below).
        // Logically, this transform relies on the horizontal transform's output, and must be located
        // after it in the effect graph.
        hr = pTransformGraph->AddNode(dftVerticalTransform.Get());
    }

    if (SUCCEEDED(hr))
    {
        // Connects the input of the DFTV transform node to the output of the DFTH transform node.
        hr = pTransformGraph->ConnectNode(dftHorizontalTransform.Get(), dftVerticalTransform.Get(), 0);
    }

    if (SUCCEEDED(hr))
    {
        // Designates the output of the DFTV transform node as the output of the effect.
        hr = pTransformGraph->SetOutputNode(dftVerticalTransform.Get());
    }

    return hr;
}

// This method provides the effect implementation with an interface for
// specifying its transform graph and transform graph changes. This will be
// called when the effect is first initialized, and if the number of inputs
// to the effect changes (variable-input effects).
IFACEMETHODIMP DftEffect::SetGraph(_In_ ID2D1TransformGraph* pTransformGraph)
{
    // Because this effect only has one input, it doesn't need to do anything here.
    return E_NOTIMPL;
}

// During this method, the effect must finalize the topology of its internal
// transform graph.  As in this example effect, the transform graph may already
// have been configured by the effect's Initialize method.
//
// This effect method is called before rendering the effect but after
// property changes occur.
IFACEMETHODIMP DftEffect::PrepareForRender(D2D1_CHANGE_TYPE changeType)
{
    return S_OK;
}

IFACEMETHODIMP_(ULONG) DftEffect::AddRef()
{
    // D2D ensures that that effects are only referenced from one thread at a time.
    // To improve performance, we simply increment/decrement our reference count
    // rather than use atomic InterlockedIncrement()/InterlockedDecrement() functions.
    m_cRef++;

    return m_cRef;
}

IFACEMETHODIMP_(ULONG) DftEffect::Release()
{
    m_cRef--;

    if (m_cRef == 0)
    {
        delete this;
        return 0;
    }
    else
    {
        return m_cRef;
    }
}

IFACEMETHODIMP DftEffect::QueryInterface(
    REFIID riid,
    _Outptr_ void** ppOutput
    )
{
    *ppOutput = nullptr;
    HRESULT hr = S_OK;

    if (riid == __uuidof(ID2D1EffectImpl))
    {
        *ppOutput = static_cast<ID2D1EffectImpl*>(this);
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