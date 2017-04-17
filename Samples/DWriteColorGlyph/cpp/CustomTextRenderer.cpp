//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "CustomTextRenderer.h"
#include "DirectXHelper.h"

using namespace DWriteColorTextRenderer;

using namespace Microsoft::WRL;

// The constructor stores the Direct2D factory and device context
// and creates resources the renderer will use.
CustomTextRenderer::CustomTextRenderer(
    ComPtr<ID2D1Factory> d2dFactory,
    ComPtr<ID2D1DeviceContext4> d2dDeviceContext
    ) :
    m_refCount(0),
    m_d2dFactory(d2dFactory),
    m_d2dDeviceContext(d2dDeviceContext)
{
    DX::ThrowIfFailed(
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory4),
            &m_dwriteFactory
            )
        );

    DX::ThrowIfFailed(
        m_d2dDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Crimson),
            &m_outlineBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            &m_tempBrush
            )
        );
}

// Decomposes the received glyph run into smaller color glyph runs
// using IDWriteFactory4::TranslateColorGlyphRun. Depending on the
// type of each color run, the renderer uses Direct2D to draw the
// outlines, SVG content, or bitmap content.
HRESULT CustomTextRenderer::DrawGlyphRun(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    DWRITE_MEASURING_MODE measuringMode,
    _In_ DWRITE_GLYPH_RUN const* glyphRun,
    _In_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
    IUnknown* clientDrawingEffect
)
{
    HRESULT hr = DWRITE_E_NOCOLOR;

    D2D1_POINT_2F baselineOrigin = D2D1::Point2F(baselineOriginX, baselineOriginY);

    // The list of glyph image formats this renderer is prepared to support.
    DWRITE_GLYPH_IMAGE_FORMATS supportedFormats =
        DWRITE_GLYPH_IMAGE_FORMATS_TRUETYPE |
        DWRITE_GLYPH_IMAGE_FORMATS_CFF |
        DWRITE_GLYPH_IMAGE_FORMATS_COLR |
        DWRITE_GLYPH_IMAGE_FORMATS_SVG |
        DWRITE_GLYPH_IMAGE_FORMATS_PNG |
        DWRITE_GLYPH_IMAGE_FORMATS_JPEG |
        DWRITE_GLYPH_IMAGE_FORMATS_TIFF |
        DWRITE_GLYPH_IMAGE_FORMATS_PREMULTIPLIED_B8G8R8A8;

    // Determine whether there are any color glyph runs within glyphRun. If
    // there are, glyphRunEnumerator can be used to iterate through them.
    ComPtr<IDWriteColorGlyphRunEnumerator1> glyphRunEnumerator;
    hr = m_dwriteFactory->TranslateColorGlyphRun(
        baselineOrigin,
        glyphRun,
        glyphRunDescription,
        supportedFormats,
        measuringMode,
        nullptr,
        0,
        &glyphRunEnumerator
        );

    if (hr == DWRITE_E_NOCOLOR)
    {
        // Simple case: the run has no color glyphs. Draw the main glyph run
        // using the current text color.
        m_d2dDeviceContext->DrawGlyphRun(
            baselineOrigin,
            glyphRun,
            glyphRunDescription,
            m_outlineBrush.Get(),
            measuringMode
            );
    }
    else
    {
        DX::ThrowIfFailed(hr);

        // Complex case: the run has one or more color runs within it. Iterate
        // over the sub-runs and draw them, depending on their format.
        for (;;)
        {
            BOOL haveRun;
            DX::ThrowIfFailed(glyphRunEnumerator->MoveNext(&haveRun));
            if (!haveRun)
                break;

            DWRITE_COLOR_GLYPH_RUN1 const* colorRun;
            DX::ThrowIfFailed(glyphRunEnumerator->GetCurrentRun(&colorRun));

            D2D1_POINT_2F currentBaselineOrigin = D2D1::Point2F(
                colorRun->baselineOriginX,
                colorRun->baselineOriginY
                );

            switch (colorRun->glyphImageFormat)
            {
            case DWRITE_GLYPH_IMAGE_FORMATS_PNG:
            case DWRITE_GLYPH_IMAGE_FORMATS_JPEG:
            case DWRITE_GLYPH_IMAGE_FORMATS_TIFF:
            case DWRITE_GLYPH_IMAGE_FORMATS_PREMULTIPLIED_B8G8R8A8:
            {
                // This run is bitmap glyphs. Use Direct2D to draw them.
                m_d2dDeviceContext->DrawColorBitmapGlyphRun(
                    colorRun->glyphImageFormat,
                    currentBaselineOrigin,
                    &colorRun->glyphRun,
                    measuringMode
                    );
            }
            break;

            case DWRITE_GLYPH_IMAGE_FORMATS_SVG:
            {
                // This run is SVG glyphs. Use Direct2D to draw them.
                m_d2dDeviceContext->DrawSvgGlyphRun(
                    currentBaselineOrigin,
                    &colorRun->glyphRun,
                    m_outlineBrush.Get(),
                    nullptr,                // svgGlyphStyle
                    0,                      // colorPaletteIndex
                    measuringMode
                    );
            }
            break;

            case DWRITE_GLYPH_IMAGE_FORMATS_TRUETYPE:
            case DWRITE_GLYPH_IMAGE_FORMATS_CFF:
            case DWRITE_GLYPH_IMAGE_FORMATS_COLR:
            default:
            {
                // This run is solid-color outlines, either from non-color
                // glyphs or from COLR glyph layers. Use Direct2D to draw them.

                ComPtr<ID2D1Brush> layerBrush;
                if (colorRun->paletteIndex == 0xFFFF)
                {
                    // This run uses the current text color.
                    layerBrush = m_outlineBrush;
                }
                else
                {
                    // This run specifies its own color.
                    m_tempBrush->SetColor(colorRun->runColor);
                    layerBrush = m_tempBrush;
                }

                // Draw the run with the selected color.
                m_d2dDeviceContext->DrawGlyphRun(
                    currentBaselineOrigin,
                    &colorRun->glyphRun,
                    colorRun->glyphRunDescription,
                    layerBrush.Get(),
                    measuringMode
                    );
            }
            break;
            }
        }
    }

    return hr;
}

