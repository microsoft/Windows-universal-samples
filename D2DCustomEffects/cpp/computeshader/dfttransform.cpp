//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "DftTransform.h"
#include "BasicReaderWriter.h"

// These values should match those in the respective shaders.
static const int CS4_numThreadsX = 24;
static const int CS4_numThreadsY = 24;
static const int CS5_numThreadsX = 32;
static const int CS5_numThreadsY = 32;

DftTransform::DftTransform(_In_ ID2D1EffectContext* context, TransformType dftType) :
    m_cRef(0),
    m_dftType(dftType)
{
    // Load the Shader Model 5 shader if the device supports it (DX 11_0), otherwise
    // fall back to Shader Model 4.
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0 };
    HRESULT hr = context->GetMaximumSupportedFeatureLevel(levels, ARRAYSIZE(levels), &m_maxLevelSupported);

    if (SUCCEEDED(hr))
    {
        BasicReaderWriter^ reader = ref new BasicReaderWriter();

        GUID shaderGUID;
        Platform::String^ shaderFile = "";

        if (m_maxLevelSupported == D3D_FEATURE_LEVEL_11_0)
        {
            if (dftType == TransformType::Horizontal)
            {
                shaderGUID = GUID_DFT_H_CS5;
                shaderFile = "DFT_H_CS5.cso";
            }
            else
            {
                shaderGUID = GUID_DFT_V_CS5;
                shaderFile = "DFT_V_CS5.cso";
            }
        }
        else if (m_maxLevelSupported == D3D_FEATURE_LEVEL_10_0)
        {
            if (dftType == TransformType::Horizontal)
            {
                shaderGUID = GUID_DFT_H_CS4;
                shaderFile = "DFT_H_CS4.cso";
            }
            else
            {
                shaderGUID = GUID_DFT_V_CS4;
                shaderFile = "DFT_V_CS4.cso";
            }
        }

        auto data = reader->ReadData(shaderFile);
        hr = context->LoadComputeShader(shaderGUID, data->Data, data->Length);
    }

    if (FAILED(hr))
    {
        throw Platform::Exception::CreateException(hr);
    }
}

IFACEMETHODIMP DftTransform::MapInvalidRect(
    UINT32 inputIndex,
    D2D1_RECT_L invalidInputRect,
    _Out_ D2D1_RECT_L* pInvalidOutputRect
    ) const
{
    // Due to the nature of this effect, any change in the input file will affect the entire
    // output. To implement this, the effect sets the invalidOutputRect to the
    // entire input rect.
    *pInvalidOutputRect = m_inputRect;
    return S_OK;
}

IFACEMETHODIMP DftTransform::MapOutputRectToInputRects(
    _In_ const D2D1_RECT_L* pOutputRect,
    _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
    UINT32 inputRectCount
    ) const
{
    // Return an error if more than one input specified for transform.
    if (inputRectCount != 1)
    {
        return E_INVALIDARG;
    }

    // Most effects will only need the region of the input that directly corresponds to the output,
    // and will want to set the inputRect accordingly. Due to the nature of this algorithm, the
    // effect needs to process the entire input, even if the output is not completely visible.
    // To do so, it sets the inputRect to be the previously reported input (in MapInputRectsToOutputRect),
    // instead of the requested output.
    pInputRects[0] = m_inputRect;

    return S_OK;
}

IFACEMETHODIMP DftTransform::MapInputRectsToOutputRect(
    _In_reads_(inputRectCount) const D2D1_RECT_L* pInputRects,
    _In_reads_(inputRectCount) const D2D1_RECT_L* pInputOpaqueSubRects,
    UINT32 inputRectCount,
    _Out_ D2D1_RECT_L* pOutputRect,
    _Out_ D2D1_RECT_L* pOutputOpaqueSubRect
    )
{
    // Return an error if more than one input is specified for transform.
    if (inputRectCount != 1)
    {
        return E_INVALIDARG;
    }

    // The output of the transform will be the same size as the input.
    *pOutputRect = pInputRects[0];

    // Save the size of the input for use in MapOutputRectToInputRects
    // and MapInvalidRect. This represents the size of the entire input.
    m_inputRect = pInputRects[0];

    // Because the effect output is merely a graph with a guaranteed alpha of '1',
    // the effect can mark the entire output as opaque. This does not affect the
    // output image itself, but allows Direct2D to use various optimizations, such
    // as using a more simple blend mode.
    *pOutputOpaqueSubRect = *pOutputRect;

    return S_OK;
}

