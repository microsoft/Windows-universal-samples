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
    class SpinningCubeRenderer : public DX::Resource
    {
    public:
        SpinningCubeRenderer(std::shared_ptr<DX::DeviceResources> deviceResources);

        // DirectXResource Interface
        concurrency::task<void> CreateDeviceDependentResourcesAsync() override;
        void ReleaseDeviceDependentResources() override;

        void Update(const DX::StepTimer& timer);
        void Render();

        void SetTargetPosition(Windows::Foundation::Numerics::float3 pos) { m_targetPosition = pos; }

        Windows::Foundation::Numerics::float3 const& GetPosition() const { return m_position; }
        Windows::Foundation::Numerics::float3 const& GetVelocity() const { return m_velocity; }

    private:
        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_modelConstantBuffer;

        // System resources for cube geometry.
        CubeModelConstantBuffer                         m_modelConstantBufferData;
        uint32                                          m_indexCount = 0;

        // Variables used with the rendering loop.
        float                                           m_degreesPerSecond = 45.f;
        Windows::Foundation::Numerics::float3           m_targetPosition = { 0.f, 0.f, -2.f };
        Windows::Foundation::Numerics::float3           m_position = { 0.f, 0.f, -2.f };
        Windows::Foundation::Numerics::float3           m_velocity = { 0.f, 0.f, 0.f };

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                            m_usingVprtShaders = false;

        // This is the rate at which the hologram position is interpolated (LERPed) to the current location.
        const float                                     c_lerpRate = 4.0f;
    };
}
