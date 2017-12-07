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

namespace HolographicFaceTracker
{
    // Simple class to manage a NV12 texture and use it in Direct3D 11
    class NV12VideoTexture : public DX::Resource
    {
    public:
        NV12VideoTexture(
            std::shared_ptr<DX::DeviceResources> deviceResources,
            uint32_t width,
            uint32_t height);

        void CopyFromVideoMediaFrame(
            Windows::Media::Capture::Frames::VideoMediaFrame^ source);

        // DX::Resource Interface
        concurrency::task<void> CreateDeviceDependentResourcesAsync() override;
        void ReleaseDeviceDependentResources() override;

        ID3D11Texture2D* GetTexture(void) const { return m_texture.Get(); }
        ID3D11ShaderResourceView* GetLuminanceTexture(void) const { return m_luminanceView.Get(); }
        ID3D11ShaderResourceView* GetChrominanceTexture(void) const { return m_chrominanceView.Get(); }

    protected:
        Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_luminanceView;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_chrominanceView;

        uint32_t const m_width;
        uint32_t const m_height;
    };
}
