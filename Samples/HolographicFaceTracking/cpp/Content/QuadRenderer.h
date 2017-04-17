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
#include "Common\StepTimer.h"
#include "ShaderStructures.h"

namespace HolographicFaceTracker
{
    // This sample renderer instantiates a basic rendering pipeline.
    class QuadRenderer : public DX::Resource
    {
    public:
        QuadRenderer(std::shared_ptr<DX::DeviceResources> deviceResources);

        // DX::Resource Interface
        concurrency::task<void> CreateDeviceDependentResourcesAsync() override;
        void ReleaseDeviceDependentResources() override;

        // Repositions the sample hologram.
        void Update(
            Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose,
            Windows::Foundation::Numerics::float3 const& offset,
            DX::StepTimer const& timer);

        // Renders an NV12 image onto the quad, requires two shader resource views for each channel.
        void RenderNV12(
            ID3D11ShaderResourceView* luminanceTexture,
            ID3D11ShaderResourceView* chrominanceTexture);

        // Renders an RGB image onto the quad, only requires one texture..
        void RenderRGB(
            ID3D11ShaderResourceView* rgbTexture);

        // Updates the texture coordinates.
        void SetTexCoordScaleAndOffset(
            Windows::Foundation::Numerics::float2 const& texCoordScale,
            Windows::Foundation::Numerics::float2 const& texCoordOffset);

        void ResetTexCoordScaleAndOffset(
            Windows::Foundation::Numerics::float2 const& texCoordScale = { 1.0f, 1.0f },
            Windows::Foundation::Numerics::float2 const& texCoordOffset = { 0.0f, 0.0f });

        Windows::Foundation::Numerics::float3 const& GetPosition() const { return m_position; }
        Windows::Foundation::Numerics::float3 const& GetVelocity() const { return m_velocity; }
        Windows::Foundation::Numerics::float3 const& GetNormal() const   { return m_normal; }

    protected:
        void RenderInternal(void);

    private:
        // Direct3D resources for quad geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>        m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShaderRGB;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShaderNV12;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_modelConstantBuffer;

        // Direct3D resources for the default texture.
        Microsoft::WRL::ComPtr<ID3D11SamplerState>          m_samplerState;

        // System resources for quad geometry.
        QuadModelConstantBuffer                             m_modelConstantBufferData;
        uint32                                              m_indexCount = 0;

        // Variables used with the rendering loop.
        float                                               m_degreesPerSecond = 45.f;
        Windows::Foundation::Numerics::float3               m_targetPosition = { 0.f, 0.f, -2.f };
        Windows::Foundation::Numerics::float3               m_targetUp = { 0.f, 1.f, 0.f };
        Windows::Foundation::Numerics::float3               m_targetForward = { 0.f, 0.f, -1.f };
        Windows::Foundation::Numerics::float3               m_position = { 0.f, 0.f, -2.f };
        Windows::Foundation::Numerics::float3               m_velocity = { 0.f, 0.f, 0.f };
        Windows::Foundation::Numerics::float3               m_normal = { 0.f, 0.f, 1.f };

        Windows::Foundation::Numerics::float2               m_targetTexCoordScale = { 1.0f, 1.0f };
        Windows::Foundation::Numerics::float2               m_targetTexCoordOffset = { 0.0f, 0.0f };
        Windows::Foundation::Numerics::float2               m_texCoordScale = { 1.0f, 1.0f };
        Windows::Foundation::Numerics::float2               m_texCoordOffset = { 0.0f, 0.0f };

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                                m_usingVprtShaders = false;

        // This is the rate at which the hologram position is interpolated (LERPed) to the current location.
        const float                                         c_lerpRate = 6.0f;
    };
}
