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
#include "RectRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "..\Common\Constants.h"

using namespace SDKTemplate;

using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Input;
using namespace Microsoft::WRL;

// Initialization.
RectRenderer::RectRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    deviceResources(deviceResources),
    rect(),
    rectTransform()
{
    CreateDeviceDependentResources();

    //Initialize start position and rect size
    rectTransform = D2D1::Matrix3x2F::Identity();
    rectSize = bigRectSize;
}

void RectRenderer::CreateDeviceDependentResources()
{
    // Create a brush with which to draw the rect.
    DX::ThrowIfFailed(
        deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::LightSlateGray),
            &brush
            )
        );
}

// Release all references to resources that depend on the graphics device.
// This method is invoked when the device is lost and resources are no longer usable.
void RectRenderer::ReleaseDeviceDependentResources()
{
    brush.Reset();
}

// Renders one frame.
void RectRenderer::Render()
{
    ComPtr<ID2D1DeviceContext1> deviceContext = deviceResources->GetD2DDeviceContext();

    deviceContext->BeginDraw();

    // Rotate the rendered scene based on the current orientation of the device and rotation of the rect
    deviceContext->SetTransform(rectTransform * deviceResources->GetOrientationTransform2D());

    // Draw the rect at its current position.
    rect = D2D1::RectF(0, 0, rectSize, rectSize);
    deviceContext->FillRectangle(rect, brush.Get());

    // Restore the transform to nothing.
    deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = deviceContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        DX::ThrowIfFailed(hr);
    }
}

// Update the rect transform based on input
void RectRenderer::UpdateRectTransform(Point newPosition, Windows::UI::Input::ManipulationDelta delta)
{
    D2D1_POINT_2F center = D2D1::Point2F(newPosition.X, newPosition.Y);

    D2D1::Matrix3x2F deltaTransform =
        D2D1::Matrix3x2F::Scale(delta.Scale, delta.Scale, center) *
        D2D1::Matrix3x2F::Rotation(delta.Rotation, center) *
        D2D1::Matrix3x2F::Translation(delta.Translation.X, delta.Translation.Y);

    rectTransform = rectTransform * deltaTransform;
}

bool RectRenderer::HitTest(Point position)
{
    D2D1::Matrix3x2F localInverseTransform = rectTransform;
    if (D2D1InvertMatrix(&localInverseTransform))
    {
        // "Untransform" (x, y) from parent coordinate system to object's initial principal axes coordinate system.
        D2D1_POINT_2F pt = localInverseTransform.TransformPoint(D2D1::Point2F(position.X, position.Y));

        // Hit test point against rectangle
        D2D1_RECT_F rect = D2D1::RectF(0, 0, rectSize, rectSize);   //initial rectangle
        return (pt.x >= rect.left) && (pt.x <= rect.right) &&
               (pt.y >= rect.top)  && (pt.y <= rect.bottom);
    }

    return false;
}

void RectRenderer::SetRectSize(float newSize)
{
    rectSize = newSize;
}

void RectRenderer::ResetRect()
{
    rectTransform = D2D1::Matrix3x2F::Identity() * D2D1::Matrix3x2F::Translation(0, 0);
}

void RectRenderer::SetBrushColor(D2D1::ColorF color)
{
    brush->SetColor(color);
}