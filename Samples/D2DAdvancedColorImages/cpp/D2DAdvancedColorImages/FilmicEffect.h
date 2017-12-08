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

DEFINE_GUID(GUID_FilmicEffectPixelShader, 0x4214db42, 0x3bc5, 0x4f17, 0xb8, 0x59, 0x9d, 0x29, 0x15, 0x27, 0x4a, 0x6c);
DEFINE_GUID(CLSID_CustomFilmicEffect, 0x40e0e874, 0x562c, 0x480f, 0x9b, 0x3e, 0x69, 0xf8, 0x42, 0x25, 0x59, 0xc9);

// Our effect contains one transform, which is simply a wrapper around a pixel shader. As such,
// we can simply make the effect itself act as the transform.
class FilmicEffect : public ID2D1EffectImpl, public ID2D1DrawTransform
{
public:
    // Declare effect registration methods.
    static HRESULT Register(_In_ ID2D1Factory1* pFactory);

    static HRESULT __stdcall CreateFilmicImpl(_Outptr_ IUnknown** ppEffectImpl);

    // Declare ID2D1EffectImpl implementation methods.
    IFACEMETHODIMP Initialize(
        _In_ ID2D1EffectContext* pContextInternal,
        _In_ ID2D1TransformGraph* pTransformGraph
        );

    IFACEMETHODIMP PrepareForRender(D2D1_CHANGE_TYPE changeType);

    IFACEMETHODIMP SetGraph(_In_ ID2D1TransformGraph* pGraph);

    // Declare ID2D1DrawTransform implementation methods.
    IFACEMETHODIMP SetDrawInfo(_In_ ID2D1DrawInfo* pRenderInfo);

    // Declare ID2D1Transform implementation methods.
    IFACEMETHODIMP MapOutputRectToInputRects(
        _In_ const D2D1_RECT_L* pOutputRect,
        _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
        UINT32 inputRectCount
        ) const;

    IFACEMETHODIMP MapInputRectsToOutputRect(
        _In_reads_(inputRectCount) CONST D2D1_RECT_L* pInputRects,
        _In_reads_(inputRectCount) CONST D2D1_RECT_L* pInputOpaqueSubRects,
        UINT32 inputRectCount,
        _Out_ D2D1_RECT_L* pOutputRect,
        _Out_ D2D1_RECT_L* pOutputOpaqueSubRect
        );

    IFACEMETHODIMP MapInvalidRect(
        UINT32 inputIndex,
        D2D1_RECT_L invalidInputRect,
        _Out_ D2D1_RECT_L* pInvalidOutputRect
        ) const;

    // Declare ID2D1TransformNode implementation methods.
    IFACEMETHODIMP_(UINT32) GetInputCount() const;

    // Declare IUnknown implementation methods.
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
    IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput);

private:
    FilmicEffect();
    HRESULT UpdateConstants();

    inline static float Clamp(float v, float low, float high)
    {
        return (v < low) ? low : (v > high) ? high : v;
    }

    inline static float Round(float v)
    {
        return floor(v + 0.5f);
    }

    // Prevents over/underflows when adding longs.
    inline static long SafeAdd(long base, long valueToAdd)
    {
        if (valueToAdd >= 0)
        {
            return ((base + valueToAdd) >= base) ? (base + valueToAdd) : LONG_MAX;
        }
        else
        {
            return ((base + valueToAdd) <= base) ? (base + valueToAdd) : LONG_MIN;
        }
    }

    // This struct defines the constant buffer of our pixel shader.
    struct
    {
        float dpi;
    } m_constants;

    Microsoft::WRL::ComPtr<ID2D1DrawInfo>      m_drawInfo;
    Microsoft::WRL::ComPtr<ID2D1EffectContext> m_effectContext;
    LONG                                       m_refCount;
    D2D1_RECT_L                                m_inputRect;
    float                                      m_dpi;
};
