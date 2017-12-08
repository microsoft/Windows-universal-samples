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

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderStructures.h"

namespace HolographicDepthBasedImageStabilization
{
    // This sample renderer instantiates a basic rendering pipeline.
    class QuadRenderer
    {
    public:
        QuadRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Render();

        // Repositions the sample hologram.
        void UpdateHologramPositionAndOrientation(
            Windows::Foundation::Numerics::float3 const& position, 
            Windows::Foundation::Numerics::float3 const& planeNormal);

        // Property accessors.
        const Windows::Foundation::Numerics::float3& GetPosition() const { return m_position; }
        const Windows::Foundation::Numerics::float3& GetNormal() const { return m_normal; }

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                m_deviceResources;

        // Direct3D resources for quad geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>        m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_modelConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState>     m_depthWriteDisable;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState>     m_depthDefault;

        // System resources for quad geometry.
        ModelConstantBuffer                                 m_modelConstantBufferData;
        uint32                                              m_indexCount = 0;

        // Variables used with the rendering loop.
        bool                                                m_loadingComplete = false;
        Windows::Foundation::Numerics::float3               m_position = { 0.f, 0.f, -2.f };
        Windows::Foundation::Numerics::float3               m_normal = { 0.f, 0.f, 1.f };

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                                m_usingVprtShaders = false;
    };
}
