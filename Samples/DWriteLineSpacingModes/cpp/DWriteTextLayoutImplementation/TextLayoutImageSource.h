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

#include "TextLayout.h"

namespace DWriteTextLayoutImplementation
{
    public ref class TextLayoutImageSource sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
    {
    public:
        TextLayoutImageSource(TextLayout^ textLayout); // Dimensions determined from layout.
        TextLayoutImageSource(TextLayout^ textLayout, uint32 pixelWidth, uint32 pixelHeight); // Explicit dimensions.
        virtual ~TextLayoutImageSource();
        void Draw();

    private:
        void CreateDeviceResources();
        void BeginDraw();
        void BeginDraw(Windows::Foundation::Rect updateRect);
        void EndDraw();
        void Clear();
        void DrawText();
        void DrawLineMetricsDecorations();

        // event handlers
        void OnSuspending(Platform::Object ^sender, Windows::ApplicationModel::SuspendingEventArgs ^e);
        Windows::Foundation::EventRegistrationToken         m_suspendEventHandlerRegistrationToken;

        // Direct3D device
        Microsoft::WRL::ComPtr<ID3D11Device>                m_d3dDevice;

        // Direct2D objects
        Microsoft::WRL::ComPtr<ID2D1Device>                 m_d2dDevice;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext>          m_d2dContext;

        // DWrite wrapper object
        TextLayout^                                         m_textLayout;

        uint32                                              m_pixelWidth;
        uint32                                              m_pixelHeight;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBackgroundBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textAscentBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textDescentBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textLineGapBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_boundsBrush;
        Microsoft::WRL::ComPtr<ID2D1StrokeStyle>            m_boundsStrokeStyle;
    };

}
