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
#include "TargetTexture.h"
#include "DirectXSample.h"

using namespace Microsoft::WRL;
using namespace Windows::Graphics::Display;

TargetTexture::TargetTexture(
    _In_ ID3D11Device1* d3dDevice,
    _In_ ID2D1Factory1* d2dFactory,
    _In_ IDWriteFactory1* dwriteFactory,
    _In_ ID2D1DeviceContext* d2dContext
    )
{
    m_d3dDevice = d3dDevice;
    m_d2dFactory = d2dFactory;
    m_dwriteFactory = dwriteFactory;
    m_d2dContext = d2dContext;

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Red, 1.f),
            &m_redBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::CornflowerBlue, 1.0f),
            &m_blueBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Green, 1.f),
            &m_greenBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White, 1.f),
            &m_whiteBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black, 1.f),
            &m_blackBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Yellow, 1.f),
            &m_yellowBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White, 0.0f),
            &m_clearBrush
            )
        );

    DX::ThrowIfFailed(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                50.0f,
                50.0f
                ),
            &m_circleGeometry1)
            );

    DX::ThrowIfFailed(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                100.0f,
                100.0f
                ),
            &m_circleGeometry2)
            );

    DX::ThrowIfFailed(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                150.0f,
                150.0f
                ),
            &m_circleGeometry3)
            );

    DX::ThrowIfFailed(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                200.0f,
                200.0f
                ),
            &m_circleGeometry4)
            );

    DX::ThrowIfFailed(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                250.0f,
                250.0f
                ),
            &m_circleGeometry5)
            );

    DX::ThrowIfFailed(
        m_dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            425,        // fontsize
            L"en-US",   // locale
            &m_textFormat
            )
        );

    // Center the text horizontally.
    DX::ThrowIfFailed(
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)
        );

    // Center the text vertically.
    DX::ThrowIfFailed(
        m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)
        );
}
//----------------------------------------------------------------------
void TargetTexture::CreateTextureResourceView(
    _In_ Platform::String^ name,
    _Out_ ID3D11ShaderResourceView** textureResourceView
    )
{
    // Allocate a offscreen D3D surface for D2D to render our 2D content into
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Height = 512;
    texDesc.Width = 512;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    ComPtr<ID3D11Texture2D> offscreenTexture;
    DX::ThrowIfFailed(
        m_d3dDevice->CreateTexture2D(&texDesc, nullptr, &offscreenTexture)
        );

    // Convert the Direct2D texture into a Shader Resource View
    ComPtr<ID3D11ShaderResourceView> texture;
    DX::ThrowIfFailed(
        m_d3dDevice->CreateShaderResourceView(offscreenTexture.Get(), nullptr, &texture)
        );
#if defined(_DEBUG)
    {
        char debugName[100];
        int l = sprintf_s(debugName, sizeof(debugName) / sizeof(debugName[0]), "Simple3DGame Target %ls", name->Data());
        DX::ThrowIfFailed(
            texture->SetPrivateData(WKPDID_D3DDebugObjectName, l, debugName)
            );
    }
#endif

    ComPtr<IDXGISurface> dxgiSurface;
    DX::ThrowIfFailed(
        offscreenTexture.As(&dxgiSurface)
        );

    // Create a D2D render target which can draw into our offscreen D3D
    // surface. Given that we use a constant size for the texture, we
    // fix the DPI at 96.

    D2D1_BITMAP_PROPERTIES1 properties;
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    properties.dpiX = 96;
    properties.dpiY = 96;
    properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    properties.colorContext = nullptr;

    ComPtr<ID2D1Bitmap1> renderTarget;
    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiSurface.Get(),
            &properties,
            &renderTarget
            )
        );

    m_d2dContext->SetTarget(renderTarget.Get());
    float saveDpiX;
    float saveDpiY;

    m_d2dContext->GetDpi(&saveDpiX, &saveDpiY);
    m_d2dContext->SetDpi(96.0f, 96.0f);

    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());

    D2D1_SIZE_F renderTargetSize = renderTarget->GetSize();

    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
    m_d2dContext->FillGeometry(m_circleGeometry5.Get(), m_redBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry4.Get(), m_blueBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry3.Get(), m_greenBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry2.Get(), m_yellowBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry1.Get(), m_blackBrush.Get());
    m_d2dContext->DrawText(
        name->Data(),
        name->Length(),
        m_textFormat.Get(),
        D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        m_whiteBrush.Get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->SetTarget(nullptr);
    m_d2dContext->SetDpi(saveDpiX, saveDpiY);

    *textureResourceView = texture.Detach();
}
//----------------------------------------------------------------------
void TargetTexture::CreateHitTextureResourceView(
    _In_ Platform::String^ name,
    _Out_ ID3D11ShaderResourceView** textureResourceView
    )
{
    // Allocate a offscreen D3D surface for D2D to render our 2D content into
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Height = 512;
    texDesc.Width = 512;
    texDesc.MipLevels = 1;
    texDesc.MiscFlags = 0;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    ComPtr<ID3D11Texture2D> offscreenTexture;
    DX::ThrowIfFailed(
        m_d3dDevice->CreateTexture2D(&texDesc, nullptr, &offscreenTexture)
        );

    // Convert the Direct2D texture into a Shader Resource View
    ComPtr<ID3D11ShaderResourceView> texture;
    DX::ThrowIfFailed(
        m_d3dDevice->CreateShaderResourceView(offscreenTexture.Get(), nullptr, &texture)
        );
#if defined(_DEBUG)
    {
        char debugName[100];
        int l = sprintf_s(debugName, sizeof(debugName) / sizeof(debugName[0]), "Simple3DGame HitTarget %ls", name->Data());
        DX::ThrowIfFailed(
            texture->SetPrivateData(WKPDID_D3DDebugObjectName, l, debugName)
            );
    }
#endif

    ComPtr<IDXGISurface> dxgiSurface;
    DX::ThrowIfFailed(
        offscreenTexture.As(&dxgiSurface)
        );

    // Create a D2D render target which can draw into our offscreen D3D
    // surface. Given that we use a constant size for the texture, we
    // fix the DPI at 96.

    D2D1_BITMAP_PROPERTIES1 properties;
    properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    properties.dpiX = 96;
    properties.dpiY = 96;
    properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    properties.colorContext = nullptr;

    ComPtr<ID2D1Bitmap1> renderTarget;
    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiSurface.Get(),
            &properties,
            &renderTarget
            )
        );

    m_d2dContext->SetTarget(renderTarget.Get());
    float saveDpiX;
    float saveDpiY;

    m_d2dContext->GetDpi(&saveDpiX, &saveDpiY);
    m_d2dContext->SetDpi(96.0f, 96.0f);

    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());

    D2D1_SIZE_F renderTargetSize = renderTarget->GetSize();

    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    m_d2dContext->FillGeometry(m_circleGeometry5.Get(), m_yellowBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry4.Get(), m_greenBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry3.Get(), m_blueBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry2.Get(), m_redBrush.Get());
    m_d2dContext->FillGeometry(m_circleGeometry1.Get(), m_whiteBrush.Get());
    m_d2dContext->DrawText(
        name->Data(),
        name->Length(),
        m_textFormat.Get(),
        D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        m_blackBrush.Get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_d2dContext->SetTarget(nullptr);
    m_d2dContext->SetDpi(saveDpiX, saveDpiY);

    *textureResourceView = texture.Detach();
}
//----------------------------------------------------------------------
