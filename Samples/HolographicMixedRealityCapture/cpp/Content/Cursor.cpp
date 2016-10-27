#include "pch.h"
#include "Cursor.h"
#include "Common\DirectXHelper.h"

using namespace HolographicMRCSample;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Cursor::Cursor(const float width, const float height) :
    m_size(float2(width, height)),
    m_textureIndex(DX::Texture_Cursor)
{
}

void Cursor::Initialize(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
    m_deviceResources = deviceResources;

    CreateDeviceDependentResources();
}

// Called once per frame. Rotates the cube, and calculates and sets the model matrix
// relative to the position transform indicated by hologramPositionTransform.
void Cursor::Update(
    const DX::StepTimer& timer,
    HolographicFrame^ holographicFrame,
    SpatialCoordinateSystem^ currentCoordinateSystem,
    SpatialInteractionSourceState^ interactionSource)
{
    XMMATRIX modelTransform;

    PositionCursor(SpatialPointerPose::TryGetAtTimestamp(currentCoordinateSystem,
                                                         holographicFrame->CurrentPrediction->Timestamp),
                   modelTransform);

    // The view and projection matrices are provided by the system; they are associated
    // with holographic cameras, and updated on a per-camera basis.
    // Here, we provide the model transform for the sample hologram. The model transform
    // matrix is transposed to prepare it for the shader.
    XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));
    m_modelConstantBufferData.color.x = 0.0f;
    m_modelConstantBufferData.color.y = 0.0f;
    m_modelConstantBufferData.color.z = 0.0f;
    m_modelConstantBufferData.color.w = 0.0f;
}

// Renders one frame using the vertex and pixel shaders.
// On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
// a pass-through geometry shader is also used to set the render 
// target array index.
void Cursor::Render()
{
    // Loading is asynchronous. Resources must be created before drawing can occur.
    if (!m_loadingComplete)
    {
        return;
    }

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

    // Each vertex is one instance of the VertexPositionTexture struct.
    const UINT stride = sizeof(VertexPositionTexture);
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
        // a pass-through geometry shader is used to set the render target 
        // array index.
        context->GSSetShader(
            m_geometryShader.Get(),
            nullptr,
            0
            );
    }

    // Set Texture Resource view
    context->PSSetShaderResources(0, 1, m_cursorTextureView.GetAddressOf());

    // Set Sampler
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

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
}

void Cursor::CreateDeviceDependentResources()
{
    if (m_loadingComplete)
    {
        return;
    }

    m_usingVprtShaders = m_deviceResources->GetDeviceSupportsVprt();

    // On devices that do support the D3D11_FEATURE_D3D11_OPTIONS3::
    // VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature
    // we can avoid using a pass-through geometry shader to set the render
    // target array index, thus avoiding any overhead that would be 
    // incurred by setting the geometry shader stage.

    m_vertexShader = m_deviceResources->GetVertexShader(m_usingVprtShaders ? DX::VertexShader_TextureVPRT : DX::VertexShader_Texture);
    m_inputLayout = m_deviceResources->GetInputLayout(m_usingVprtShaders ? DX::VertexShader_TextureVPRT : DX::VertexShader_Texture);
    m_pixelShader = m_deviceResources->GetPixelShader(DX::PixelShader_Cursor);

    D3D11_TEXTURE2D_DESC texDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Load Pressed Button texture and create resource view
    Microsoft::WRL::ComPtr<ID3D11Resource> spResource;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> spTexture;
    spResource = m_deviceResources->GetTexture(DX::Texture_Cursor);
    DX::ThrowIfFailed(spResource.As(&spTexture));

    spTexture->GetDesc(&texDesc);
    viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MostDetailedMip = 0;
    viewDesc.Texture2D.MipLevels = 1;
    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateShaderResourceView(spResource.Get(), &viewDesc, m_cursorTextureView.ReleaseAndGetAddressOf()));

    if (!m_usingVprtShaders)
    {
        // Get the pass-through geometry shader.
        m_geometryShader = m_deviceResources->GetGeometryShader(DX::GeometryShader_Texture);
    }

    // After the pixel shader file is loaded, create the shader and constant buffer.
    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelColorConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &constantBufferDesc,
            nullptr,
            &m_modelConstantBuffer));

    // Once all shaders are loaded, create the mesh.
    {
        float halfWidth  = m_size.x * 0.5f;
        float halfHeight = m_size.y * 0.5f;

        // Load mesh vertices. Each vertex has a position and a texture UV.
        const VertexPositionTexture cubeVertices[] =
        {
            { XMFLOAT3(-halfWidth, -halfHeight,  0.0f), XMFLOAT2(0.0f, 1.0f) }, // 0 (1)
            { XMFLOAT3(-halfWidth,  halfHeight,  0.0f), XMFLOAT2(0.0f, 0.0f) }, // 1 (3)
            { XMFLOAT3( halfWidth, -halfHeight,  0.0f), XMFLOAT2(1.0f, 1.0f) }, // 2 (5)
            { XMFLOAT3( halfWidth,  halfHeight,  0.0f), XMFLOAT2(1.0f, 0.0f) }, // 3 (7)
        };

        D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
        vertexBufferData.pSysMem = cubeVertices;
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;
        const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
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
        static const unsigned short cubeIndices [] =
        {
            0,1,3,
            0,3,2,
        };

        m_indexCount = ARRAYSIZE(cubeIndices);

        D3D11_SUBRESOURCE_DATA indexBufferData = {0};
        indexBufferData.pSysMem          = cubeIndices;
        indexBufferData.SysMemPitch      = 0;
        indexBufferData.SysMemSlicePitch = 0;
        const CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &m_indexBuffer
                )
            );
    }

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateSamplerState(
            &sampDesc,
            m_samplerState.ReleaseAndGetAddressOf()));


    // Once the cube is loaded, the object is ready to be rendered.
    m_loadingComplete = true;
}

void Cursor::ReleaseDeviceDependentResources()
{
    m_inputLayout.Reset();
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    m_vertexShader.Reset();
    m_geometryShader.Reset();
    m_pixelShader.Reset();
    m_modelConstantBuffer.Reset();
    m_samplerState.Reset();
    m_cursorTextureView.Reset();

    m_loadingComplete = false;
    m_usingVprtShaders = false;
}

void Cursor::PositionCursor(
    SpatialPointerPose^ pointerPose,
    XMMATRIX &modelRotation)
{
    if (pointerPose != nullptr)
    {
        // Get the gaze direction relative to the given coordinate system.
        const float3 headPosition  = pointerPose->Head->Position;
        const float3 headDirection = pointerPose->Head->ForwardDirection;

        const float3 gazeAtTwoMeters = headPosition + (m_distance * headDirection);

        // This will be used as the translation component of the hologram's
        // model transform.
        const float3 position = gazeAtTwoMeters;

        XMVECTOR facingNormal = XMVector3Normalize(-XMLoadFloat3(&position));
        XMVECTOR xAxisRotation = XMVector3Normalize(XMVectorSet(XMVectorGetZ(facingNormal), 0.0f, -XMVectorGetX(facingNormal), 0.0f));
        XMVECTOR yAxisRotation = XMVector3Normalize(XMVector3Cross(facingNormal, xAxisRotation));
        modelRotation = XMMATRIX(xAxisRotation,
                                 yAxisRotation,
                                 facingNormal,
                                 XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)) *
                        XMMatrixTranslationFromVector(XMLoadFloat3(&position));
    }
}
