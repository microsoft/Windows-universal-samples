//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include "DeviceResources.h"

namespace DWriteColorTextRenderer
{
    //  The IDWriteTextRenderer interface is an input parameter to
    //  IDWriteTextLayout::Draw.  This interfaces defines a number of
    //  callback functions that the client application implements for
    //  custom text rendering.
    class CustomTextRenderer : public IDWriteTextRenderer
    {
    public:
        CustomTextRenderer(
            Microsoft::WRL::ComPtr<ID2D1Factory> d2dFactory,
            Microsoft::WRL::ComPtr<ID2D1DeviceContext4> d2dDeviceContext
            );

        IFACEMETHOD(IsPixelSnappingDisabled)(
            _In_opt_ void* clientDrawingContext,
            _Out_ BOOL* isDisabled
            );

        IFACEMETHOD(GetCurrentTransform)(
            _In_opt_ void* clientDrawingContext,
            _Out_ DWRITE_MATRIX* transform
            );

        IFACEMETHOD(GetPixelsPerDip)(
            _In_opt_ void* clientDrawingContext,
            _Out_ FLOAT* pixelsPerDip
            );

        IFACEMETHOD(DrawGlyphRun)(
            _In_opt_ void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            _In_ DWRITE_GLYPH_RUN const* glyphRun,
            _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            IUnknown* clientDrawingEffect
            );

        IFACEMETHOD(DrawUnderline)(
            _In_opt_ void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            _In_ DWRITE_UNDERLINE const* underline,
            IUnknown* clientDrawingEffect
            );

        IFACEMETHOD(DrawStrikethrough)(
            _In_opt_ void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            _In_ DWRITE_STRIKETHROUGH const* strikethrough,
            IUnknown* clientDrawingEffect
            );

        IFACEMETHOD(DrawInlineObject)(
            _In_opt_ void* clientDrawingContext,
            FLOAT originX,
            FLOAT originY,
            IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            IUnknown* clientDrawingEffect
            );

    public:
        IFACEMETHOD_(unsigned long, AddRef) ();
        IFACEMETHOD_(unsigned long, Release) ();
        IFACEMETHOD(QueryInterface) (
            IID const& riid,
            void** ppvObject
            );

    private:
        unsigned long                                m_refCount;
        Microsoft::WRL::ComPtr<ID2D1Factory>         m_d2dFactory;
        Microsoft::WRL::ComPtr<IDWriteFactory4>      m_dwriteFactory;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext4>  m_d2dDeviceContext;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_outlineBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_tempBrush;
    };
};