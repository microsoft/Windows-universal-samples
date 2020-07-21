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

// GameHud:
// This class maintains the resources necessary to draw the Heads Up Display, which is a
// 2D overlay on top of the main 3D graphics generated for the game.
// The GameHud Render method expects to be called within the context of a D2D BeginDraw
// and makes D2D drawing calls to draw the text elements on the window.

#include "DeviceResources.h"
#include "Simple3DGame.h"

class GameHud
{
public:
    GameHud(
        _In_ std::shared_ptr<DX::DeviceResources> const& deviceResources,
        _In_ winrt::hstring const& titleHeader,
        _In_ winrt::hstring const& titleBody
        );
    GameHud(GameHud const&) = delete;
    void operator=(GameHud const&) = delete;

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    void ReleaseDeviceDependentResources();
    void Render(_In_ std::shared_ptr<Simple3DGame> const& game);

private:
    // Cached pointer to device resources.
    std::shared_ptr<DX::DeviceResources> m_deviceResources;

    winrt::com_ptr<ID2D1SolidColorBrush> m_textBrush;
    winrt::com_ptr<IDWriteTextFormat>    m_textFormatBody;
    winrt::com_ptr<IDWriteTextFormat>    m_textFormatBodySymbol;

    winrt::com_ptr<IDWriteTextFormat>    m_textFormatTitleHeader;
    winrt::com_ptr<IDWriteTextFormat>    m_textFormatTitleBody;
    winrt::com_ptr<ID2D1Bitmap>          m_logoBitmap;
    winrt::com_ptr<IDWriteTextLayout>    m_titleHeaderLayout;
    winrt::com_ptr<IDWriteTextLayout>    m_titleBodyLayout;

    bool                                 m_showTitle;
    winrt::hstring                       m_titleHeader;
    winrt::hstring                       m_titleBody;

    float                                m_titleBodyVerticalOffset;
    D2D1_SIZE_F                          m_logoSize;
    D2D1_SIZE_F                          m_maxTitleSize;
};