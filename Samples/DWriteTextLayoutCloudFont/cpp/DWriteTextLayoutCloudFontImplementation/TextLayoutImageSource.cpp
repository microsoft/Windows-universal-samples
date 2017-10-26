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
#include "TextLayoutImageSource.h"
#include "DirectXHelper.h"

using namespace DWriteTextLayoutCloudFontImplementation;
using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;

using Windows::UI::Colors;



// Constructors:

// On older devices, the size of the canvas in which we can draw may be limited. For this sample,
// the amount of text and the size of the frame containing the text have been limited to ensure that 
// the maximum dimension never exceeds 2048 pixels (the required minimum for DirectX feature level 
// 9_1) on a 200% scale display.

TextLayoutImageSource::TextLayoutImageSource(TextLayout^ textLayout, float dpi) :
    TextLayoutImageSource(textLayout, dpi, Colors::Black, Colors::White) {}


TextLayoutImageSource::TextLayoutImageSource(TextLayout^ textLayout, float dpi, Windows::UI::Color textColor, Windows::UI::Color backgroundColor) :
    SurfaceImageSource(DX::DipsToPixelsCeil(textLayout->Width, dpi), DX::DipsToPixelsCeil(textLayout->Height, dpi)),
    m_textLayout(textLayout),
    m_dpi(dpi),
    m_width(textLayout->Width),
    m_height(textLayout->Height),
    m_pixelWidth(DX::DipsToPixelsCeil(m_width, dpi)),
    m_pixelHeight(DX::DipsToPixelsCeil(m_height, dpi))
{
    CreateDeviceResources();
    m_suspendEventHandlerRegistrationToken = Application::Current->Suspending += ref new SuspendingEventHandler(this, &TextLayoutImageSource::OnSuspending);

    SetTextColor(textColor);
    SetTextBackgroundColor(backgroundColor);
    Draw();
}


TextLayoutImageSource::~TextLayoutImageSource()
{
    Application::Current->Suspending -= m_suspendEventHandlerRegistrationToken;
}


// Other Private methods:

void TextLayoutImageSource::CreateDeviceResources()
{
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    if (DX::SdkLayersAvailable())
    {
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
#endif

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Create the Direct3D 11 API device object.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags,              // Set debug and Direct2D compatibility flags.
        featureLevels,              // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
        &m_d3dDevice,               // Returns the Direct3D device created.
        nullptr,
        nullptr
        );

    if (FAILED(hr))
    {
        // If the initialization fails, fall back to the WARP device.
        // For more information on WARP, see:
        // http://go.microsoft.com/fwlink/?LinkId=286690
        DX::ThrowIfFailed(
            D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
                0,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &m_d3dDevice,
                nullptr,
                nullptr
                )
            );
    }

    // Get the Direct3D 11.1 API device.
    ComPtr<IDXGIDevice> dxgiDevice;
    DX::ThrowIfFailed(
        m_d3dDevice.As(&dxgiDevice)
        );

    // Create the Direct2D device object and a corresponding context.
    DX::ThrowIfFailed(
        D2D1CreateDevice(
            dxgiDevice.Get(),
            nullptr,
            &m_d2dDevice
            )
        );

    DX::ThrowIfFailed(
        m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dContext
            )
        );

    // Set the DPI on the context -- it will operate in DIPs and handle scaling
    // to device pixels.
    m_d2dContext->SetDpi(m_dpi, m_dpi);

    // Grayscale text anti-aliasing is recommended for all Windows Runtime apps.
    m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    // Query for ISurfaceImageSourceNative interface.
    ComPtr<ISurfaceImageSourceNative> sisNative;
    DX::ThrowIfFailed(
        reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
        );

    // Associate the DXGI device with the SurfaceImageSource.
    DX::ThrowIfFailed(
        sisNative->SetDevice(dxgiDevice.Get())
        );
}


void TextLayoutImageSource::SetTextColor(Windows::UI::Color color)
{
    D2D1_COLOR_F d2dColor = DX::ConvertToColorF(color);
    m_d2dContext->CreateSolidColorBrush(d2dColor, &m_textBrush);
}


void TextLayoutImageSource::SetTextBackgroundColor(Windows::UI::Color color)
{
    m_backgroundColor = DX::ConvertToColorF(color);
    m_d2dContext->CreateSolidColorBrush(m_backgroundColor, &m_textBackgroundBrush);
}


