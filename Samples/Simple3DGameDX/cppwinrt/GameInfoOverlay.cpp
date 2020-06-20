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
#include "GameUIConstants.h"
#include "DeviceResources.h"
#include "GameInfoOverlay.h"

using namespace Simple3DGameDX;

static const int bufferLength = 1000;
static wchar_t wsbuffer[bufferLength];

GameInfoOverlay::GameInfoOverlay(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_visible(false),
    m_titleString(L"Title"),
    m_bodyString(L"Body"),
    m_actionString(L"Action")
{
    auto dwriteFactory = m_deviceResources->GetDWriteFactory();

    // Create D2D Resources
    winrt::check_hresult(
        dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            GameInfoOverlayConstant::TitlePointSize,
            L"en-us",   // locale
            m_textFormatTitle.put()
            )
        );

    winrt::check_hresult(
        dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            GameInfoOverlayConstant::BodyPointSize,
            L"en-us",   // locale
            m_textFormatBody.put()
            )
        );

    winrt::check_hresult(
        m_textFormatTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)
        );
    winrt::check_hresult(
        m_textFormatTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
        );
    winrt::check_hresult(
        m_textFormatBody->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
        );
    winrt::check_hresult(
        m_textFormatBody->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
        );
}

void GameInfoOverlay::CreateDeviceDependentResources()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    winrt::check_hresult(
        d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            m_textBrush.put()
            )
        );
    winrt::check_hresult(
        d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black),
            m_backgroundBrush.put()
            )
        );
    winrt::check_hresult(
        d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(0xdb7100, 1.0f),
            m_actionBrush.put()
            )
        );
}

void GameInfoOverlay::CreateWindowSizeDependentResources(D2D_SIZE_F overlaySize)
{
    auto dpi = m_deviceResources->GetDpi();
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();
    auto windowSize = m_deviceResources->GetLogicalSize();

    m_levelBitmap = nullptr;
    m_tooSmallBitmap = nullptr;

    m_titleRectangle = D2D1::RectF(
        GameInfoOverlayConstant::SideMargin,
        GameInfoOverlayConstant::TopMargin,
        overlaySize.width - GameInfoOverlayConstant::SideMargin,
        GameInfoOverlayConstant::TopMargin + GameInfoOverlayConstant::TitleHeight
        );
    m_actionRectangle = D2D1::RectF(
        GameInfoOverlayConstant::SideMargin,
        overlaySize.height - (GameInfoOverlayConstant::ActionHeight + GameInfoOverlayConstant::BottomMargin),
        overlaySize.width - GameInfoOverlayConstant::SideMargin,
        overlaySize.height - GameInfoOverlayConstant::BottomMargin
        );
    m_bodyRectangle = D2D1::RectF(
        GameInfoOverlayConstant::SideMargin,
        m_titleRectangle.bottom + GameInfoOverlayConstant::Separator,
        overlaySize.width - GameInfoOverlayConstant::SideMargin,
        m_actionRectangle.top - GameInfoOverlayConstant::Separator
        );

    // Create a D2D Bitmap to be used for Game Info Overlay when waiting to
    // start a level or when display game statistics.
    D2D1_BITMAP_PROPERTIES1 properties;
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    properties.dpiX = dpi;
    properties.dpiY = dpi;
    properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
    properties.colorContext = nullptr;
    winrt::check_hresult(
        d2dContext->CreateBitmap(
            D2D1::SizeU(
                static_cast<UINT32>(overlaySize.width * dpi / 96.0f),
                static_cast<UINT32>(overlaySize.height * dpi / 96.0f)
                ),
            nullptr,
            0,
            &properties,
            m_levelBitmap.put()
            )
        );
    d2dContext->SetTarget(m_levelBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    d2dContext->DrawText(
        m_titleString.c_str(),
        m_titleString.size(),
        m_textFormatTitle.get(),
        m_titleRectangle,
        m_textBrush.get()
        );
    d2dContext->DrawText(
        m_bodyString.c_str(),
        m_bodyString.size(),
        m_textFormatBody.get(),
        m_bodyRectangle,
        m_textBrush.get()
        );
    d2dContext->DrawText(
        m_actionString.c_str(),
        m_actionString.size(),
        m_textFormatBody.get(),
        m_actionRectangle,
        m_actionBrush.get()
        );
    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }

    // Create the TooSmall Bitmap.
    winrt::check_hresult(
        d2dContext->CreateBitmap(
            D2D1::SizeU(
                static_cast<UINT32>(overlaySize.width * dpi / 96.0f),
                static_cast<UINT32>(overlaySize.height * dpi / 96.0f)
                ),
            nullptr,
            0,
            &properties,
            m_tooSmallBitmap.put()
            )
        );
    d2dContext->SetTarget(m_tooSmallBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    d2dContext->FillRectangle(&m_titleRectangle, m_backgroundBrush.get());
    d2dContext->FillRectangle(&m_bodyRectangle, m_backgroundBrush.get());
    std::wstring string = L"Paused";

    d2dContext->DrawText(
        string.c_str(),
        string.size(),
        m_textFormatTitle.get(),
        m_bodyRectangle,
        m_textBrush.get()
        );
    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }
}

