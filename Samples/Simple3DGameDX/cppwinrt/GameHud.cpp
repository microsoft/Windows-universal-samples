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
#include "GameHud.h"
#include "GameConstants.h"
#include "GameUIConstants.h"

using namespace D2D1;
using namespace winrt::Windows::ApplicationModel;

GameHud::GameHud(
    _In_ std::shared_ptr<DX::DeviceResources> const& deviceResources,
    _In_ winrt::hstring const& titleHeader,
    _In_ winrt::hstring const& titleBody
    ) :
    m_deviceResources(deviceResources),
    m_titleHeader(titleHeader),
    m_titleBody(titleBody)
{
    m_showTitle = true;
    m_titleBodyVerticalOffset = GameUIConstants::Margin;
    m_logoSize = D2D1::SizeF(0.0f, 0.0f);

    auto dwriteFactory = m_deviceResources->GetDWriteFactory();

    winrt::check_hresult(
        dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            GameUIConstants::HudBodyPointSize,
            L"en-us",
            m_textFormatBody.put()
            )
        );
    winrt::check_hresult(
        dwriteFactory->CreateTextFormat(
            L"Segoe UI Symbol",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            GameUIConstants::HudBodyPointSize,
            L"en-us",
            m_textFormatBodySymbol.put()
            )
        );
    winrt::check_hresult(
        dwriteFactory->CreateTextFormat(
            L"Segoe UI Light",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            GameUIConstants::HudTitleHeaderPointSize,
            L"en-us",
            m_textFormatTitleHeader.put()
            )
        );
    winrt::check_hresult(
        dwriteFactory->CreateTextFormat(
            L"Segoe UI Light",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            GameUIConstants::HudTitleBodyPointSize,
            L"en-us",
            m_textFormatTitleBody.put()
            )
        );

    winrt::check_hresult(m_textFormatBody->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
    winrt::check_hresult(m_textFormatBody->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
    winrt::check_hresult(m_textFormatBodySymbol->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
    winrt::check_hresult(m_textFormatBodySymbol->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
    winrt::check_hresult(m_textFormatTitleHeader->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
    winrt::check_hresult(m_textFormatTitleHeader->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
    winrt::check_hresult(m_textFormatTitleBody->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
    winrt::check_hresult(m_textFormatTitleBody->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
}

void GameHud::CreateDeviceDependentResources()
{
    auto location = Package::Current().InstalledLocation();
    winrt::hstring path{ location.Path() + L"\\Assets\\windows-sdk.png" };

    auto wicFactory = m_deviceResources->GetWicImagingFactory();

    winrt::com_ptr<IWICBitmapDecoder> wicBitmapDecoder;
    winrt::check_hresult(
        wicFactory->CreateDecoderFromFilename(
            path.c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            wicBitmapDecoder.put()
            )
        );

    winrt::com_ptr<IWICBitmapFrameDecode> wicBitmapFrame;
    winrt::check_hresult(
        wicBitmapDecoder->GetFrame(0, wicBitmapFrame.put())
        );

    winrt::com_ptr<IWICFormatConverter> wicFormatConverter;
    winrt::check_hresult(
        wicFactory->CreateFormatConverter(wicFormatConverter.put())
        );

    winrt::check_hresult(
        wicFormatConverter->Initialize(
            wicBitmapFrame.get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom  // The BGRA format has no palette so this value is ignored.
            )
        );

    double dpiX = 96.0f;
    double dpiY = 96.0f;
    winrt::check_hresult(
        wicFormatConverter->GetResolution(&dpiX, &dpiY)
        );

    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    // Create D2D Resources
    winrt::check_hresult(
        d2dContext->CreateBitmapFromWicBitmap(
            wicFormatConverter.get(),
            BitmapProperties(
                PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                static_cast<float>(dpiX),
                static_cast<float>(dpiY)
                ),
            m_logoBitmap.put()
            )
        );

    m_logoSize = m_logoBitmap->GetSize();

    winrt::check_hresult(
        d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            m_textBrush.put()
            )
        );
}

void GameHud::CreateWindowSizeDependentResources()
{
    auto windowBounds = m_deviceResources->GetLogicalSize();

    m_maxTitleSize.width = windowBounds.Width - GameUIConstants::HudSafeWidth;
    m_maxTitleSize.height = windowBounds.Height;

    float headerWidth = m_maxTitleSize.width - (m_logoSize.width + 2 * GameUIConstants::Margin);

    if (headerWidth > 0)
    {
        auto dwriteFactory = m_deviceResources->GetDWriteFactory();

        // Only resize the text layout for the Title area when there is enough space.
        m_showTitle = true;

        m_titleHeaderLayout = nullptr;
        winrt::check_hresult(
            dwriteFactory->CreateTextLayout(
                m_titleHeader.c_str(),
                m_titleHeader.size(),
                m_textFormatTitleHeader.get(),
                headerWidth,
                m_maxTitleSize.height,
                m_titleHeaderLayout.put()
                )
            );

        DWRITE_TEXT_METRICS metrics = { 0 };
        winrt::check_hresult(
            m_titleHeaderLayout->GetMetrics(&metrics)
            );

        // Compute the vertical size of the laid out Header and logo. This could change
        // based on the window size and the layout of the text. In some cases the text
        // may wrap.
        m_titleBodyVerticalOffset = max(m_logoSize.height + GameUIConstants::Margin * 2, metrics.height + 2 * GameUIConstants::Margin);

        m_titleBodyLayout = nullptr;
        winrt::check_hresult(
            dwriteFactory->CreateTextLayout(
                m_titleBody.c_str(),
                m_titleBody.size(),
                m_textFormatTitleBody.get(),
                m_maxTitleSize.width,
                m_maxTitleSize.height - m_titleBodyVerticalOffset,
                m_titleBodyLayout.put()
                )
            );
    }
    else
    {
        // Not enough horizontal space for the titles so just turn it off.
        m_showTitle = false;
    }
}

void GameHud::Render(_In_ std::shared_ptr<Simple3DGame> const& game)
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    auto windowBounds = m_deviceResources->GetLogicalSize();

    if (m_showTitle)
    {
        d2dContext->DrawBitmap(
            m_logoBitmap.get(),
            D2D1::RectF(
                GameUIConstants::Margin,
                GameUIConstants::Margin,
                m_logoSize.width + GameUIConstants::Margin,
                m_logoSize.height + GameUIConstants::Margin
                )
            );
        d2dContext->DrawTextLayout(
            Point2F(m_logoSize.width + 2.0f * GameUIConstants::Margin, GameUIConstants::Margin),
            m_titleHeaderLayout.get(),
            m_textBrush.get()
            );
        d2dContext->DrawTextLayout(
            Point2F(GameUIConstants::Margin, m_titleBodyVerticalOffset),
            m_titleBodyLayout.get(),
            m_textBrush.get()
            );
    }

    if (game != nullptr)
    {
        // This section is only used after the game state has been initialized.
        static const int bufferLength = 256;
        static wchar_t wsbuffer[bufferLength];
        int length = swprintf_s(
            wsbuffer,
            bufferLength,
            L"Hits:\t%10d\nShots:\t%10d\nTime:\t%8.1f",
            game->TotalHits(),
            game->TotalShots(),
            game->TimeRemaining()
            );

        d2dContext->DrawText(
            wsbuffer,
            length,
            m_textFormatBody.get(),
            D2D1::RectF(
                windowBounds.Width - GameUIConstants::HudRightOffset,
                GameUIConstants::HudTopOffset,
                windowBounds.Width,
                GameUIConstants::HudTopOffset + (GameUIConstants::HudBodyPointSize + GameUIConstants::Margin) * 3
                ),
            m_textBrush.get()
            );

        // Using the unicode characters starting at 0x2780 ( ➀ ) for the consecutive levels of the game.
        // For completed levels start with 0x278A ( ➊ ) (This is 0x2780 + 10).
        uint32_t levelCharacter[6];
        for (uint32_t i = 0; i < 6; i++)
        {
            levelCharacter[i] = 0x2780 + i + ((static_cast<uint32_t>(game->LevelCompleted()) == i) ? 10 : 0);
        }
        length = swprintf_s(
            wsbuffer,
            bufferLength,
            L"%lc %lc %lc %lc %lc %lc",
            levelCharacter[0],
            levelCharacter[1],
            levelCharacter[2],
            levelCharacter[3],
            levelCharacter[4],
            levelCharacter[5]
            );
        d2dContext->DrawText(
            wsbuffer,
            length,
            m_textFormatBodySymbol.get(),
            D2D1::RectF(
                windowBounds.Width - GameUIConstants::HudRightOffset,
                GameUIConstants::HudTopOffset + (GameUIConstants::HudBodyPointSize + GameUIConstants::Margin) * 3 + GameUIConstants::Margin,
                windowBounds.Width,
                GameUIConstants::HudTopOffset + (GameUIConstants::HudBodyPointSize + GameUIConstants::Margin) * 4
                ),
            m_textBrush.get()
            );

        if (game->IsActivePlay())
        {
            // Draw a rectangle for the touch input for the move control.
            d2dContext->DrawRectangle(
                D2D1::RectF(
                    0.0f,
                    windowBounds.Height - GameUIConstants::TouchRectangleSize,
                    GameUIConstants::TouchRectangleSize,
                    windowBounds.Height
                    ),
                m_textBrush.get()
                );
            // Draw a rectangle for the touch input for the fire control.
            d2dContext->DrawRectangle(
                D2D1::RectF(
                    windowBounds.Width - GameUIConstants::TouchRectangleSize,
                    windowBounds.Height - GameUIConstants::TouchRectangleSize,
                    windowBounds.Width,
                    windowBounds.Height
                    ),
                m_textBrush.get()
                );

            // Draw the cross hairs
            d2dContext->DrawLine(
                D2D1::Point2F(windowBounds.Width / 2.0f - GameUIConstants::CrossHairHalfSize, windowBounds.Height / 2.0f),
                D2D1::Point2F(windowBounds.Width / 2.0f + GameUIConstants::CrossHairHalfSize, windowBounds.Height / 2.0f),
                m_textBrush.get(),
                3.0f
                );
            d2dContext->DrawLine(
                D2D1::Point2F(windowBounds.Width / 2.0f, windowBounds.Height / 2.0f - GameUIConstants::CrossHairHalfSize),
                D2D1::Point2F(windowBounds.Width / 2.0f, windowBounds.Height / 2.0f + GameUIConstants::CrossHairHalfSize),
                m_textBrush.get(),
                3.0f
                );
        }
    }
}

void GameHud::ReleaseDeviceDependentResources()
{
    m_textBrush = nullptr;
    m_logoBitmap = nullptr;
}