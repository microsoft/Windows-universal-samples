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

namespace CameraStreamCoordinateMapper
{
    // This sample renderer instantiates a basic rendering pipeline.
    class QuadRenderer
    {
    public:
        QuadRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();

        void Render(ID3D11ShaderResourceView* texture, float aspectRatio);

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // Direct3D resources for the geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>       m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>       m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>       m_constantBuffer;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

        // System resources for the geometry.
        ModelViewProjectionConstantBuffer m_constantBufferData;
        uint32 m_indexCount = 0u;
    };
}

