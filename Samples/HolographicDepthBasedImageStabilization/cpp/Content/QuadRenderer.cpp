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

#include "pch.h"

#include <array>
#include "QuadRenderer.h"
#include "Common\DirectXHelper.h"

using namespace HolographicDepthBasedImageStabilization;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the quad geometry.
QuadRenderer::QuadRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    CreateDeviceDependentResources();
}

// This function uses a SpatialPointerPose to position the world-locked hologram
// two meters in front of the user's heading.
void QuadRenderer::UpdateHologramPositionAndOrientation(
    float3 const& position, 
    float3 const& planeNormal)
{
    m_position = position;
    m_normal = planeNormal;

    // Rotate the quad.
    const XMVECTOR zAxis         = XMLoadFloat3(&m_normal);
    const XMVECTOR unitY         = XMLoadFloat3(&float3::unit_y());
    const XMVECTOR unitZ         = XMLoadFloat3(&float3::unit_z());
    const XMVECTOR forOrthogonal = XMVector3NearEqual(zAxis, unitY, g_XMEpsilon) ? unitZ : unitY;
    const XMVECTOR orthogonal    = XMVector3Normalize(XMVector3Cross(zAxis, forOrthogonal));
    const XMVECTOR complementary = XMVector3Normalize(XMVector3Cross(orthogonal, zAxis));
    XMMATRIX rotateToPlaneNormal = XMMatrixTranspose(XMMATRIX(
        orthogonal,
        complementary,
        zAxis,
        g_XMIdentityR3));

    // Position the quad.
    const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));

    // World-space transform.
    XMStoreFloat4x4(
        &m_modelConstantBufferData.model, 
        XMMatrixTranspose(rotateToPlaneNormal * modelTranslation));

    // Loading is asynchronous. Resources must be created before they can be updated.
    if (!m_loadingComplete)
    {
        return;
    }

    // Use the D3D device context to update Direct3D device-based resources.
    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Update the model transform buffer for the hologram.
    context->UpdateSubresource(
        m_modelConstantBuffer.Get(),
        0,
        nullptr,
        &m_modelConstantBufferData,
        0,
        0
        );
}

void QuadRenderer::Render()
{
    // Loading is asynchronous. Resources must be created before drawing can occur.
    if (!m_loadingComplete)
    {
        return;
    }

    const auto context = m_deviceResources->GetD3DDeviceContext();
    
    // This visualization should not affect the plane fit, so depth write is turned off.
    context->OMSetDepthStencilState(m_depthWriteDisable.Get(), 0);

    // Each vertex is one instance of the VertexPositionColor struct.
    const UINT stride = sizeof(VertexPositionColor);
    const UINT offset = 0;
    context->IASetVertexBuffers(
        0,
        1,
        m_vertexBuffer.GetAddressOf(),
        &stride,
        &offset
        );
    context->IASetIndexBuffer(
        m_indexBuffer.Get(),
        DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
        0
        );
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(m_inputLayout.Get());

    // Attach the vertex shader.
    context->VSSetShader(
        m_vertexShader.Get(),
        nullptr,
        0
        );
    // Apply the model constant buffer to the vertex shader.
    context->VSSetConstantBuffers(
        0,
        1,
        m_modelConstantBuffer.GetAddressOf()
        );

    if (!m_usingVprtShaders)
    {
        // On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
        // VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
        // a pass-through geometry shader sets the render target ID.
        context->GSSetShader(
            m_geometryShader.Get(),
            nullptr,
            0
            );
    }

    // Attach the pixel shader.
    context->PSSetShader(
        m_pixelShader.Get(),
        nullptr,
        0
        );

    // Draw the objects.
    context->DrawIndexedInstanced(
        m_indexCount,   // Index count per instance.
        2,              // Instance count.
        0,              // Start index location.
        0,              // Base vertex location.
        0               // Start instance location.
        );

    context->OMSetDepthStencilState(m_depthDefault.Get(), 0);
}

