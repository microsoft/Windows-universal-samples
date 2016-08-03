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

#include "common\DeviceResources.h"

namespace HolographicTagAlongSample
{
    class DistanceFieldRenderer
    {
    public:
        DistanceFieldRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, unsigned int const& textureWidth, unsigned int const& textureHeight);
        
        void RenderDistanceField(ID3D11ShaderResourceView* texture);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

        ID3D11ShaderResourceView*   GetTexture()        const { return m_shaderResourceView.Get();  };
        unsigned int const&         GetRenderCount()    const { return m_renderCount;               };
        
    private:
        // Cached pointer to device resources.
        const std::shared_ptr<DX::DeviceResources>          m_deviceResources;

        // Direct3D resources for filtering a texture to an off-screen render target.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_shaderResourceView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>      m_d3dDepthStencilView;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_renderTargetView;

        // CPU-based variables for configuring the offscreen render target.
        const UINT                                          m_textureWidth;
        const UINT                                          m_textureHeight;

        // System resources for quad geometry.
        uint32                                              m_indexCount        = 0;
        uint32                                              m_vertexStride      = 0;

        // Variables used with the rendering loop.
        bool                                                m_loadingComplete   = false;
        unsigned int                                        m_renderCount       = 0;
    };
}