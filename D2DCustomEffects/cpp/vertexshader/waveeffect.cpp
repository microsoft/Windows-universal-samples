//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include <initguid.h>
#include "WaveEffect.h"
#include "BasicReaderWriter.h"

#define XML(X) TEXT(#X)

static const float WaveMarginLeft = 0.25f;
static const float WaveMarginTop = 0.20f;
static const float WaveMarginRight = 0.25f;
static const float WaveMarginBottom = 0.40f;
static const float WaveHeight = 60;
static const float SkewOffset = -0.4f;
static const unsigned int TessellationAmount = 32;

HRESULT __stdcall WaveEffect::CreateWaveImpl(_Outptr_ IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new (std::nothrow) WaveEffect());

    if (*ppEffectImpl == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    else
    {
        return S_OK;
    }
}

HRESULT WaveEffect::Register(_In_ ID2D1Factory1* pFactory)
{
    PCWSTR propertyXml =
        XML(
            <?xml version='1.0'?>
            <Effect>
                <!-- System Properties -->
                <Property name='DisplayName' type='string' value='Wave Effect'/>
                <Property name='Author' type='string' value='Microsoft Corporation'/>
                <Property name='Category' type='string' value='Sample'/>
                <Property name='Description' type='string' value='Animation of a wave.'/>
                <Inputs>
                    <Input name='Source'/>
                </Inputs>
                <!-- Effect-specific Properties -->
                <Property name='WaveOffset' type='float' value='0'>
                    <Property name='DisplayName' type='string' value='Wave Offset'/>
                    <Property name='Default' type='float' value='0.0'/>
                </Property>
                <Property name='SkewX' type='float' value='0'>
                    <Property name='DisplayName' type='string' value='Skew (X-Axis)'/>
                    <Property name='Min' type='float' value='-5.0'/>
                    <Property name='Max' type='float' value='5.0'/>
                    <Property name='Default' type='float' value='0.0'/>
                </Property>
                <Property name='SkewY' type='float' value='0'>
                    <Property name='DisplayName' type='string' value='Skew (Y-Axis)'/>
                    <Property name='Min' type='float' value='-5.0'/>
                    <Property name='Max' type='float' value='5.0'/>
                    <Property name='Default' type='float' value='0.0'/>
                </Property>
            </Effect>
            );

    // Property bindings are performed automatically for the system properties.
    // For effect-specific properties (i.e. WaveOffset), you must direct
    // Direct2D to the specific method in your implementation for manipulating
    // the property. This is done by assigning each property a 'setter' and 'getter'
    // in this table. Direct2D provides a set of helpers for doing this in
    // D2D1EffectHelpers.h, such as D2D1_VALUE_TYPE_BINDING used here.
    D2D1_PROPERTY_BINDING bindings[] =
    {
        D2D1_VALUE_TYPE_BINDING(L"WaveOffset",  &SetWaveOffset,  &GetWaveOffset),
        D2D1_VALUE_TYPE_BINDING(L"SkewX",  &SetSkewX,  &GetSkewX),
        D2D1_VALUE_TYPE_BINDING(L"SkewY",  &SetSkewY,  &GetSkewY)
    };

    // Register the effect using the data defined above.
    return pFactory->RegisterEffectFromString(
        CLSID_CustomWaveEffect,
        propertyXml,
        bindings,
        ARRAYSIZE(bindings),
        CreateWaveImpl
        );
}

WaveEffect::WaveEffect() :
    m_refCount(1),
    m_waveOffset(0),
    m_skewX(0),
    m_skewY(0),
    m_sizeX(0),
    m_sizeY(0)
{
}

