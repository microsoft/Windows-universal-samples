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

#include "Common\DirectXHelper.h"
#include "Content\ShaderStructures.h"
#include "DistanceFieldRenderer.h"

using namespace Concurrency;
using namespace DirectX;
using namespace HolographicTagAlongSample;

DistanceFieldRenderer::DistanceFieldRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, unsigned int const& textureWidth, unsigned int const& textureHeight)
  : m_deviceResources(deviceResources),
    m_textureWidth(textureWidth),
    m_textureHeight(textureHeight)
{
    CreateDeviceDependentResources();
}

void DistanceFieldRenderer::RenderDistanceField(ID3D11ShaderResourceView* texture)
{
    // Loading is asynchronous. Resources must be created before drawing can occur.
    if (!m_loadingComplete)
    {
        return;
    }

    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Set and clear the off-screen render target.    
    ID3D11RenderTargetView *const targets[1] = { m_renderTargetView.Get() };
    context->OMSetRenderTargets(1, targets, m_d3dDepthStencilView.Get());
    context->ClearRenderTargetView(targets[0], DirectX::Colors::Transparent);
    context->ClearDepthStencilView(m_d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Set the viewport to cover the whole texture.
    CD3D11_VIEWPORT viewport = CD3D11_VIEWPORT(
        0.f, 0.f,
        static_cast<float>(m_textureWidth),
        static_cast<float>(m_textureHeight));
    context->RSSetViewports(1, &viewport);

    // Each vertex is one instance of the XMFLOAT2 struct.
    const UINT stride = m_vertexStride;
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

    // Attach the pixel shader.
    context->PSSetShader(
        m_pixelShader.Get(),
        nullptr,
        0
    );
    context->PSSetShaderResources(
        0,
        1,
        &texture
    );

    // Draw the objects.
    context->DrawIndexed(
        m_indexCount,   // Index count.
        0,              // Start index location.
        0               // Base vertex location.
    );

    ++m_renderCount;
}

void DistanceFieldRenderer::ReleaseDeviceDependentResources()
{
    m_loadingComplete = false;

    m_vertexShader.Reset();
    m_inputLayout.Reset();
    m_pixelShader.Reset();

    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();

    m_texture.Reset();
    m_shaderResourceView.Reset();
    m_renderTargetView.Reset();
}

void DistanceFieldRenderer::CreateDeviceDependentResources()
{
    // Create the texture that will be used as the offscreen render target.
    CD3D11_TEXTURE2D_DESC textureDesc(
        DXGI_FORMAT_R8G8_UNORM,
        m_textureWidth,
        m_textureHeight,
        1,
        1,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
    );
    m_deviceResources->GetD3DDevice()->CreateTexture2D(&textureDesc, nullptr, &m_texture);

    // Create read and write views for the offscreen render target.
    m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_texture.Get(), nullptr, &m_shaderResourceView);
    m_deviceResources->GetD3DDevice()->CreateRenderTargetView(m_texture.Get(), nullptr, &m_renderTargetView);


    // Create a depth stencil view.
    // NOTE: This is used only for drawing. It could be recycled for additional distance rendering 
    //       passes, and discarded after all distance fields have been created (as could the index 
    //       and vertex buffers).
    CD3D11_TEXTURE2D_DESC depthStencilDesc(
        DXGI_FORMAT_D16_UNORM,
        m_textureWidth,
        m_textureHeight,
        1, // One array level.
        1, // Use a single mipmap level.
        D3D11_BIND_DEPTH_STENCIL
    );

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            &depthStencil
        )
    );

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateDepthStencilView(
            depthStencil.Get(),
            &depthStencilViewDesc,
            &m_d3dDepthStencilView
        )
    );


    // Load shaders asynchronously.
    task<std::vector<byte>> loadVSTask = DX::ReadDataAsync(L"ms-appx:///FullscreenQuadVertexShader.cso");
    task<std::vector<byte>> loadPSTask = DX::ReadDataAsync(L"ms-appx:///CreateDistanceFieldPixelShader.cso");

    // After the vertex shader file is loaded, create the shader and input layout.
    task<void> createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateVertexShader(
                fileData.data(),
                fileData.size(),
                nullptr,
                &m_vertexShader
            )
        );

        constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 1> vertexDesc =
            {{
                { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
    task<void> createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
    {
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreatePixelShader(
                fileData.data(),
                fileData.size(),
                nullptr,
                &m_pixelShader
            )
        );
    });

    // Once all shaders are loaded, create the mesh.
    task<void> createQuadTask = (createPSTask && createVSTask).then([this]()
    {
        // Load mesh vertices for a full-screen quad.
        static const std::array<XMFLOAT2, 4> quadVertices =
        {{
            { XMFLOAT2(-1.0f,  1.0f) },
            { XMFLOAT2( 1.0f,  1.0f) },
            { XMFLOAT2( 1.0f, -1.0f) },
            { XMFLOAT2(-1.0f, -1.0f) },
        }};

        m_vertexStride = sizeof(XMFLOAT2);

        D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
        vertexBufferData.pSysMem = quadVertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;
        const CD3D11_BUFFER_DESC vertexBufferDesc(m_vertexStride * quadVertices.size(), D3D11_BIND_VERTEX_BUFFER);
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
        constexpr std::array<unsigned short, 6> quadIndices =
        {{
            // -z
            0,2,3,
            0,1,2,
        }};

        m_indexCount = quadIndices.size();

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = quadIndices.data();
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;
        const CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * quadIndices.size(), D3D11_BIND_INDEX_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &m_indexBuffer
            )
        );
    });

    auto loadingCompleteTask = createQuadTask.then([this]() {
        
        m_loadingComplete = true;
    });
}

