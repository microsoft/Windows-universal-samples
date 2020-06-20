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

// TargetTexture:
// This is a helper class to procedurally generate textures for game
// targets. There are two versions of the textures, one when it is
// hit and the other is when it is not.
// The class creates the necessary resources to draw the texture into
// an off screen resource at initialization time.

class TargetTexture
{
public:
    TargetTexture(
        _In_ winrt::com_ptr<ID3D11Device3> d3dDevice,
        _In_ ID2D1Factory1* d2dFactory,
        _In_ IDWriteFactory1* dwriteFactory,
        _In_ ID2D1DeviceContext* d2dContext
        );

    void CreateTextureResourceView(
        _In_ winrt::hstring const& name,
        _Out_ ID3D11ShaderResourceView** textureResourceView
        );

    void CreateHitTextureResourceView(
        _In_ winrt::hstring const& name,
        _Out_ ID3D11ShaderResourceView** textureResourceView
        );

private:
    winrt::com_ptr<ID3D11Device1>           m_d3dDevice;
    winrt::com_ptr<ID2D1Factory1>           m_d2dFactory;
    winrt::com_ptr<ID2D1DeviceContext>      m_d2dContext;
    winrt::com_ptr<IDWriteFactory1>         m_dwriteFactory;

    winrt::com_ptr<ID2D1SolidColorBrush>    m_redBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_blueBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_greenBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_whiteBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_blackBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_yellowBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_clearBrush;

    winrt::com_ptr<ID2D1EllipseGeometry>    m_circleGeometry1;
    winrt::com_ptr<ID2D1EllipseGeometry>    m_circleGeometry2;
    winrt::com_ptr<ID2D1EllipseGeometry>    m_circleGeometry3;
    winrt::com_ptr<ID2D1EllipseGeometry>    m_circleGeometry4;
    winrt::com_ptr<ID2D1EllipseGeometry>    m_circleGeometry5;

    winrt::com_ptr<IDWriteTextFormat>       m_textFormat;
};