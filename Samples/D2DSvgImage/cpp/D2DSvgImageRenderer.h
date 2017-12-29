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

#include "Common\DeviceResources.h"

namespace SDKTemplate
{
    class D2DSvgImageRenderer : public DX::IDeviceNotify
    {
    public:
        D2DSvgImageRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~D2DSvgImageRenderer();

        void CreateDeviceIndependentResources();
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Draw();

        void RecolorSvg(D2D1_COLOR_F newColor);
        void ToggleTongue();
        void SetScaleValue(float val);

        // IDeviceNotify methods handle device lost and restored.
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        // Private helper for SetNewThemeColor.
        void RecolorSubtree(ID2D1SvgElement* element, D2D1_COLOR_F newColor);

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>        m_deviceResources;

        // Device-dependent resources.
        Microsoft::WRL::ComPtr<ID2D1SvgDocument>    m_svgDocument;

        float                                       m_scaleValue;
        bool                                        m_isWindowClosed;
    };
}