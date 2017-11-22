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

using namespace DWriteTextLayoutImplementation;
using namespace Platform;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;


// constructors

TextLayoutImageSource::TextLayoutImageSource(TextLayout^ textLayout) :
    TextLayoutImageSource(textLayout, textLayout->Width, textLayout->Height) {}


TextLayoutImageSource::TextLayoutImageSource(TextLayout^ textLayout, uint32 pixelWidth, uint32 pixelHeight) :
    SurfaceImageSource(static_cast<int>(pixelWidth), static_cast<int>(pixelHeight)),
    m_textLayout(textLayout),
    m_pixelWidth(pixelWidth),
    m_pixelHeight(pixelHeight)
{
    CreateDeviceResources();
    m_suspendEventHandlerRegistrationToken = (Application::Current->Suspending += ref new SuspendingEventHandler(this, &TextLayoutImageSource::OnSuspending));
}


TextLayoutImageSource::~TextLayoutImageSource()
{
    Application::Current->Suspending -= m_suspendEventHandlerRegistrationToken;
}


// private methods:

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
    D3D_FEATURE_LEVEL supportedFeatureLevel; // We will need this to determine size limits for our SurfaceImageSource.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags,              // Set debug and Direct2D compatibility flags.
        featureLevels,              // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
        &m_d3dDevice,               // Returns the Direct3D device created.
        &supportedFeatureLevel,
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
                &supportedFeatureLevel,
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

    // Query for ISurfaceImageSourceNative interface.
    ComPtr<ISurfaceImageSourceNative> sisNative;
    DX::ThrowIfFailed(
        reinterpret_cast<IUnknown*>(this)->QueryInterface(IID_PPV_ARGS(&sisNative))
        );

    // Associate the DXGI device with the SurfaceImageSource.
    DX::ThrowIfFailed(
        sisNative->SetDevice(dxgiDevice.Get())
        );

    // Create D2D brushes.
    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_textBrush)
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightYellow), &m_textBackgroundBrush)
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightBlue), &m_textAscentBrush)
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightPink), &m_textDescentBrush)
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen), &m_textLineGapBrush)
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_boundsBrush)
        );

    // Create D2D stroke style
    ComPtr<ID2D1Factory> d2dFactory;
    m_d2dDevice->GetFactory(&d2dFactory);

    float const dashes[] = { 2.0f, 2.0f };

    auto strokeProperties = D2D1::StrokeStyleProperties(
        D2D1_CAP_STYLE_FLAT,
        D2D1_CAP_STYLE_FLAT,
        D2D1_CAP_STYLE_FLAT,
        D2D1_LINE_JOIN_MITER,
        1.0f,
        D2D1_DASH_STYLE_CUSTOM,
        0.0f
        );

    DX::ThrowIfFailed(
        d2dFactory->CreateStrokeStyle(strokeProperties, dashes, ARRAYSIZE(dashes), &m_boundsStrokeStyle)
        );
}