void TextLayoutImageSource::Draw()
{
    // If the device was lost in one of the steps, an exception will bubble up.
    // We'll test for the relevant errors and attempt to recreate the device.
    try
    {
        BeginDraw();
        Clear();
        DrawText();
        EndDraw();
    }
    catch (Platform::Exception^ e)
    {
        HRESULT hr = e->HResult;
        if (hr == D2DERR_RECREATE_TARGET || hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device has been removed or reset, attempt to recreate it and continue drawing.
            CreateDeviceResources();
            Draw();
        }
        else
        {
            // Notify the caller by throwing an exception if any other error was encountered.
            // If hit after a recursively re-entering, we won't be making another recursive 
            // call -- this will bubble back up the call stack.
            DX::ThrowIfFailed(hr);
        }
    }
}


void TextLayoutImageSource::BeginDraw()
{
    POINT offset = {}; // Returned from the SurfaceImageSource in device pixel units.
    ComPtr<IDXGISurface> surface;

    // Express target area as a native RECT type.
    RECT updateRectNative;
    updateRectNative.left = 0;
    updateRectNative.top = 0;
    updateRectNative.right = static_cast<LONG>(m_pixelWidth);
    updateRectNative.bottom = static_cast<LONG>(m_pixelHeight);

    // Query for ISurfaceImageSourceNative interface.
    ComPtr<ISurfaceImageSourceNative> sisNative;
    DX::ThrowIfFailed(
        reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
        );

    // Begin drawing - returns a target surface and an offset to use as the top left origin when drawing.
    DX::ThrowIfFailed(
        sisNative->BeginDraw(updateRectNative, &surface, &offset)
        );

    // Create render target.
    ComPtr<ID2D1Bitmap1> bitmap;
    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            surface.Get(),
            nullptr,
            &bitmap
            )
        );

    // Set context's render target.
    m_d2dContext->SetTarget(bitmap.Get());

    // Begin drawing using D2D context.
    m_d2dContext->BeginDraw();

    // Apply a clip and transform to constrain updates to the target update area.
    // This is required to ensure coordinates within the target surface remain
    // consistent by taking into account the offset returned by BeginDraw, and
    // can also improve performance by optimizing the area that is drawn by D2D.
    // Apps should always account for the offset output parameter returned by 
    // BeginDraw, since it may not match the passed updateRect input parameter's location.
    m_d2dContext->PushAxisAlignedClip(
        D2D1::RectF(
            floorf(DX::PixelsToDips(offset.x, m_dpi)),
            floorf(DX::PixelsToDips(offset.y, m_dpi)),
            ceilf(DX::PixelsToDips(offset.x, m_dpi) + m_width),
            ceilf(DX::PixelsToDips(offset.y, m_dpi) + m_height)
            ),
        D2D1_ANTIALIAS_MODE_ALIASED
        );

    m_d2dContext->SetTransform(
        D2D1::Matrix3x2F::Translation(
            DX::PixelsToDips(offset.x, m_dpi),
            DX::PixelsToDips(offset.y, m_dpi)
            )
        );
}


void TextLayoutImageSource::EndDraw()
{
    // Remove the transform and clip applied in BeginDraw since
    // the target area can change on every update.
    m_d2dContext->SetTransform(D2D1::IdentityMatrix());
    m_d2dContext->PopAxisAlignedClip();

    // Remove the render target and end drawing.
    m_d2dContext->SetTarget(nullptr);
    DX::ThrowIfFailed(
        m_d2dContext->EndDraw()
        );

    // Query for ISurfaceImageSourceNative interface.
    Microsoft::WRL::ComPtr<ISurfaceImageSourceNative> sisNative;
    DX::ThrowIfFailed(
        reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
        );

    DX::ThrowIfFailed(
        sisNative->EndDraw()
        );
}


void TextLayoutImageSource::Clear()
{
    m_d2dContext->Clear(m_backgroundColor);
}


void TextLayoutImageSource::DrawText()
{
    // The underlying IDWriteTextLayout from the TextLayout object will be needed
    // to interact with D2D.
    Microsoft::WRL::ComPtr<IDWriteTextLayout> dwriteTextLayout = m_textLayout->GetDWriteTextLayout();

    DWRITE_TEXT_METRICS textMetrics;
    DX::ThrowIfFailed(
        dwriteTextLayout->GetMetrics(&textMetrics)
        );

    m_d2dContext->FillRectangle(
        D2D1::RectF(0.0f, 0.0f, textMetrics.widthIncludingTrailingWhitespace, textMetrics.height),
        m_textBackgroundBrush.Get()
        );

    m_d2dContext->DrawTextLayout(D2D1::Point2F(), dwriteTextLayout.Get(), m_textBrush.Get());
}


// event handlers

void TextLayoutImageSource::OnSuspending(Object ^sender, SuspendingEventArgs ^e)
{
    // Hint to the display driver that the app is entering an idle state and that its 
    // memory can be used temporarily for other apps.

    ComPtr<IDXGIDevice3> dxgiDevice;
    m_d3dDevice.As(&dxgiDevice);
    dxgiDevice->Trim();
}
