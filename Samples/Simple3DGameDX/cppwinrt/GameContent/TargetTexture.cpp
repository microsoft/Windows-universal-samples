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

TargetTexture::TargetTexture(
    _In_ winrt::com_ptr<ID3D11Device3> d3dDevice,
    _In_ ID2D1Factory1* d2dFactory,
    _In_ IDWriteFactory1* dwriteFactory,
    _In_ ID2D1DeviceContext* d2dContext
    ) :
    m_d3dDevice(std::move(d3dDevice))
{
    m_d2dFactory.copy_from(d2dFactory);
    m_dwriteFactory.copy_from(dwriteFactory);
    m_d2dContext.copy_from(d2dContext);

    winrt::check_hresult(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Red, 1.f),
            m_redBrush.put()
            )
        );

    winrt::check_hresult(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::CornflowerBlue, 1.0f),
            m_blueBrush.put()
            )
        );

    winrt::check_hresult(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Green, 1.f),
            m_greenBrush.put()
            )
        );

    winrt::check_hresult(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White, 1.f),
            m_whiteBrush.put()
            )
        );

    winrt::check_hresult(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Black, 1.f),
            m_blackBrush.put()
            )
        );

    winrt::check_hresult(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Yellow, 1.f),
            m_yellowBrush.put()
            )
        );

    winrt::check_hresult(
        m_d2dContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White, 0.0f),
            m_clearBrush.put()
            )
        );

    winrt::check_hresult(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                50.0f,
                50.0f
                ),
            m_circleGeometry1.put()
            )
        );

    winrt::check_hresult(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                100.0f,
                100.0f
                ),
            m_circleGeometry2.put()
            )
        );

    winrt::check_hresult(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                150.0f,
                150.0f
                ),
            m_circleGeometry3.put()
            )
        );

    winrt::check_hresult(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                200.0f,
                200.0f
                ),
            m_circleGeometry4.put()
            )
        );

    winrt::check_hresult(
        m_d2dFactory->CreateEllipseGeometry(
            D2D1::Ellipse(
                D2D1::Point2F(256.0f, 256.0f),
                250.0f,
                250.0f
                ),
            m_circleGeometry5.put()
            )
        );

    winrt::check_hresult(
        m_dwriteFactory->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            425,        // fontsize
            L"en-US",   // locale
            m_textFormat.put()
            )
        );

    // Center the text horizontally.
    winrt::check_hresult(
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)
        );

    // Center the text vertically.
    winrt::check_hresult(
        m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER)
        );
}

void TargetTexture::CreateTextureResourceView(
    _In_ winrt::hstring const& name,
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

    winrt::com_ptr<ID3D11Texture2D> offscreenTexture;
    winrt::check_hresult(
        m_d3dDevice->CreateTexture2D(&texDesc, nullptr, offscreenTexture.put())
        );

    // Convert the Direct2D texture into a Shader Resource View
    winrt::com_ptr<ID3D11ShaderResourceView> texture;
    winrt::check_hresult(
        m_d3dDevice->CreateShaderResourceView(offscreenTexture.get(), nullptr, texture.put())
        );
#if defined(_DEBUG)
    {
        char debugName[100];
        int l = sprintf_s(debugName, sizeof(debugName) / sizeof(debugName[0]), "Simple3DGame Target %ls", name.c_str());
        winrt::check_hresult(
            texture->SetPrivateData(WKPDID_D3DDebugObjectName, l, debugName)
            );
    }
#endif

    winrt::com_ptr<IDXGISurface> dxgiSurface;
    dxgiSurface = offscreenTexture.as<IDXGISurface>();

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

    winrt::com_ptr<ID2D1Bitmap1> renderTarget;
    winrt::check_hresult(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiSurface.get(),
            &properties,
            renderTarget.put()
            )
        );

    m_d2dContext->SetTarget(renderTarget.get());
    float saveDpiX;
    float saveDpiY;

    m_d2dContext->GetDpi(&saveDpiX, &saveDpiY);
    m_d2dContext->SetDpi(96.0f, 96.0f);

    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());

    D2D1_SIZE_F renderTargetSize = renderTarget->GetSize();

    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
    m_d2dContext->FillGeometry(m_circleGeometry5.get(), m_redBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry4.get(), m_blueBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry3.get(), m_greenBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry2.get(), m_yellowBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry1.get(), m_blackBrush.get());
    m_d2dContext->DrawText(
        name.c_str(),
        name.size(),
        m_textFormat.get(),
        D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        m_whiteBrush.get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }

    m_d2dContext->SetTarget(nullptr);
    m_d2dContext->SetDpi(saveDpiX, saveDpiY);

    *textureResourceView = texture.detach();
}

void TargetTexture::CreateHitTextureResourceView(
    _In_ winrt::hstring const& name,
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

    winrt::com_ptr<ID3D11Texture2D> offscreenTexture;
    winrt::check_hresult(
        m_d3dDevice->CreateTexture2D(&texDesc, nullptr, offscreenTexture.put())
        );

    // Convert the Direct2D texture into a Shader Resource View
    winrt::com_ptr<ID3D11ShaderResourceView> texture;
    winrt::check_hresult(
        m_d3dDevice->CreateShaderResourceView(offscreenTexture.get(), nullptr, texture.put())
        );
#if defined(_DEBUG)
    {
        char debugName[100];
        int l = sprintf_s(debugName, sizeof(debugName) / sizeof(debugName[0]), "Simple3DGame HitTarget %ls", name.c_str());
        winrt::check_hresult(
            texture->SetPrivateData(WKPDID_D3DDebugObjectName, l, debugName)
            );
    }
#endif

    winrt::com_ptr<IDXGISurface> dxgiSurface;
    dxgiSurface = offscreenTexture.as<IDXGISurface>();

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

    winrt::com_ptr<ID2D1Bitmap1> renderTarget;
    winrt::check_hresult(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiSurface.get(),
            &properties,
            renderTarget.put()
            )
        );

    m_d2dContext->SetTarget(renderTarget.get());
    float saveDpiX;
    float saveDpiY;

    m_d2dContext->GetDpi(&saveDpiX, &saveDpiY);
    m_d2dContext->SetDpi(96.0f, 96.0f);

    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(D2D1::Matrix3x2F::Identity());

    D2D1_SIZE_F renderTargetSize = renderTarget->GetSize();

    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    m_d2dContext->FillGeometry(m_circleGeometry5.get(), m_yellowBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry4.get(), m_greenBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry3.get(), m_blueBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry2.get(), m_redBrush.get());
    m_d2dContext->FillGeometry(m_circleGeometry1.get(), m_whiteBrush.get());
    m_d2dContext->DrawText(
        name.c_str(),
        name.size(),
        m_textFormat.get(),
        D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
        m_blackBrush.get()
        );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }

    m_d2dContext->SetTarget(nullptr);
    m_d2dContext->SetDpi(saveDpiX, saveDpiY);

    *textureResourceView = texture.detach();
}