IFACEMETHODIMP CustomTextRenderer::DrawUnderline(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    _In_ DWRITE_UNDERLINE const* underline,
    IUnknown* clientDrawingEffect
    )
{
    // Not implemented
    return E_NOTIMPL;
}

IFACEMETHODIMP CustomTextRenderer::DrawStrikethrough(
    _In_opt_ void* clientDrawingContext,
    FLOAT baselineOriginX,
    FLOAT baselineOriginY,
    _In_ DWRITE_STRIKETHROUGH const* strikethrough,
    IUnknown* clientDrawingEffect
    )
{
    // Not implemented
    return E_NOTIMPL;
}

IFACEMETHODIMP CustomTextRenderer::DrawInlineObject(
    _In_opt_ void* clientDrawingContext,
    FLOAT originX,
    FLOAT originY,
    IDWriteInlineObject* inlineObject,
    BOOL isSideways,
    BOOL isRightToLeft,
    IUnknown* clientDrawingEffect
    )
{
    // Not implemented
    return E_NOTIMPL;
}

IFACEMETHODIMP_(unsigned long) CustomTextRenderer::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(unsigned long) CustomTextRenderer::Release()
{
    unsigned long newCount = InterlockedDecrement(&m_refCount);
    if (newCount == 0)
    {
        delete this;
        return 0;
    }

    return newCount;
}

IFACEMETHODIMP CustomTextRenderer::IsPixelSnappingDisabled(
    _In_opt_ void* clientDrawingContext,
    _Out_ BOOL* isDisabled
    )
{
    *isDisabled = FALSE;
    return S_OK;
}

IFACEMETHODIMP CustomTextRenderer::GetCurrentTransform(
    _In_opt_ void* clientDrawingContext,
    _Out_ DWRITE_MATRIX* transform
    )
{
    // forward the render target's transform
    m_d2dDeviceContext->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
    return S_OK;
}

IFACEMETHODIMP CustomTextRenderer::GetPixelsPerDip(
    _In_opt_ void* clientDrawingContext,
    _Out_ FLOAT* pixelsPerDip
    )
{
    float x, yUnused;

    m_d2dDeviceContext.Get()->GetDpi(&x, &yUnused);
    *pixelsPerDip = x / 96.0f;

    return S_OK;
}

IFACEMETHODIMP CustomTextRenderer::QueryInterface(
    IID const& riid,
    void** ppvObject
    )
{
    if (__uuidof(IDWriteTextRenderer) == riid)
    {
        *ppvObject = this;
    }
    else if (__uuidof(IDWritePixelSnapping) == riid)
    {
        *ppvObject = this;
    }
    else if (__uuidof(IUnknown) == riid)
    {
        *ppvObject = this;
    }
    else
    {
        *ppvObject = nullptr;
        return E_FAIL;
    }

    this->AddRef();

    return S_OK;
}