IFACEMETHODIMP DftTransform::CalculateThreadgroups(
    _In_ const D2D1_RECT_L* pOutputRect,
    _Out_ UINT32* pDimensionX,
    _Out_ UINT32* pDimensionY,
    _Out_ UINT32* pDimensionZ
    )
{
    // The dimensions specified here are what Direct2D uses to calculate how many instances of the shader to
    // call (said another way, which parameters are passed to the Direct3D Dispatch call). NumThreads specifies how many pixels
    // one instance of a shader operates on, while the Dimension parameter here specifies how many instances of the
    // shader to call. In this case we divide the width and height of the image by the corresponding NumThreads
    // to determine how many times we need to call the shader.

    // If the image is not perfectly divisible by the numThreads values,
    // the app should round it up to the next whole number then manually
    // check in the shader when the current thread is outside the image's bounds.
    // If the value is not rounded up, the remainder pixels will not be processed.

    *pDimensionX = 0;
    *pDimensionY = 0;

    if (m_maxLevelSupported == D3D_FEATURE_LEVEL_11_0)
    {
        *pDimensionX = static_cast<UINT32>(ceil((m_inputRect.right - m_inputRect.left) / static_cast<float>(CS5_numThreadsX)));
        *pDimensionY = static_cast<UINT32>(ceil((m_inputRect.bottom - m_inputRect.top) / static_cast<float>(CS5_numThreadsY)));
    }
    else if (m_maxLevelSupported == D3D_FEATURE_LEVEL_10_0)
    {
        *pDimensionX = static_cast<UINT32>(ceil((m_inputRect.right - m_inputRect.left) / static_cast<float>(CS4_numThreadsX)));
        *pDimensionY = static_cast<UINT32>(ceil((m_inputRect.bottom - m_inputRect.top) / static_cast<float>(CS4_numThreadsY)));
    }

    *pDimensionZ = 1;

    return S_OK;
}

IFACEMETHODIMP DftTransform::SetComputeInfo(_In_ ID2D1ComputeInfo* pComputeInfo)
{
    HRESULT hr = S_OK;

    UINT32 instructionCount = 0;

    if (m_dftType == TransformType::Horizontal)
    {
        // The output of the horizontal shader is best represented as a float. This thus instructs
        // Direct2D to set the type of the output buffer to 32 bit floats.
        hr = pComputeInfo->SetOutputBuffer(D2D1_BUFFER_PRECISION_32BPC_FLOAT, D2D1_CHANNEL_DEPTH_4);

        if (SUCCEEDED(hr))
        {
            if (m_maxLevelSupported == D3D_FEATURE_LEVEL_11_0)
            {
                hr = pComputeInfo->SetComputeShader(GUID_DFT_H_CS5);
            }
            else
            {
                hr = pComputeInfo->SetComputeShader(GUID_DFT_H_CS4);
            }
        }
    }
    else
    {
        // Update constant buffer b1 with the magnitude scale factor of our choosing. Constant
        // buffers can be updated as often as the transform wants, such as on property changes.
        m_constantBuffer.magnitudeScale = 500.0f;
        hr = pComputeInfo->SetComputeShaderConstantBuffer(reinterpret_cast<const BYTE*>(&m_constantBuffer), sizeof(m_constantBuffer));

        if (SUCCEEDED(hr))
        {
            if (m_maxLevelSupported == D3D_FEATURE_LEVEL_11_0)
            {
                hr = pComputeInfo->SetComputeShader(GUID_DFT_V_CS5);
            }
            else
            {
                hr = pComputeInfo->SetComputeShader(GUID_DFT_V_CS4);
            }
        }
    }

    if (SUCCEEDED(hr))
    {
        // Providing this hint allows D2D to optimize performance when processing large images.
        pComputeInfo->SetInstructionCountHint(instructionCount);
    }

    return hr;
}

IFACEMETHODIMP_(ULONG) DftTransform::AddRef()
{
    // D2D ensures that that effects are only referenced from one thread at a time.
    // To improve performance, we simply increment/decrement our reference count
    // rather than use atomic InterlockedIncrement()/InterlockedDecrement() functions.
    m_cRef++;

    return m_cRef;
}

IFACEMETHODIMP_(ULONG) DftTransform::Release()
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

IFACEMETHODIMP DftTransform::QueryInterface(
    REFIID riid,
    _Outptr_ void** ppOutput
    )
{
    *ppOutput = nullptr;
    HRESULT hr = S_OK;

    if (riid == __uuidof(ID2D1ComputeTransform))
    {
        *ppOutput = static_cast<ID2D1ComputeTransform*>(this);
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