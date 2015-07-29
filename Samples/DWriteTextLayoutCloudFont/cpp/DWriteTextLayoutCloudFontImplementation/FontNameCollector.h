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

#pragma once


namespace DWriteTextLayoutCloudFontImplementation
{
    // An implementation of IDWriteTextRender is included here solely to allow
    // us to get information back from the text layout about the fonts that
    // have been used. It is not required for using downloadable fonts, and
    // most of the methods have trivial implementations because they will 
    // not be used.
    class FontNameCollector : public IDWriteTextRenderer
    {
    public:
        // Method to provide font names from a text layout
        std::wstring GetFontNames();

        void ClearNames();

        // IDWriteTextRenderer implementation
        HRESULT STDMETHODCALLTYPE DrawGlyphRun(
            _In_opt_ void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            DWRITE_MEASURING_MODE measuringMode,
            _In_ DWRITE_GLYPH_RUN const* glyphRun,
            _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
            _In_opt_ IUnknown* clientDrawingEffect
            ) override;

        HRESULT STDMETHODCALLTYPE DrawInlineObject(
            _In_opt_ void* clientDrawingContext,
            FLOAT originX,
            FLOAT originY,
            _In_ IDWriteInlineObject* inlineObject,
            BOOL isSideways,
            BOOL isRightToLeft,
            _In_opt_ IUnknown* clientDrawingEffect
            ) override
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DrawStrikethrough(
            _In_opt_ void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            _In_ DWRITE_STRIKETHROUGH const* strikethrough,
            _In_opt_ IUnknown* clientDrawingEffect
            ) override
        {
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE DrawUnderline(
            _In_opt_ void* clientDrawingContext,
            FLOAT baselineOriginX,
            FLOAT baselineOriginY,
            _In_ DWRITE_UNDERLINE const* underline,
            _In_opt_ IUnknown* clientDrawingEffect
            ) override
        {
            return S_OK;
        }

        // IDWriteTextRenderer inherits from IDWritePixelSnapping:

        HRESULT STDMETHODCALLTYPE GetCurrentTransform(
            _In_opt_ void* clientDrawingContext,
            _Out_ DWRITE_MATRIX* transform
            ) override;

        HRESULT STDMETHODCALLTYPE GetPixelsPerDip(
            _In_opt_ void* clientDrawingContext,
            _Out_ FLOAT* pixelsPerDip
            ) override;

        HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(
            _In_opt_ void* clientDrawingContext,
            _Out_ BOOL* isDisabled
            ) override;

        // IDWritePixelSnapping inherits from IUnknown:

        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid,
            _COM_Outptr_ void **obj
            ) override;

        ULONG STDMETHODCALLTYPE AddRef(void) override;

        ULONG STDMETHODCALLTYPE Release(void) override;

    private:
        uint32                  m_refCount = 0;
        std::set<std::wstring>  m_uniqueFontNames;
    };
}
