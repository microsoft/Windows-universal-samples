//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "DirectXHelper.h"
#include "DeviceResources.h"

// This class is responsible for initializing and rendering the title overlay visible
// at the top of the DirectX SDK samples.
namespace DWriteColorTextRenderer
{
    class SampleOverlay
    {
    public:
        SampleOverlay(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::wstring& caption);
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Render();
        float GetTitleHeightInDips();

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
        Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock>  m_stateBlock;

        Microsoft::WRL::ComPtr<ID2D1Bitmap>             m_logoBitmap;
        Microsoft::WRL::ComPtr<IDWriteTextLayout>       m_textLayout;

        UINT                                            m_idIncrement;
        bool                                            m_drawOverlay;
        std::wstring                                    m_caption;
        float                                           m_padding;
        float                                           m_textVerticalOffset;
        D2D1_SIZE_F                                     m_logoSize;
        float                                           m_overlayWidth;
    };
}
