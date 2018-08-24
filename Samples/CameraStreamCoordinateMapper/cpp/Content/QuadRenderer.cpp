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

#include "Shaders\QuadVertexShader.h"
#include "Shaders\QuadPixelShader.h"

using namespace CameraStreamCoordinateMapper;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
QuadRenderer::QuadRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void QuadRenderer::CreateWindowSizeDependentResources()
{
    const Size outputSize = m_deviceResources->GetOutputSize();
    const float aspectRatio = outputSize.Width / outputSize.Height;

    const XMMATRIX projectionMatrix = XMMatrixOrthographicRH(2.0f, 2.0f / aspectRatio, 0.01f, 100.0f);

    XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(projectionMatrix));

    const XMMATRIX viewMatrix = XMMatrixTranslation(0.0f, 0.0f, -1.0f);

    XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(viewMatrix));
}

// Renders one frame using the vertex and pixel shaders.
void QuadRenderer::Render(ID3D11ShaderResourceView* texture, float aspectRatio)
{
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

    XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(1.0f, 1.0f / aspectRatio, 1.0f)));

    context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0);

    const UINT stride = sizeof(VertexPositionTex);
    const UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(m_inputLayout.Get());

    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

    context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    const Size outputSize = m_deviceResources->GetOutputSize();

    const D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        outputSize.Width,
        outputSize.Height
    );

    context->RSSetViewports(1, &viewport);

    ID3D11ShaderResourceView* textures[] = {
        texture
    };

    context->PSSetShaderResources(0, _countof(textures), textures);

    context->DrawIndexed(m_indexCount, 0, 0);

    ID3D11ShaderResourceView* nullTextures[] = {
        nullptr
    };

    context->PSSetShaderResources(0, _countof(nullTextures), nullTextures);
}

void QuadRenderer::CreateDeviceDependentResources()
{
    ID3D11Device* device = m_deviceResources->GetD3DDevice();

    m_vertexShader = DX::CreateVertexShader(
        device,
        g_QuadVertexShader,
        _countof(g_QuadVertexShader));

    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    m_inputLayout = DX::CreateInputLayout(
        device,
        vertexDesc,
        _countof(vertexDesc),
        g_QuadVertexShader,
        _countof(g_QuadVertexShader));

    m_pixelShader = DX::CreatePixelShader(
        device,
        g_QuadPixelShader,
        _countof(g_QuadPixelShader));

    m_constantBuffer = DX::CreateBuffer(
        device,
        sizeof(ModelViewProjectionConstantBuffer),
        nullptr,
        D3D11_BIND_CONSTANT_BUFFER);

    m_samplerState = DX::CreateSamplerState(device);

    static const VertexPositionTex quadVertices[] = 
    {
        { XMFLOAT3(-1.0f,  1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3( 1.0f,  1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3( 1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
    };

    m_vertexBuffer = DX::CreateBuffer(
        device,
        sizeof(quadVertices),
        quadVertices,
        D3D11_BIND_VERTEX_BUFFER);

    static const unsigned short quadIndices [] =
    {
        0,2,3,
        0,1,2,
    };

    m_indexCount = _countof(quadIndices);

    m_indexBuffer = DX::CreateBuffer(
        device,
        sizeof(quadIndices),
        quadIndices,
        D3D11_BIND_INDEX_BUFFER);
}

void QuadRenderer::ReleaseDeviceDependentResources()
{
    m_vertexShader = nullptr;
    m_inputLayout = nullptr;
    m_pixelShader = nullptr;
    m_constantBuffer = nullptr;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_samplerState = nullptr;
}