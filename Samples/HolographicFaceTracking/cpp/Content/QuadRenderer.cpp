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
#include "QuadRenderer.h"
#include "Common\DirectXHelper.h"

using namespace HolographicFaceTracker;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the quad geometry.
QuadRenderer::QuadRenderer(std::shared_ptr<DX::DeviceResources> deviceResources) :
    DX::Resource(std::move(deviceResources))
{
}

// This function uses a SpatialPointerPose to position the world-locked hologram
// two meters in front of the user's heading.
void QuadRenderer::Update(SpatialPointerPose^ pointerPose, float3 const& offset, DX::StepTimer const& timer)
{
    float const deltaTime = static_cast<float>(timer.GetElapsedSeconds());
    float const lerpDeltaTime = deltaTime * c_lerpRate;

    if (pointerPose != nullptr)
    {
        // Get the gaze direction relative to the given coordinate system.
        float3 const headPosition = pointerPose->Head->Position;
        float3 const headForward = pointerPose->Head->ForwardDirection;
        float3 const headBack = -headForward;
        float3 const headUp = pointerPose->Head->UpDirection;
        float3 const headRight = cross(headForward, headUp);

        m_targetPosition = headPosition + (headRight * offset.x) + (headUp * offset.y) + (headBack * offset.z);

        float3 const prevPosition = m_position;
        m_position = lerp(m_position, m_targetPosition, lerpDeltaTime);

        m_velocity = (m_position - prevPosition) / deltaTime;

        m_normal = normalize(-m_position);

        m_texCoordScale = lerp(m_texCoordScale, m_targetTexCoordScale, lerpDeltaTime);
        m_texCoordOffset = lerp(m_texCoordOffset, m_targetTexCoordOffset, lerpDeltaTime);

        // Calculate our model to world matrix relative to the user's head.
        float4x4 const modelRotationTranslation = make_float4x4_world(m_position, -m_normal, headUp);

        // Scale our 1m quad down to 20cm wide.
        float4x4 const modelScale = make_float4x4_scale({ 0.2f });

        m_modelConstantBufferData.model = modelScale * modelRotationTranslation;
        m_modelConstantBufferData.texCoordScale = m_texCoordScale;
        m_modelConstantBufferData.texCoordOffset = m_texCoordOffset;

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
}

void QuadRenderer::SetTexCoordScaleAndOffset(float2 const& texCoordScale, float2 const& texCoordOffset)
{
    m_targetTexCoordScale = texCoordScale;
    m_targetTexCoordOffset = texCoordOffset;
}

void QuadRenderer::ResetTexCoordScaleAndOffset(float2 const& texCoordScale, float2 const& texCoordOffset)
{
   m_texCoordScale = m_targetTexCoordScale = texCoordScale;
   m_texCoordOffset = m_targetTexCoordOffset = texCoordOffset;
}

void QuadRenderer::RenderInternal(void)
{
    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Each vertex is one instance of the VertexPositionTex struct.
    const UINT stride = sizeof(VertexPositionTex);
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

    context->PSSetSamplers(
        0,
        1,
        m_samplerState.GetAddressOf()
    );

    // Draw the objects.
    context->DrawIndexedInstanced(
        m_indexCount,   // Index count per instance.
        2,              // Instance count.
        0,              // Start index location.
        0,              // Base vertex location.
        0               // Start instance location.
    );
}

void QuadRenderer::RenderRGB(ID3D11ShaderResourceView* rgbTexture)
{
    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Set the RGB shader
    context->PSSetShader(
        m_pixelShaderRGB.Get(),
        nullptr,
        0
    );

    // Bind the RGB texture to the shader.
    context->PSSetShaderResources(
        0,
        1,
        &rgbTexture
    );

    // Handle the rest of the rendering which is shared by RGB and NV12 rendering.
    RenderInternal();
}

void QuadRenderer::RenderNV12(ID3D11ShaderResourceView* luminanceTexture, ID3D11ShaderResourceView* chrominanceTexture)
{
    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Set the NV12 shader.
    context->PSSetShader(
        m_pixelShaderNV12.Get(),
        nullptr,
        0
    );

    // Rendering NV12 requires two resource views, which represent the luminance and chrominance channels of the YUV formatted texture.
    std::array<ID3D11ShaderResourceView*, 2> const textureViews = {
        luminanceTexture,
        chrominanceTexture
    };

    // Bind the NV12 channels to the shader.
    context->PSSetShaderResources(
        0,
        textureViews.size(),
        textureViews.data()
    );

    // Handle the rest of the rendering which is shared by RGB and NV12 rendering.
    RenderInternal();
}

task<void> QuadRenderer::CreateDeviceDependentResourcesAsync()
{
    m_usingVprtShaders = m_deviceResources->GetDeviceSupportsVprt();

    // If the optional VPRT feature is supported by the graphics device, we
    // can avoid using geometry shaders to set the render target array index.
    std::wstring vertexShaderFileName = m_usingVprtShaders ? L"ms-appx:///QuadVPRTVertexShader.cso" : L"ms-appx:///QuadVertexShader.cso";

    // Load shaders asynchronously.
    task<std::vector<byte>> loadVSTask = DX::ReadDataAsync(vertexShaderFileName);
    task<std::vector<byte>> loadPSTaskNV12 = DX::ReadDataAsync(L"ms-appx:///QuadPixelShaderNV12.cso");
    task<std::vector<byte>> loadPSTaskRGB = DX::ReadDataAsync(L"ms-appx:///QuadPixelShaderRGB.cso");
    task<std::vector<byte>> loadGSTask;

    if (!m_usingVprtShaders)
    {
        loadGSTask = DX::ReadDataAsync(L"ms-appx:///QuadGeometryShader.cso");
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

        constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> vertexDesc =
        {{
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionTex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(VertexPositionTex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    task<void> createPSTaskNV12 = loadPSTaskNV12.then([this] (const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreatePixelShader(
                fileData.data(),
                fileData.size(),
                nullptr,
                &m_pixelShaderNV12
                )
            );
    });

    task<void> createPSTaskRGB = loadPSTaskRGB.then([this] (const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreatePixelShader(
                fileData.data(),
                fileData.size(),
                nullptr,
                &m_pixelShaderRGB
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
    task<void> shaderTaskGroup = m_usingVprtShaders ? (createPSTaskNV12 && createPSTaskRGB && createVSTask) : (createPSTaskNV12 && createPSTaskRGB && createVSTask && createGSTask);
    task<void> finalLoadingTask = shaderTaskGroup.then([this] ()
    {
        // Load mesh vertices. Each vertex has a position and a color.
        // Note that the quad size has changed from the default DirectX app
        // template. Windows Holographic is scaled in meters, so to draw the
        // quad at a comfortable size we made the quad width 0.2 m (20 cm).
        static const std::array<VertexPositionTex, 4> quadVertices =
        {{
            { float3(-0.5f,  0.5f, 0.f), float2(0.f, 0.f) },
            { float3( 0.5f,  0.5f, 0.f), float2(1.f, 0.f) },
            { float3( 0.5f, -0.5f, 0.f), float2(1.f, 1.f) },
            { float3(-0.5f, -0.5f, 0.f), float2(0.f, 1.f) },
        }};

        D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
        vertexBufferData.pSysMem                = quadVertices.data();
        vertexBufferData.SysMemPitch            = 0;
        vertexBufferData.SysMemSlicePitch       = 0;
        const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionTex) * quadVertices.size(), D3D11_BIND_VERTEX_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &m_vertexBuffer
                )
            );

        // Load mesh indices. Each trio of indices represents
        // a triangle to be rendered on the screen.
        // For example: 2,1,0 means that the vertices with indexes
        // 2, 1, and 0 from the vertex buffer compose the
        // first triangle of this mesh.
        // Note that the winding order is clockwise by default.
        constexpr std::array<uint16_t, 12> quadIndices =
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

        D3D11_SAMPLER_DESC desc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateSamplerState(
                &desc,
                &m_samplerState
                )
            );

        const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(QuadModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_modelConstantBuffer
            )
        );
    });

    return finalLoadingTask;
}

void QuadRenderer::ReleaseDeviceDependentResources()
{
    m_usingVprtShaders = false;

    m_vertexShader.Reset();
    m_inputLayout.Reset();
    m_pixelShaderRGB.Reset();
    m_pixelShaderNV12.Reset();
    m_geometryShader.Reset();

    m_modelConstantBuffer.Reset();

    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();

    m_samplerState.Reset();
}
