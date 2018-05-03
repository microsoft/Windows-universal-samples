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
#include "TextRenderer.h"
#include "Common\DirectXHelper.h"

using namespace CameraStreamCoordinateMapper;
using namespace Microsoft::WRL;

// Initializes D2D resources used for text rendering.
TextRenderer::TextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) : 
    m_deviceResources(deviceResources)
{
    // Create device independent resources
    DX::ThrowIfFailed(m_deviceResources->GetDWriteFactory()->CreateTextFormat(
        L"Courier New",
        nullptr,
        DWRITE_FONT_WEIGHT_LIGHT,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        24.0f,
        L"en-US",
        &m_textFormat));

    DX::ThrowIfFailed(m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

    DX::ThrowIfFailed(m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock));

    CreateDeviceDependentResources();
}

// Updates the text to be displayed.
void TextRenderer::Update(std::wstring_view text)
{
    DX::ThrowIfFailed(m_deviceResources->GetDWriteFactory()->CreateTextLayout(
        text.data(),
        static_cast<UINT32>(text.length()),
        m_textFormat.Get(),
        1024.0f, // Max width of the input text.
        256.0f, // Max height of the input text.
        &m_textLayout));

    DX::ThrowIfFailed(m_textLayout->GetMetrics(&m_textMetrics));
}

D2D1::Matrix3x2F GetScreenTranslation(const Windows::Foundation::Size& screenSize, const DWRITE_TEXT_METRICS& textMetrics, TextRenderer::TextAlignment alignment)
{
    const float layoutOffsetX = screenSize.Width - textMetrics.layoutWidth;
    const float layoutOffsetY = screenSize.Height - textMetrics.height;

    if (alignment == TextRenderer::TextAlignment::BottomRight)
    {
        return D2D1::Matrix3x2F::Translation(layoutOffsetX, layoutOffsetY);
    }

    if (alignment == TextRenderer::TextAlignment::TopRight)
    {
        return D2D1::Matrix3x2F::Translation(layoutOffsetX, 0.0f);
    }

    if (alignment == TextRenderer::TextAlignment::BottomLeft)
    {
        return D2D1::Matrix3x2F::Translation(0.0f, layoutOffsetY);
    }

    return D2D1::Matrix3x2F::Translation(0.0f, 0.0f);
}

DWRITE_TEXT_ALIGNMENT GetDwriteTextAlignment(TextRenderer::TextAlignment alignment)
{
    if (alignment == TextRenderer::TextAlignment::TopLeft || alignment == TextRenderer::TextAlignment::BottomLeft)
    {
        return DWRITE_TEXT_ALIGNMENT_LEADING;
    }
    else
    {
        return DWRITE_TEXT_ALIGNMENT_TRAILING;
    }
}

// Renders a frame to the screen.
void TextRenderer::Render(TextAlignment alignment)
{
    ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
    const Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

    context->SaveDrawingState(m_stateBlock.Get());
    context->BeginDraw();

    // Position on the bottom right corner
    const D2D1::Matrix3x2F screenTranslation = GetScreenTranslation(logicalSize, m_textMetrics, alignment);

    context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());

    DX::ThrowIfFailed(m_textFormat->SetTextAlignment(GetDwriteTextAlignment(alignment)));

    context->DrawTextLayout(
        D2D1::Point2F(0.f, 0.f),
        m_textLayout.Get(),
        m_whiteBrush.Get());

    // Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    const HRESULT hr = context->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    context->RestoreDrawingState(m_stateBlock.Get());
}

void TextRenderer::CreateDeviceDependentResources()
{
    DX::ThrowIfFailed(m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White), 
        &m_whiteBrush));
}
void TextRenderer::ReleaseDeviceDependentResources()
{
    m_whiteBrush = nullptr;
}