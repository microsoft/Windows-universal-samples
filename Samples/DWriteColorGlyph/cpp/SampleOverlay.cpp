//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "SampleOverlay.h"

using namespace DWriteColorTextRenderer;

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Display;
using namespace D2D1;

SampleOverlay::SampleOverlay(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::wstring& caption) :
    m_deviceResources(deviceResources),
    m_caption(caption),
    m_drawOverlay(true)
{
    m_padding = 8.0f;
    m_textVerticalOffset = 7.0f;
    m_logoSize = D2D1::SizeF(0.0f, 0.0f);
    m_overlayWidth = 0.0f;

    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void SampleOverlay::CreateDeviceDependentResources()
{
    DX::ThrowIfFailed(
        m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(ColorF(ColorF::White), &m_whiteBrush)
        );

    ComPtr<IWICBitmapDecoder> wicBitmapDecoder;
    DX::ThrowIfFailed(
        m_deviceResources->GetWicImagingFactory()->CreateDecoderFromFilename(
            L"Assets\\storelogo-sdk.png",
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &wicBitmapDecoder
            )
        );

    ComPtr<IWICBitmapFrameDecode> wicBitmapFrame;
    DX::ThrowIfFailed(
        wicBitmapDecoder->GetFrame(0, &wicBitmapFrame)
        );

    ComPtr<IWICFormatConverter> wicFormatConverter;
    DX::ThrowIfFailed(
        m_deviceResources->GetWicImagingFactory()->CreateFormatConverter(&wicFormatConverter)
        );

    DX::ThrowIfFailed(
        wicFormatConverter->Initialize(
            wicBitmapFrame.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom  // the BGRA format has no palette so this value is ignored
            )
        );

    double dpiX = 96.0;
    double dpiY = 96.0;
    DX::ThrowIfFailed(
        wicFormatConverter->GetResolution(&dpiX, &dpiY)
        );

    DX::ThrowIfFailed(
        m_deviceResources->GetD2DDeviceContext()->CreateBitmapFromWicBitmap(
            wicFormatConverter.Get(),
            BitmapProperties(
                PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                static_cast<float>(dpiX),
                static_cast<float>(dpiY)
                ),
            &m_logoBitmap
            )
        );

    m_logoSize = m_logoBitmap->GetSize();

    ComPtr<IDWriteTextFormat> nameTextFormat;
    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            36.0f,
            L"en-US",
            &nameTextFormat
            )
        );

    DX::ThrowIfFailed(
        nameTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
        );

    DX::ThrowIfFailed(
        nameTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
        );

    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextLayout(
            m_caption.c_str(),
            static_cast<UINT32>(m_caption.size()),
            nameTextFormat.Get(),
            4096.0f,
            4096.0f,
            &m_textLayout
            )
        );

    DWRITE_TEXT_METRICS metrics = {0};
    DX::ThrowIfFailed(
        m_textLayout->GetMetrics(&metrics)
        );

    m_overlayWidth = m_padding * 3.0f + m_logoSize.width + metrics.width;

    DX::ThrowIfFailed(
        m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock)
        );
}

void SampleOverlay::CreateWindowSizeDependentResources()
{
    if (CoreWindow::GetForCurrentThread()->Bounds.Width < m_overlayWidth)
    {
        m_drawOverlay = false;
    }
    else
    {
        m_drawOverlay = true;
    }
}

// Release all references to resources that depend on the graphics device.
// This method is invoked when the device is lost and resources are no longer usable.
void SampleOverlay::ReleaseDeviceDependentResources()
{
    m_whiteBrush.Reset();
    m_logoBitmap.Reset();
}

void SampleOverlay::Render()
{
    if (m_drawOverlay)
    {
        m_deviceResources->GetD2DDeviceContext()->SaveDrawingState(m_stateBlock.Get());

        m_deviceResources->GetD2DDeviceContext()->BeginDraw();
        m_deviceResources->GetD2DDeviceContext()->SetTransform(m_deviceResources->GetOrientationTransform2D());
        m_deviceResources->GetD2DDeviceContext()->DrawBitmap(
            m_logoBitmap.Get(),
            D2D1::RectF(m_padding, m_padding, m_logoSize.width + m_padding, m_logoSize.height + m_padding)
            );

        m_deviceResources->GetD2DDeviceContext()->DrawTextLayout(
            Point2F(m_logoSize.width + 2.0f * m_padding, m_textVerticalOffset),
            m_textLayout.Get(),
            m_whiteBrush.Get()
            );

        // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        HRESULT hr = m_deviceResources->GetD2DDeviceContext()->EndDraw();
        if (hr != D2DERR_RECREATE_TARGET)
        {
            DX::ThrowIfFailed(hr);
        }

        m_deviceResources->GetD2DDeviceContext()->RestoreDrawingState(m_stateBlock.Get());
    }
}

float SampleOverlay::GetTitleHeightInDips()
{
    return m_logoSize.height + m_padding;
}
