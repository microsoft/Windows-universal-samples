//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "DWriteColorTextRendererRenderer.h"

#include "DirectXHelper.h"

using namespace DWriteColorTextRenderer;

using namespace DirectX;
using namespace Windows::Foundation;

// Creates a DWriteTextFormat with our text properties. This includes setting our flow and reading direction.
DWriteColorTextRendererRenderer::DWriteColorTextRendererRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Create a DirectWrite text format object.
    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextFormat(
            L"Segoe UI Emoji",
            nullptr,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            32.0f,
            L"en-US", // locale
            &m_dwriteTextFormat
            )
        );

    // Center the text horizontally.
    DX::ThrowIfFailed(
        m_dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
        );

    // Center the text vertically.
    DX::ThrowIfFailed(
        m_dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)
        );

    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

// Initialization.
void DWriteColorTextRendererRenderer::CreateDeviceDependentResources()
{
    m_textRenderer = new (std::nothrow) CustomTextRenderer(
        m_deviceResources->GetD2DFactory(),
        m_deviceResources->GetD2DDeviceContext()
        );
}

// Initialization.
void DWriteColorTextRendererRenderer::CreateWindowSizeDependentResources()
{
    Platform::String^ text = L"🌂🌃🌄🌅🌆🌇🌈🌉🌊🌋🌍🌎🌏🌒🌓🌔🌕🌖🌗🌘🌝🌞🌱🌲🌳🌴🌷🌹🌻🌽🌾🍅🍆🍇🍈🍉🍌🍍🍎🍏🍐🍑🍒🍓🍔🍕🍖🍗🍘🍛🍜🍝🍟🍠🍡🍢🍣🍤🍥🍦🍧🍨🍩🍪🍫🍬🍭🍮🍯🍰🍱🍲🍳🍵🍹🍺🍻🍼🎂🎃🎄🎅🎆🎇🎉🎊🎋🎌🎍🎎🎐🎑🎓🎠🎡🎢🎣🎤🎦🎨🎯🎰🎱🎳🎴🎻🎼🎾🎿🏀🏁🏂🏄🏇🏊🏡🏤🏦🏧🏩🏫🏬🐌🐓🐝🐠🐡🐢🐣🐳🐵🐶🐸🐹👆👇👈👉👊👒👔👛👝👦👧👨👩👮👯👰👱👲👳👴👵👶👷👸👹👺👼👾💂💄💅💆💇💈💉💊💋💌💐💑💒💘💝💟💨💩💱💹💺💾📈📉📊📌📍📑📓📔📛📝📟📣📵🔞🔫😁😂😃😄😅😆😇😈😉😊😋😌😍😎😏😐😒😓😔😖😘😚😜😝😞😠😡😢😣😤😥😨😩😪😫😭😰😱😲😳😵😶😷🙅🙆🙇🙈🙉🙊🙋🙌🙍🙎🙏🚀🚃🚄🚅🚆🚈🚉🚊🚋🚌🚍🚎🚏🚐";
    auto size = m_deviceResources->GetLogicalSize();

    DX::ThrowIfFailed(
        m_deviceResources->GetDWriteFactory()->CreateTextLayout(
            text->Data(),                              // Text to be displayed
            text->Length(),                            // Length of the text
            m_dwriteTextFormat.Get(),                  // DirectWrite Text Format object
            size.Width,                                // Width of the Text Layout
            size.Height,                               // Height of the Text Layout
            &m_dwriteTextLayout
            )
        );
}

// Release all references to resources that depend on the graphics device.
// This method is invoked when the device is lost and resources are no longer usable.
void DWriteColorTextRendererRenderer::ReleaseDeviceDependentResources()
{
    m_textRenderer.Reset();
}

// Renders one frame.
void DWriteColorTextRendererRenderer::Render()
{
    m_deviceResources->GetD2DDeviceContext()->BeginDraw();

    m_deviceResources->GetD2DDeviceContext()->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

    m_deviceResources->GetD2DDeviceContext()->SetTransform(
        m_deviceResources->GetOrientationTransform2D()
        );

    D2D1_POINT_2F origin = D2D1::Point2F(0.0f, 0.0f);

    DX::ThrowIfFailed(
        m_dwriteTextLayout->Draw(
            m_deviceResources->GetD2DDeviceContext(),
            m_textRenderer.Get(),
            origin.x,
            origin.y
            )
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_deviceResources->GetD2DDeviceContext()->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }
}
