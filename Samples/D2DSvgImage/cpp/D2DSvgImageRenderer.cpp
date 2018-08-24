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
#include "D2DSvgImageRenderer.h"
#include "DirectXPage.xaml.h"
#include "Common\DirectXHelper.h"

using namespace SDKTemplate;

using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace std;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Input;

static const float sc_svgSize = 500.0f;

D2DSvgImageRenderer::D2DSvgImageRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    m_scaleValue(1.0f),
    m_isWindowClosed(false)
{
    // Register to be notified if the GPU device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    CreateDeviceIndependentResources();
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

D2DSvgImageRenderer::~D2DSvgImageRenderer()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

void D2DSvgImageRenderer::CreateDeviceIndependentResources()
{
}

void D2DSvgImageRenderer::CreateDeviceDependentResources()
{
    auto d2dContext = m_deviceResources->GetD2DDeviceContext();

    StorageFolder^ packageFolder = Windows::ApplicationModel::Package::Current->InstalledLocation;

    // Retrieve the SVG file from the app package.
    create_task(packageFolder->GetFileAsync("Assets\\drawing.svg")).then([=](StorageFile^ file)
    {
        // Open the SVG file for reading.
        return file->OpenAsync(FileAccessMode::Read);

    }).then([=](IRandomAccessStream^ stream)
    {
        // Wrap the WinRT stream with a COM stream.
        ComPtr<IStream> iStream;
        DX::ThrowIfFailed(
            CreateStreamOverRandomAccessStream(
                stream,
                IID_PPV_ARGS(&iStream)
                )
            );

        // Parse the file stream into an SVG document.
        DX::ThrowIfFailed(
            d2dContext->CreateSvgDocument(
                iStream.Get(),
                D2D1::SizeF(sc_svgSize, sc_svgSize), // Create the document at a size of 500x500 DIPs.
                &m_svgDocument
                )
            );
    });
}

void D2DSvgImageRenderer::ReleaseDeviceDependentResources()
{
    m_svgDocument.Reset();
}

void D2DSvgImageRenderer::CreateWindowSizeDependentResources()
{
}

void D2DSvgImageRenderer::Draw()
{
    ID2D1DeviceContext5* d2dContext = m_deviceResources->GetD2DDeviceContext();
    Size logicalSize = m_deviceResources->GetLogicalSize();
    
    d2dContext->BeginDraw();

    d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));

    // DeviceResources explicitly handles screen rotation by setting the rotation on the swap chain.
    // In order for Direct2D content to appear correctly on this swap chain, the app is responsible for
    // explicitly "prerotating" by setting the appropriate transform before drawing.
    D2D1_MATRIX_3X2_F transform = m_deviceResources->GetOrientationTransform2D();
    
    // Also perform a translation to center the SVG image in the window.
    transform = transform * D2D1::Matrix3x2F::Translation(
        (logicalSize.Width - sc_svgSize) / 2.0f,
        (logicalSize.Height - sc_svgSize) / 2.0f
        );

    // Also perform a scale based on the current slider value.
    transform = transform * D2D1::Matrix3x2F::Scale(
        m_scaleValue,
        m_scaleValue,
        D2D1::Point2F(
            logicalSize.Width / 2.0f,
            logicalSize.Height / 2.0f
            )
        );

    m_deviceResources->GetD2DDeviceContext()->SetTransform(transform);

    if (m_svgDocument)
    {
        // Draw the SVG document.
        d2dContext->DrawSvgDocument(m_svgDocument.Get());
    }

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }

    m_deviceResources->Present();
}

// Performs a rudimentary recoloring of the renderer's SVG document using
// Direct2D's SVG OM APIs. Recursively walks the SVG tree and changes the
// "fill" property of any non-black nodes to the specified new color.
void D2DSvgImageRenderer::RecolorSvg(D2D1_COLOR_F newColor)
{
    if (m_svgDocument)
    {
        // Retrieve the root element of the SVG document.
        ComPtr<ID2D1SvgElement> root;
        m_svgDocument->GetRoot(&root);

        // Recursively recolor the tree, beginning at the root.
        RecolorSubtree(root.Get(), newColor);
    }
}