ID2D1Bitmap1* GameInfoOverlay::Bitmap()
{
    if (m_tooSmallActive)
    {
        return m_tooSmallBitmap.get();
    }
    else
    {
        return m_levelBitmap.get();
    }
}

void GameInfoOverlay::ReleaseDeviceDependentResources()
{
    m_levelBitmap = nullptr;
    m_tooSmallBitmap = nullptr;
    m_textBrush = nullptr;
    m_backgroundBrush = nullptr;
    m_actionBrush = nullptr;
}

void GameInfoOverlay::SetGameLoading(uint32_t dots)
{
    int length;
    m_titleString = L"Loading Resources";
    m_bodyString = L"";

    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->SetTarget(m_levelBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->FillRectangle(&m_titleRectangle, m_backgroundBrush.get());
    d2dContext->FillRectangle(&m_bodyRectangle, m_backgroundBrush.get());
    d2dContext->FillRectangle(&m_actionRectangle, m_backgroundBrush.get());

    d2dContext->DrawText(
        m_titleString.c_str(),
        m_titleString.size(),
        m_textFormatTitle.get(),
        m_titleRectangle,
        m_textBrush.get()
        );

    dots = dots % 10;
    for (length = 0; length < 25; length++)
    {
        wsbuffer[length] = L' ';
    }
    for (uint32_t i = 0; i < dots; i++)
    {
        wsbuffer[length++] = 0x25CF;   // This is a Dot character in the font.
        wsbuffer[length++] = L' ';
        wsbuffer[length++] = L' ';
        wsbuffer[length++] = L' ';
    }

    m_bodyString = std::wstring(wsbuffer, length);
    d2dContext->DrawText(
        m_bodyString.c_str(),
        m_bodyString.size(),
        m_textFormatBody.get(),
        m_bodyRectangle,
        m_actionBrush.get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }
}

void GameInfoOverlay::SetGameStats(int maxLevel, int hitCount, int shotCount)
{
    int length;

    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->SetTarget(m_levelBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->FillRectangle(&m_titleRectangle, m_backgroundBrush.get());
    d2dContext->FillRectangle(&m_bodyRectangle, m_backgroundBrush.get());
    m_titleString = L"High Score";

    d2dContext->DrawText(
        m_titleString.c_str(),
        m_titleString.size(),
        m_textFormatTitle.get(),
        m_titleRectangle,
        m_textBrush.get()
        );
    length = swprintf_s(
        wsbuffer,
        bufferLength,
        L"Levels Completed %d\nTotal Points %d\nTotal Shots %d",
        maxLevel,
        hitCount,
        shotCount
        );
    m_bodyString = std::wstring(wsbuffer, length);
    d2dContext->DrawText(
        m_bodyString.c_str(),
        m_bodyString.size(),
        m_textFormatBody.get(),
        m_bodyRectangle,
        m_textBrush.get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }
}

void GameInfoOverlay::SetGameOver(bool win, int maxLevel, int hitCount, int shotCount, int highScore)
{
    int length;

    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->SetTarget(m_levelBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->FillRectangle(&m_titleRectangle, m_backgroundBrush.get());
    d2dContext->FillRectangle(&m_bodyRectangle, m_backgroundBrush.get());
    if (win)
    {
        m_titleString = L"You WON!";
    }
    else
    {
        m_titleString = L"Game Over";
    }
    d2dContext->DrawText(
        m_titleString.c_str(),
        m_titleString.size(),
        m_textFormatTitle.get(),
        m_titleRectangle,
        m_textBrush.get()
        );
    length = swprintf_s(
        wsbuffer,
        bufferLength,
        L"Levels Completed %d\nTotal Points %d\nTotal Shots %d\n\nHigh Score %d\n",
        maxLevel,
        hitCount,
        shotCount,
        highScore
        );
    m_bodyString = std::wstring(wsbuffer, length);
    d2dContext->DrawText(
        m_bodyString.c_str(),
        m_bodyString.size(),
        m_textFormatBody.get(),
        m_bodyRectangle,
        m_textBrush.get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }
}

void GameInfoOverlay::SetLevelStart(int level, std::wstring const& objective, float timeLimit, float bonusTime)
{
    int length;

    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->SetTarget(m_levelBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->FillRectangle(&m_titleRectangle, m_backgroundBrush.get());
    d2dContext->FillRectangle(&m_bodyRectangle, m_backgroundBrush.get());
    length = swprintf_s(wsbuffer, bufferLength, L"Level %d", level);
    m_titleString = std::wstring(wsbuffer, length);
    d2dContext->DrawText(
        m_titleString.c_str(),
        m_titleString.size(),
        m_textFormatTitle.get(),
        m_titleRectangle,
        m_textBrush.get()
        );

    if (bonusTime > 0.0f)
    {
        length = swprintf_s(
            wsbuffer,
            bufferLength,
            L"Objective: %ls\nTime  Limit: %6.1f sec\nBonus Time: %6.1f sec\n",
            objective.c_str(),
            timeLimit,
            bonusTime
            );
    }
    else
    {
        length = swprintf_s(
            wsbuffer,
            bufferLength,
            L"Objective: %ls\nTime  Limit: %6.1f sec\n",
            objective.c_str(),
            timeLimit
            );
    }
    m_bodyString = winrt::hstring(wsbuffer, length);
    d2dContext->DrawText(
        m_bodyString.c_str(),
        m_bodyString.size(),
        m_textFormatBody.get(),
        m_bodyRectangle,
        m_textBrush.get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }
}

void GameInfoOverlay::SetPause(int /* level */, int /* hitCount */, int /* shotCount */, float /*timeRemaining */)
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->SetTarget(m_levelBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->FillRectangle(&m_titleRectangle, m_backgroundBrush.get());
    d2dContext->FillRectangle(&m_bodyRectangle, m_backgroundBrush.get());
    m_titleString = L"Game Paused";
    m_bodyString = L"";

    d2dContext->DrawText(
        m_titleString.c_str(),
        m_titleString.size(),
        m_textFormatTitle.get(),
        m_bodyRectangle,
        m_textBrush.get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }
}

void GameInfoOverlay::ShowTooSmall()
{
    m_visible = true;
    m_tooSmallActive = true;
}

void GameInfoOverlay::HideTooSmall()
{
    m_visible = false;
    m_tooSmallActive = false;
}

void GameInfoOverlay::SetAction(GameInfoOverlayCommand action)
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    d2dContext->SetTarget(m_levelBitmap.get());
    d2dContext->BeginDraw();
    d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dContext->FillRectangle(&m_actionRectangle, m_backgroundBrush.get());

    switch (action)
    {
    case GameInfoOverlayCommand::PlayAgain:
        m_actionString = L"Tap to play again ...";
        break;
    case GameInfoOverlayCommand::PleaseWait:
        m_actionString = L"Level loading, please wait ...";
        break;
    case GameInfoOverlayCommand::TapToContinue:
        m_actionString = L"Tap to continue ...";
        break;
    default:
        m_actionString = L"";
        break;
    }
    if (action != GameInfoOverlayCommand::None)
    {
        d2dContext->DrawText(
            m_actionString.c_str(),
            m_actionString.size(),
            m_textFormatBody.get(),
            m_actionRectangle,
            m_actionBrush.get()
            );
    }

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }
}