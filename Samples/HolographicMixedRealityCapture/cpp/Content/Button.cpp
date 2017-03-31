#include "pch.h"
#include "Button.h"
#include "Common\DirectXHelper.h"

using namespace HolographicMRCSample;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

Button::Button(const Windows::Foundation::Numerics::float3 size,
               const Windows::Foundation::Numerics::float4 color,
               const Windows::Foundation::Numerics::float4 focusColor,
               const Windows::Foundation::Numerics::float4 turnedOnColor,
               const DX::TextureIndex textureId) :
    m_size(size),
    m_color(color),
    m_colorFocus(focusColor),
    m_colorOn(turnedOnColor),
    m_textureIndex(textureId)
{
    m_type = Control_Button;
}

// Called once per frame. Rotates the cube, and calculates and sets the model matrix
// relative to the position transform indicated by hologramPositionTransform.
void Button::DoUpdate(
    const DX::StepTimer& timer,
    HolographicFrame^ holographicFrame,
    SpatialCoordinateSystem^ currentCoordinateSystem,
    SpatialInteractionSourceState^ interactionSource,
    const DirectX::XMMATRIX &parentTransform)
{
    // Rotate the cube.
    const XMMATRIX modelRotation    = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_rotation));

    // Position the cube.
    const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));

    // Multiply to get the transform matrix.
    // Note that this transform does not enforce a particular coordinate system. The calling
    // class is responsible for rendering this content in a consistent manner.
    const XMMATRIX modelTransform = modelRotation * modelTranslation * parentTransform;
    XMStoreFloat4x4(&m_transform, modelTransform);

    // The view and projection matrices are provided by the system; they are associated
    // with holographic cameras, and updated on a per-camera basis.
    // Here, we provide the model transform for the sample hologram. The model transform
    // matrix is transposed to prepare it for the shader.
    XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));

    auto pointerPose = SpatialPointerPose::TryGetAtTimestamp(currentCoordinateSystem, holographicFrame->CurrentPrediction->Timestamp);
    if (pointerPose != nullptr)
    {
        // Get the gaze direction relative to the given coordinate system.
        const float3 position = pointerPose->Head->Position;
        const float3 direction = pointerPose->Head->ForwardDirection;

        XMFLOAT3 headPosition = XMFLOAT3(position.x, position.y, position.z);
        XMFLOAT3 headDirection = XMFLOAT3(direction.x, direction.y, direction.z);
        float distance = 0.0f;

        BoundingOrientedBox currentBoundingBox;
        GetBoundingBox(currentBoundingBox);

        m_focused = currentBoundingBox.Intersects(XMLoadFloat3(&headPosition), XMLoadFloat3(&headDirection), distance);
    }

    if (m_enabled)
    {
        if (m_focused)
        {
            m_modelConstantBufferData.color.x = m_colorFocus.x;
            m_modelConstantBufferData.color.y = m_colorFocus.y;
            m_modelConstantBufferData.color.z = m_colorFocus.z;
            m_modelConstantBufferData.color.w = m_colorFocus.w;
        }
        else
        {
            if (m_turnedOn)
            {
                m_modelConstantBufferData.color.x = m_colorOn.x;
                m_modelConstantBufferData.color.y = m_colorOn.y;
                m_modelConstantBufferData.color.z = m_colorOn.z;
                m_modelConstantBufferData.color.w = m_colorOn.w;
            }
            else
            {
                m_modelConstantBufferData.color.x = m_color.x;
                m_modelConstantBufferData.color.y = m_color.y;
                m_modelConstantBufferData.color.z = m_color.z;
                m_modelConstantBufferData.color.w = m_color.w;
            }
        }
    }
    else
    {
        m_modelConstantBufferData.color.x = 0.5f;
        m_modelConstantBufferData.color.y = 0.5f;
        m_modelConstantBufferData.color.z = 0.5f;
        m_modelConstantBufferData.color.w = 1.0f;
    }

    if (interactionSource)
    {
        if (m_focused && m_enabled && interactionSource->IsPressed && m_onAirTapCallback != nullptr)
        {
            Concurrency::task<void> callbackTask = Concurrency::create_task([this]()
            {
                m_onAirTapCallback();
            });
        }
    }
}

// Renders one frame using the vertex and pixel shaders.
// On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
// a pass-through geometry shader is also used to set the render 
// target array index.
void Button::DoRender()
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
    context->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());

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

void Button::DoCreateDeviceDependentResources()
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
    m_pixelShader = m_deviceResources->GetPixelShader(DX::PixelShader_Texture);

    D3D11_TEXTURE2D_DESC texDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    ZeroMemory(&viewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Load Pressed Button texture and create resource view
    Microsoft::WRL::ComPtr<ID3D11Resource> spResource;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> spTexture;
    spResource = m_deviceResources->GetTexture(m_textureIndex);
    DX::ThrowIfFailed(spResource.As(&spTexture));

    spTexture->GetDesc(&texDesc);
    viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MostDetailedMip = 0;
    viewDesc.Texture2D.MipLevels = 1;
    DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateShaderResourceView(spResource.Get(), &viewDesc, m_textureView.ReleaseAndGetAddressOf()));

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
        float halfDepth  = m_size.z * 0.5f;
        XMFLOAT3 color(m_color.x, m_color.y, m_color.z);

        m_boundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(halfWidth, halfHeight, halfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

        // Load mesh vertices. Each vertex has a position and a color.
        const VertexPositionTexture cubeVertices[] =
        {
            { XMFLOAT3(-halfWidth, -halfHeight, -halfDepth), XMFLOAT2(0.0f, 0.0f) }, // 0
            { XMFLOAT3(-halfWidth, -halfHeight,  halfDepth), XMFLOAT2(0.0f, 1.0f) }, // 1
            { XMFLOAT3(-halfWidth,  halfHeight, -halfDepth), XMFLOAT2(0.0f, 0.0f) }, // 2
            { XMFLOAT3(-halfWidth,  halfHeight,  halfDepth), XMFLOAT2(0.0f, 0.0f) }, // 3
            { XMFLOAT3( halfWidth, -halfHeight, -halfDepth), XMFLOAT2(0.0f, 0.0f) }, // 4
            { XMFLOAT3( halfWidth, -halfHeight,  halfDepth), XMFLOAT2(1.0f, 1.0f) }, // 5
            { XMFLOAT3( halfWidth,  halfHeight, -halfDepth), XMFLOAT2(0.0f, 0.0f) }, // 6
            { XMFLOAT3( halfWidth,  halfHeight,  halfDepth), XMFLOAT2(1.0f, 0.0f) }, // 7
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
            2,1,0, // -x
            2,3,1,

            6,4,5, // +x
            6,5,7,

            0,1,5, // -y
            0,5,4,

            2,6,7, // +y
            2,7,3,

            0,4,6, // -z
            0,6,2,

            1,3,7, // +z
            1,7,5,
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

void Button::DoReleaseDeviceDependentResources()
{
    m_inputLayout.Reset();
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    m_vertexShader.Reset();
    m_geometryShader.Reset();
    m_pixelShader.Reset();
    m_modelConstantBuffer.Reset();
    m_samplerState.Reset();
    m_textureView.Reset();

    m_loadingComplete = false;
    m_usingVprtShaders = false;
}