// This method is where as many resources should be initialized as possible.
// While some resources may change over time and cannot be initialized until
// the PrepareForRender() call, initializing resources in this method
// reduces the amount of processing needed during rendering, improving the
// performance of your effect.
HRESULT WaveEffect::Initialize(
    _In_ ID2D1EffectContext* pEffectContext,
    _In_ ID2D1TransformGraph* pTransformGraph
    )
{
    BasicReaderWriter^ reader = ref new BasicReaderWriter();

    // Read compiled shader data instructions. Visual Studio automatically generates
    // the cso (Compiled Shader Object) file at compile time from the HLSL.

    Platform::Array<unsigned char, 1U>^ data;

    try
    {
        data = reader->ReadData("WaveEffect.cso");
    }
    catch (Platform::Exception^ e)
    {
        // Return error if file can not be read.
        return e->HResult;
    }

    HRESULT hr = pEffectContext->LoadVertexShader(GUID_WaveTransformVertexShader, data->Data, data->Length);

    if (SUCCEEDED(hr))
    {
        // Only generate the vertex buffer if it has not already been initialized.
        pEffectContext->FindVertexBuffer(&GUID_WaveTransformVertexBuffer, &m_vertexBuffer);
        if (m_vertexBuffer == nullptr)
        {
            Vertex* mesh;
            hr = GenerateMesh(&mesh);

            if (SUCCEEDED(hr))
            {
                // Updating geometry every time the effect is rendered can be costly, so it is
                // recommended that vertex buffer remain static if possible (which it is in this
                // sample effect).
                D2D1_VERTEX_BUFFER_PROPERTIES vbProp = {0};
                vbProp.byteWidth = sizeof(Vertex) * m_numVertices;
                vbProp.data = reinterpret_cast<BYTE*>(mesh);
                vbProp.inputCount = 1;
                vbProp.usage =  D2D1_VERTEX_USAGE_STATIC;

                static const D2D1_INPUT_ELEMENT_DESC vertexLayout[] =
                {
                    {"MESH_POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0},
                };

                D2D1_CUSTOM_VERTEX_BUFFER_PROPERTIES cvbProp = {0};
                cvbProp.elementCount = ARRAYSIZE(vertexLayout);
                cvbProp.inputElements = vertexLayout;
                cvbProp.stride = sizeof(Vertex);
                cvbProp.shaderBufferWithInputSignature = data->Data;
                cvbProp.shaderBufferSize = data->Length;

                // The GUID is optional, and is provided here to register the geometry globally.
                // As mentioned above, this avoids duplication if multiple versions of the effect
                // are created.
                hr = pEffectContext->CreateVertexBuffer(
                    &vbProp,
                    &GUID_WaveTransformVertexBuffer,
                    &cvbProp,
                    &m_vertexBuffer
                    );
            }

            // Since mesh has already been transferred to GPU, it can be removed.
            delete[] mesh;
        }
    }

    hr = pTransformGraph->SetSingleTransformNode(this);

    return hr;
}

HRESULT WaveEffect::GenerateMesh(_Outptr_ Vertex** ppMesh)
{
    m_numVertices = 6 * TessellationAmount * TessellationAmount;

    Vertex *mesh = new (std::nothrow) Vertex[m_numVertices];

    if (mesh == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    float offset = 1.0f / TessellationAmount;

    for (int i = 0; i < TessellationAmount; i++)
    {
        for (int j = TessellationAmount - 1; j >= 0; j--)
        {
            int index = (i * TessellationAmount + j) * 6;

            // This mesh consists of pairs of triangles forming squares. Each square contains
            // six vertices, three for each triangle. 'offset' represents the distance between each vertice
            // in the triangles. In this mesh, we only set the x and y coordinates of the vertices, since
            // they are the only variable part of the geometry. In the vertex shader, z is generated
            // based on x, and w is defined to be '1'. The actual coordinates here range from 0 to 1,
            // these values are scaled up based on the size of the image in the vertex shader.

            mesh[index].x     = i * offset;
            mesh[index].y     = j * offset;
            mesh[index + 1].x = i * offset;
            mesh[index + 1].y = j * offset + offset;
            mesh[index + 2].x = i * offset + offset;
            mesh[index + 2].y = j * offset;
            mesh[index + 3].x = i * offset + offset;
            mesh[index + 3].y = j * offset;
            mesh[index + 4].x = i * offset;
            mesh[index + 4].y = j * offset + offset;
            mesh[index + 5].x = i * offset + offset;
            mesh[index + 5].y = j * offset + offset;
        }
    }

    *ppMesh = mesh;

    return S_OK;
}

// Called on first render and/or when the context changes.
IFACEMETHODIMP WaveEffect::PrepareForRender(D2D1_CHANGE_TYPE changeType)
{
    return UpdateConstants();
}

// The effect properties are not automatically clamped based on the
// registration XML. Effects should manually implement the min and
// max specified at registration. These functions are called by Direct2D itself,
// based on the property specified in the ID2D1Effect::SetValue call.
HRESULT WaveEffect::SetSkewX(float skew)
{
    m_skewX = Clamp(skew, -5.0f, 5.0f);
    return S_OK;
}

HRESULT WaveEffect::SetSkewY(float skew)
{
    m_skewY = Clamp(skew, -5.0f, 5.0f);
    return S_OK;
}

// The wave offset is not clamped since no min or max is specified.
HRESULT WaveEffect::SetWaveOffset(float waveOffset)
{
    m_waveOffset = waveOffset;
    return S_OK;
}

float WaveEffect::GetSkewX() const
{
    return m_skewX;
}

float WaveEffect::GetSkewY() const
{
    return m_skewY;
}

float WaveEffect::GetWaveOffset() const
{
    return m_waveOffset;
}

HRESULT WaveEffect::UpdateConstants()
{
    // Update constant buffer 1 (the first constant buffer available to the effect)
    // with the progress, skew, and size values.

    m_constants.matrix = D2D1::Matrix4x4F::RotationX(3);
    m_constants.sizeX = m_sizeX;
    m_constants.sizeY = m_sizeY;
    m_constants.waveOffset = m_waveOffset;
    m_constants.skewX = m_skewX;
    m_constants.skewY = m_skewY;

    return m_drawInfo->SetVertexShaderConstantBuffer(reinterpret_cast<const BYTE*>(&m_constants), sizeof(m_constants));

}

IFACEMETHODIMP WaveEffect::MapOutputRectToInputRects(
    _In_ const D2D1_RECT_L* pOutputRect,
    _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
    UINT32 inputRectCount
    ) const
{
    if (inputRectCount != 1)
    {
        return E_FAIL;
    }

    pInputRects[0] = m_inputRect;

    return S_OK;
}

IFACEMETHODIMP WaveEffect::MapInputRectsToOutputRect(
    _In_reads_(inputRectCount) CONST D2D1_RECT_L* pInputRects,
    _In_reads_(inputRectCount) CONST D2D1_RECT_L* pInputOpaqueSubRects,
    UINT32 inputRectCount,
    _Out_ D2D1_RECT_L* pOutputRect,
    _Out_ D2D1_RECT_L* pOutputOpaqueSubRect
    )
{
    HRESULT hr = S_OK;

    if (inputRectCount != 1)
    {
        return E_FAIL;
    }

    m_inputRect = pInputRects[0];

    // Store the size of the rect so we can pass it into the vertex shader later.
    int newSizeX = pInputRects[0].right - pInputRects[0].left;
    int newSizeY = pInputRects[0].bottom - pInputRects[0].top;

    if (m_sizeX != newSizeX || m_sizeY != newSizeY)
    {
        m_sizeX = static_cast<float>(newSizeX);
        m_sizeY = static_cast<float>(newSizeY);
        hr = UpdateConstants();
    }

    if (SUCCEEDED(hr))
    {
        // Based on how the vertex shader is deforming the image, we can adjust the output rect to
        // avoid unnecessary rendering/blending. While simply setting the output rect to the input
        // rect is the simplest solution, there will be rendering errors if the custom effect is
        // subsequently transformed such that content that was previously outside screen bounds
        // should now be visible (i.e. a translation). If no geometric transformation is done to
        // an image, the outputRect can simply be assigned to the inputRect (*outputRect = m_inputRect)

        long inputRectHeight = m_inputRect.bottom - m_inputRect.top;
        long inputRectWidth = m_inputRect.right - m_inputRect.left;

        // These values are based on the transforms done in the vertex shader itself.

        // SkewOffset is the point where the wave is perpendicular to the screen.
        if (m_skewY > SkewOffset)
        {
            pOutputRect->top    = static_cast<long>(m_inputRect.top +    (inputRectHeight * WaveMarginTop) - WaveHeight);
            pOutputRect->bottom = static_cast<long>(m_inputRect.bottom - (inputRectHeight * WaveMarginBottom) + WaveHeight + (inputRectHeight * m_skewY));
        }
        else
        {
            pOutputRect->top    = static_cast<long>(m_inputRect.top + (inputRectHeight * WaveMarginTop) - WaveHeight + (inputRectHeight * (m_skewY - SkewOffset)));
            pOutputRect->bottom = static_cast<long>(m_inputRect.top + (inputRectHeight * WaveMarginTop) + WaveHeight);
        }

        if (m_skewX > 0)
        {
            pOutputRect->left  = static_cast<long>(m_inputRect.left   + (inputRectWidth * WaveMarginLeft));
            pOutputRect->right = static_cast<long>(m_inputRect.right  - (inputRectWidth * WaveMarginRight) + (inputRectWidth * m_skewX));
        }
        else
        {
            pOutputRect->left  = static_cast<long>(m_inputRect.left  + (inputRectWidth * WaveMarginLeft) + (inputRectWidth * m_skewX));
            pOutputRect->right = static_cast<long>(m_inputRect.right - (inputRectWidth * WaveMarginRight));
        }

        // Indicate that entire output might contain transparency
        ZeroMemory(pOutputOpaqueSubRect, sizeof(*pOutputOpaqueSubRect));
    }

    return hr;
}

IFACEMETHODIMP WaveEffect::MapInvalidRect(
    UINT32 inputIndex,
    D2D1_RECT_L invalidInputRect,
    _Out_ D2D1_RECT_L* pInvalidOutputRect
    ) const
{
    // Indicate that the entire output may be invalid.
    *pInvalidOutputRect = m_inputRect;

    return S_OK;
}

IFACEMETHODIMP WaveEffect::SetDrawInfo(
    _In_ ID2D1DrawInfo* pDrawInfo
    )
{
    m_drawInfo = pDrawInfo;

    D2D1_VERTEX_RANGE range;
    range.startVertex = 0;
    range.vertexCount = m_numVertices;

    return m_drawInfo->SetVertexProcessing(
        m_vertexBuffer.Get(),
        D2D1_VERTEX_OPTIONS_USE_DEPTH_BUFFER,
        nullptr, // Setting the blend description to nullptr draws the geometry using a simple source-over blend.
        &range,
        &GUID_WaveTransformVertexShader
        );
}

// D2D ensures that that effects are only referenced from one thread at a time.
// To improve performance, we simply increment/decrement our reference count
// rather than use atomic InterlockedIncrement()/InterlockedDecrement() functions.
IFACEMETHODIMP_(ULONG) WaveEffect::AddRef()
{
    m_refCount++;
    return m_refCount;
}

IFACEMETHODIMP_(ULONG) WaveEffect::Release()
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
// of the Wave interface, this method simply enables casting a Wave
// instance to an ID2D1EffectImpl or IUnknown instance.
IFACEMETHODIMP WaveEffect::QueryInterface(
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
