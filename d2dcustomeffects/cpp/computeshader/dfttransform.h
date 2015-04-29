//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// Define unique GUIDs for the shaders.
DEFINE_GUID(GUID_DFT_H_CS4, 0x6716AAA9, 0x06A0, 0x460F, 0xAA, 0x9C, 0x8B, 0x8F, 0xF0, 0x33, 0x5E, 0xA6);
DEFINE_GUID(GUID_DFT_H_CS5, 0x652562A9, 0x0523, 0x460F, 0xDE, 0x1C, 0x8B, 0x8F, 0xA0, 0x43, 0x1E, 0x86);
DEFINE_GUID(GUID_DFT_V_CS4, 0x6716BCD1, 0x04D0, 0x3750, 0xAF, 0x9C, 0x7B, 0x23, 0xC0, 0x63, 0x5C, 0x61);
DEFINE_GUID(GUID_DFT_V_CS5, 0x895136D1, 0x1511, 0x3850, 0xAF, 0x3B, 0x7A, 0xA3, 0xB0, 0x6C, 0x5D, 0xE1);

enum class TransformType
{
    Horizontal,
    Vertical
};

class DftTransform : public ID2D1ComputeTransform
{
public:
    DftTransform(_In_ ID2D1EffectContext* context, TransformType dftType);

    // ID2D1ComputeTransform Methods:
    IFACEMETHODIMP_(UINT32) GetInputCount() const { return 1; }
    IFACEMETHODIMP SetComputeInfo(_In_ ID2D1ComputeInfo* pComputeInfo);
    IFACEMETHODIMP CalculateThreadgroups(
        _In_ const D2D1_RECT_L* pOutputRect,
        _Out_ UINT32* pDimensionX,
        _Out_ UINT32* pDimensionY,
        _Out_ UINT32* pDimensionZ
        );

    // ID2D1Transform Methods:
    IFACEMETHODIMP MapOutputRectToInputRects(
        _In_ const D2D1_RECT_L* pOutputRect,
        _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
        UINT32 inputRectCount
        ) const;

    IFACEMETHODIMP MapInputRectsToOutputRect(
        _In_reads_(inputRectCount) const D2D1_RECT_L* pInputRects,
        _In_reads_(inputRectCount) const D2D1_RECT_L* pInputOpaqueSubRects,
        UINT32 inputRectCount,
        _Out_ D2D1_RECT_L* pOutputRect,
        _Out_ D2D1_RECT_L* pOutputOpaqueSubRect
        );

    IFACEMETHODIMP MapInvalidRect(
        UINT32 inputIndex,
        D2D1_RECT_L invalidInputRect,
        _Out_ D2D1_RECT_L* pInvalidOutputRect
        ) const;

    // IUnknown Methods:
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
    IFACEMETHODIMP QueryInterface(REFIID riid, _Outptr_ void** ppOutput);

private:
    LONG m_cRef;
    D2D1_RECT_L m_inputRect;
    D3D_FEATURE_LEVEL m_maxLevelSupported;
    TransformType m_dftType;

    // Used to update the constant buffer for the vertex shader.
    struct
    {
        float magnitudeScale;
    } m_constantBuffer;
};