void QuadRenderer::CreateDeviceDependentResources()
{
    m_usingVprtShaders = m_deviceResources->GetDeviceSupportsVprt();

    // If the optional VPRT feature is supported by the graphics device, we
    // can avoid using geometry shaders to set the render target array index.
    std::wstring vertexShaderFileName = m_usingVprtShaders ? L"ms-appx:///VprtVertexShader.cso" : L"ms-appx:///VertexShader.cso";

    // Load shaders asynchronously.
    task<std::vector<byte>> loadVSTask = DX::ReadDataAsync(vertexShaderFileName);
    task<std::vector<byte>> loadPSTask = DX::ReadDataAsync(L"ms-appx:///PixelShader.cso");

    task<std::vector<byte>> loadGSTask;
    if (!m_usingVprtShaders)
    {
        // Load the pass-through geometry shader.
        loadGSTask = DX::ReadDataAsync(L"ms-appx:///GeometryShader.cso");
    }

    // After the vertex shader file is loaded, create the shader and input layout.
    task<void> createVSTask = loadVSTask.then([this] (const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateVertexShader(
                fileData.data(),
                fileData.size(),
                nullptr,
                &m_vertexShader
                )
            );

        constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 3> vertexDesc =
        {{
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        }};

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateInputLayout(
                vertexDesc.data(),
                vertexDesc.size(),
                fileData.data(),
                fileData.size(),
                &m_inputLayout
                )
            );
    });

    // After the pixel shader file is loaded, create the shader and constant buffer.
    task<void> createPSTask = loadPSTask.then([this] (const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreatePixelShader(
                fileData.data(),
                fileData.size(),
                nullptr,
                &m_pixelShader
                )
            );

        const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_modelConstantBuffer
                )
            );
    });

    task<void> createGSTask;
    if (!m_usingVprtShaders)
    {
        // After the geometry shader file is loaded, create the shader.
        createGSTask = loadGSTask.then([this] (const std::vector<byte>& fileData)
        {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateGeometryShader(
                    fileData.data(),
                    fileData.size(),
                    nullptr,
                    &m_geometryShader
                    )
                );
        });
    }

    // Once all shaders are loaded, create the mesh.
    task<void> shaderTaskGroup = m_usingVprtShaders ? (createPSTask && createVSTask) : (createPSTask && createVSTask && createGSTask);
    task<void> finishLoadingTask = shaderTaskGroup.then([this] ()
    {
        CD3D11_DEPTH_STENCIL_DESC depthWriteDisableDesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
        depthWriteDisableDesc.StencilWriteMask = 0;
        depthWriteDisableDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&depthWriteDisableDesc, m_depthWriteDisable.GetAddressOf()) 
        );

        CD3D11_DEPTH_STENCIL_DESC depthDefaultDesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&depthDefaultDesc, m_depthDefault.GetAddressOf())
        );

        // Load mesh vertices. Each vertex has a position and a color.
        // Note that the quad size has changed from the default DirectX app
        // template. Windows Holographic is scaled in meters, so to draw the
        // quad at a comfortable size we made the quad width 0.2 m (20 cm).
        static const std::array<VertexPositionColor, 4> quadVertices =
        {{
            { XMFLOAT3(-0.1f,  0.1f, 0.f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3( 0.1f,  0.1f, 0.f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3( 0.1f, -0.1f, 0.f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { XMFLOAT3(-0.1f, -0.1f, 0.f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        }};

        D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
        vertexBufferData.pSysMem                = quadVertices.data();
        vertexBufferData.SysMemPitch            = 0;
        vertexBufferData.SysMemSlicePitch       = 0;
        const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionColor) * quadVertices.size(), D3D11_BIND_VERTEX_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &m_vertexBuffer)
            );

        // Load mesh indices. Each trio of indices represents
        // a triangle to be rendered on the screen.
        // For example: 2,1,0 means that the vertices with indexes
        // 2, 1, and 0 from the vertex buffer compose the
        // first triangle of this mesh.
        // Note that the winding order is clockwise by default.
        constexpr std::array<unsigned short, 12> quadIndices =
        {{
            // -z
            0,2,3,
            0,1,2,

            // +z
            2,0,3,
            1,0,2,
        }};

        m_indexCount = quadIndices.size();

        D3D11_SUBRESOURCE_DATA indexBufferData = {0};
        indexBufferData.pSysMem          = quadIndices.data();
        indexBufferData.SysMemPitch      = 0;
        indexBufferData.SysMemSlicePitch = 0;
        const CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * quadIndices.size(), D3D11_BIND_INDEX_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &m_indexBuffer
                )
            );

        // After the assets are loaded, the quad is ready to be rendered.
        m_loadingComplete = true;
    });
}

void QuadRenderer::ReleaseDeviceDependentResources()
{
    m_loadingComplete  = false;
    m_usingVprtShaders = false;

    m_vertexShader.Reset();
    m_inputLayout.Reset();
    m_pixelShader.Reset();
    m_geometryShader.Reset();

    m_modelConstantBuffer.Reset();

    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();

    m_depthDefault.Reset();
    m_depthWriteDisable.Reset();
}
