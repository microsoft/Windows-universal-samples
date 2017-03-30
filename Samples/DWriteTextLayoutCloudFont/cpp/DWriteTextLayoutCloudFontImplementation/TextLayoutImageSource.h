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

namespace DWriteTextLayoutCloudFontImplementation
{
    [Windows::Foundation::Metadata::WebHostHiddenAttribute]
    public ref class TextLayoutImageSource sealed : Windows::UI::Xaml::Media::Imaging::SurfaceImageSource
    {
    public:
        TextLayoutImageSource(TextLayout^ textLayout, float dpi);
        TextLayoutImageSource(TextLayout^ textLayout, float dpi, Windows::UI::Color textColor, Windows::UI::Color backgroundColor);
        virtual ~TextLayoutImageSource();

    private:
        void CreateDeviceResources();
        void SetTextBackgroundColor(Windows::UI::Color color);
        void SetTextColor(Windows::UI::Color color);
        void Draw();
        void BeginDraw();
        void EndDraw();
        void Clear();
        void DrawText();

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

        float                                               m_dpi;
        float                                               m_width; // width in DIPs -- for interaction with D2D
        float                                               m_height; // height in DIPs
        uint32                                              m_pixelWidth; // width in device pixels -- for interaction with SurfaceImageSource
        uint32                                              m_pixelHeight; // height in device pixels
        D2D1_COLOR_F                                        m_backgroundColor;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBrush;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>        m_textBackgroundBrush;
    };
}
