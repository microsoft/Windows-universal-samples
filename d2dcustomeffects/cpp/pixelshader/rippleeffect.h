//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

DEFINE_GUID(GUID_RipplePixelShader, 0x6716FB29, 0x06A0, 0x460F, 0xAA, 0x9C, 0x8B, 0x8F, 0xF0, 0x33, 0x5E, 0xA6);
DEFINE_GUID(CLSID_CustomRippleEffect, 0xB7B36C92, 0x3498, 0x4A94, 0x9E, 0x95, 0x9F, 0x24, 0x6F, 0x92, 0x45, 0xBF);

enum RIPPLE_PROP
{
    RIPPLE_PROP_FREQUENCY = 0,
    RIPPLE_PROP_PHASE     = 1,
    RIPPLE_PROP_AMPLITUDE = 2,
    RIPPLE_PROP_SPREAD    = 3,
    RIPPLE_PROP_CENTER    = 4
};

// Our effect contains one transform, which is simply a wrapper around a pixel shader. As such,
// we can simply make the effect itself act as the transform.
class RippleEffect : public ID2D1EffectImpl, public ID2D1DrawTransform
{
public:
    // Declare effect registration methods.
    static HRESULT Register(_In_ ID2D1Factory1* pFactory);

    static HRESULT __stdcall CreateRippleImpl(_Outptr_ IUnknown** ppEffectImpl);

    // Declare property getter/setter methods.
    HRESULT SetCenter(D2D1_POINT_2F center);
    D2D1_POINT_2F GetCenter() const;

    HRESULT SetFrequency(float frequency);
    float GetFrequency() const;

    HRESULT SetPhase(float phase);
    float GetPhase() const;

    HRESULT SetAmplitude(float amplitude);
    float GetAmplitude() const;

    HRESULT SetSpread(float spread);
    float GetSpread() const;

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
    RippleEffect();
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
        float frequency;
        float phase;
        float amplitude;
        float spread;
        D2D1_POINT_2F center;
        float dpi;
    } m_constants;

    Microsoft::WRL::ComPtr<ID2D1DrawInfo>      m_drawInfo;
    Microsoft::WRL::ComPtr<ID2D1EffectContext> m_effectContext;
    LONG                                       m_refCount;
    D2D1_RECT_L                                m_inputRect;
    float                                      m_dpi;
};