void TextLayoutImageSource::Draw()
{
    // If the device was lost in one of the steps, an exception will bubble up.
    // We'll test for the relevant errors and attempt to recreate the device.
    try
    {
        BeginDraw();
        Clear();
        DrawLineMetricsDecorations();
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
    BeginDraw(Windows::Foundation::Rect(0.0f, 0.0f, static_cast<float>(m_pixelWidth), static_cast<float>(m_pixelHeight)));
}


void TextLayoutImageSource::BeginDraw(Windows::Foundation::Rect updateRect)
{
    // Assumed: the caller knows not to use SurfaceImageSource to draw content
    // that might exceed the available maximum texture dimensions of the device.
    // If that is a possibility, then VirutalSurfaceImageSource should be used
    // instead. For more information on maxiumum texture dimensions, see:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476876
    //
    // For information on VirtualSurfaceImageSource versus SurfaceImageSource, see
    // https://msdn.microsoft.com/en-us/library/windows/apps/hh825871

    POINT offset;
    ComPtr<IDXGISurface> surface;

    // Express target area as a native RECT type.
    RECT updateRectNative;
    updateRectNative.left = static_cast<LONG>(updateRect.Left);
    updateRectNative.top = static_cast<LONG>(updateRect.Top);
    updateRectNative.right = static_cast<LONG>(updateRect.Right);
    updateRectNative.bottom = static_cast<LONG>(updateRect.Bottom);

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
            static_cast<float>(offset.x),
            static_cast<float>(offset.y),
            static_cast<float>(offset.x + updateRect.Width),
            static_cast<float>(offset.y + updateRect.Height)
            ),
        D2D1_ANTIALIAS_MODE_ALIASED
        );

    m_d2dContext->SetTransform(
        D2D1::Matrix3x2F::Translation(
            static_cast<float>(offset.x),
            static_cast<float>(offset.y)
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
    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
}


void TextLayoutImageSource::DrawText()
{
    // The underlying IDWriteTextLayout from the TextLayout object will be needed
    // to interact with D2D.
    Microsoft::WRL::ComPtr<IDWriteTextLayout> dwriteTextLayout = m_textLayout->GetDWriteTextLayout();

    m_d2dContext->DrawTextLayout(D2D1::Point2F(), dwriteTextLayout.Get(), m_textBrush.Get());
}


void TextLayoutImageSource::DrawLineMetricsDecorations()
{
    // Decorations will be drawn to indicate the metrics of each line in the text layout.

    // Start by filling a background color.
    float right = static_cast<float>(m_textLayout->Width);
    DWRITE_TEXT_METRICS textMetrics = m_textLayout->GetTextMetrics();

    m_d2dContext->FillRectangle(
        D2D1::RectF(0.0f, 0.0f, right, textMetrics.height),
        m_textBackgroundBrush.Get()
        );

    // Get the line metrics.
    std::vector<DWRITE_LINE_METRICS1> lineMetrics = m_textLayout->GetLineMetrics();
    uint32 lineCount = static_cast<uint32>(lineMetrics.size());

    float y = 0;

    for (uint32 i = 0; i < lineCount; ++i)
    {
        // Get the line metrics structure for the current line.
        auto& line = lineMetrics[i];

        // Calculate distances relative to the top of the text layout.
        float topOfLine = y + line.leadingBefore;
        float baseline = y + line.baseline;
        float bottomOfLine = y + line.height - line.leadingAfter;

        // Draw different colored backgrounds to indicate the ascent 
        // and descent distances.
        if (baseline < bottomOfLine)
        {
            m_d2dContext->FillRectangle(D2D1::RectF(0, topOfLine, right, baseline), m_textAscentBrush.Get());
            m_d2dContext->FillRectangle(D2D1::RectF(0, baseline, right, bottomOfLine), m_textDescentBrush.Get());
        }
        else
        {
            // The baseline distance is greater than the line height. This
            // means that a portion of the line will be drawn outside the
            // nominal extent of the line. When uniform spacing is used,
            // this will occur if the baseline that was specified is 
            // greater than the specified height. When proportional spacing
            // is used, this will occur if the baseline that was specified 
            // is greater than 1.0.
            m_d2dContext->FillRectangle(D2D1::RectF(0, topOfLine, right, bottomOfLine), m_textAscentBrush.Get());
        }

        // Draw a bounding line above the current text line.
        m_d2dContext->DrawLine(D2D1::Point2F(0, y), D2D1::Point2F(right, y), m_boundsBrush.Get(), 1.0f, m_boundsStrokeStyle.Get());

        y += line.height;
    }

    // Draw a bounding line after the last text line.
    m_d2dContext->DrawLine(D2D1::Point2F(0, y), D2D1::Point2F(right, y), m_boundsBrush.Get(), 1.0f, m_boundsStrokeStyle.Get());

}


// event handlers

void TextLayoutImageSource::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
    // Hint to the display driver that the app is entering an idle state and that its 
    // memory can be used temporarily for other apps.

    ComPtr<IDXGIDevice3> dxgiDevice;
    m_d3dDevice.As(&dxgiDevice);
    dxgiDevice->Trim();
}
