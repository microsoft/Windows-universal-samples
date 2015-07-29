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

#include "Simple3DGame.h"
#include "DirectXSample.h"

ref class Simple3DGame;

ref class GameHud
{
internal:
    GameHud(
        _In_ const std::shared_ptr<DX::DeviceResources>& deviceResources,
        _In_ Platform::String^ titleHeader,
        _In_ Platform::String^ titleBody,
        _In_ Platform::String^ titleLicenseInfo
        );

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();
    void ReleaseDeviceDependentResources();
    void Render(_In_ Simple3DGame^ game);

    void SetLicenseInfo(_In_ Platform::String^ licenseInfo);

private:
    // Cached pointer to device resources.
    std::shared_ptr<DX::DeviceResources>                m_deviceResources;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBrushGrey;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatBody;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatBodySymbol;

    Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatTitleHeader;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatTitleBody;
    Microsoft::WRL::ComPtr<IDWriteTextFormat>           m_textFormatTitleLicense;
    Microsoft::WRL::ComPtr<ID2D1Bitmap>                 m_logoBitmap;
    Microsoft::WRL::ComPtr<IDWriteTextLayout>           m_titleHeaderLayout;
    Microsoft::WRL::ComPtr<IDWriteTextLayout>           m_titleBodyLayout;
    Microsoft::WRL::ComPtr<IDWriteTextLayout>           m_titleLicenseInfoLayout;

    bool                                                m_showTitle;
    Platform::String^                                   m_titleHeader;
    Platform::String^                                   m_titleBody;
    Platform::String^                                   m_titleLicenseInfo;
    bool                                                m_licenseInfoChanged;

    float                                               m_titleBodyVerticalOffset;
    float                                               m_titleLicenseVerticalOffset;
    D2D1_SIZE_F                                         m_logoSize;
    D2D1_SIZE_F                                         m_maxTitleSize;
};