// Helper method for RecolorSvg which recursively recolors the given subtree of SVG document using
// the given color.
void D2DSvgImageRenderer::RecolorSubtree(ID2D1SvgElement* element, D2D1_COLOR_F newColor)
{
    // Check if this SVG element has a "fill" element explicitly specified or inherited.
    if (element->IsAttributeSpecified(L"fill"))
    {
        // Retrieve the value of this element's "fill" attribute, as a paint object.
        ComPtr<ID2D1SvgPaint> paint;
        DX::ThrowIfFailed(element->GetAttributeValue(L"fill", &paint));

        // Check the type of paint object that was set. There are different types of
        // paints, such as plain colors, URLs, or the 'none' type. For this app we
        // only want to modify fills that were set to a specific color.
        D2D1_SVG_PAINT_TYPE paintType = paint->GetPaintType();
        if (paintType == D2D1_SVG_PAINT_TYPE_COLOR)
        {
            // Retrieve the color of this paint.
            D2D1_COLOR_F oldColor;
            paint->GetColor(&oldColor);

            // If the old color not black, change it to newColor.
            if (oldColor.r != 0.0f || oldColor.g != 0.0f || oldColor.b != 0.0f)
            {
                // Attributes that are returned as an interface (such as ID2D1SvgPaint)
                // are returned by reference, rather than by value. This means that
                // modifying them (such as by calling SetColor, below) directly modifies
                // the SVG document, without needing to call SetAttributeValue.
                DX::ThrowIfFailed(paint->SetColor(newColor));
            }
        }
    }

    // Now, iterate through any child nodes of this SVG element and recursively recolor them.

    // Retrieve the first child of the current element, if it has any.
    ComPtr<ID2D1SvgElement> child;
    element->GetFirstChild(&child);

    while (child != nullptr)
    {
        // Recursively recolor the subtree starting with this child node.
        RecolorSubtree(child.Get(), newColor);

        // Retrieve the next child, if any.
        ComPtr<ID2D1SvgElement> nextChild;
        element->GetNextChild(child.Get(), &nextChild);

        // Move to the next child.
        child = nextChild;
    }
}

// Performs a rudimentary SVG OM operation on the renderer's SVG document
// to toggle the visibility of the element in the SVG drawing with the
// identifier "tongue".
void D2DSvgImageRenderer::ToggleTongue()
{
    if (m_svgDocument)
    {
        // Retrieve the SVG element with the identifier "tongue". (Open
        // drawing.svg to see where this is defined by the SVG author.)
        ComPtr<ID2D1SvgElement> tongueElement;
        DX::ThrowIfFailed(m_svgDocument->FindElementById(L"tongue", &tongueElement));

        // Retrieve the value of the element's "display" attribute.
        D2D1_SVG_DISPLAY displayValue;
        DX::ThrowIfFailed(tongueElement->GetAttributeValue(L"display", &displayValue));

        // Invert the value of the retrieved variable.
        if (displayValue == D2D1_SVG_DISPLAY::D2D1_SVG_DISPLAY_NONE)
        {
            displayValue = D2D1_SVG_DISPLAY::D2D1_SVG_DISPLAY_INLINE;
        }
        else
        {
            displayValue = D2D1_SVG_DISPLAY::D2D1_SVG_DISPLAY_NONE;
        }

        // Attributes that are returned as an enum (such as D2D1_SVG_DISPLAY) are
        // returned by value, rather than by reference. This means that in order
        // to change the SVG document itself, we must store the modified variable
        // back into the SVG document using SetAttributeValue. The new value will
        // be used the next time the SVG document is drawn.
        DX::ThrowIfFailed(tongueElement->SetAttributeValue(L"display", displayValue));
    }
}

void D2DSvgImageRenderer::SetScaleValue(float val)
{
    m_scaleValue = val;
}

// Notifies renderers that device resources need to be released.
void D2DSvgImageRenderer::OnDeviceLost()
{
    ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void D2DSvgImageRenderer::OnDeviceRestored()
{
    CreateDeviceDependentResources();
}
