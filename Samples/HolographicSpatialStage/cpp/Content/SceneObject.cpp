#include "pch.h"
#include "SceneObject.h"

#include "Common\DirectXHelper.h"

#include <ppltasks.h>

using namespace HolographicSpatialStage;

using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Perception::Spatial;


SceneObject::SceneObject(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    DirectX::XMFLOAT3 const& color,
    SpatialCoordinateSystem^ coordinateSystem)
    : SceneObject(deviceResources, c_cubeVertices, color, coordinateSystem)
{
}

SceneObject::SceneObject(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    std::vector<DirectX::XMFLOAT3> const& vertices,
    DirectX::XMFLOAT3 const& color,
    SpatialCoordinateSystem^ coordinateSystem)
    : SceneObject(deviceResources, vertices, c_cubeIndices, color, coordinateSystem)
{
}

SceneObject::SceneObject(
    const std::shared_ptr<DX::DeviceResources>& deviceResources,
    std::vector<DirectX::XMFLOAT3> const& vertices,
    std::vector<unsigned short> const& indices,
    DirectX::XMFLOAT3 const& color,
    Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem)
{
    m_coordinateSystem = coordinateSystem;
    m_deviceResources = deviceResources;

    m_vertices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        m_vertices[i].pos = vertices[i];
        m_vertices[i].color = color;
    }

    m_indices = indices;
    m_indexCount = unsigned int(m_indices.size());

    CreateDeviceDependentResources();
}

void SceneObject::Update(const DX::StepTimer& timer, Windows::Perception::Spatial::SpatialCoordinateSystem^ viewCoords)
{
    if (viewCoords == nullptr)
    {
        return;
    }

    // Try to get transform.
    auto transformContainer = m_coordinateSystem->TryGetTransformTo(viewCoords);
    if (m_canRenderThisFrame = (transformContainer != nullptr))
    {
        const XMMATRIX modelTransform = XMLoadFloat4x4(&transformContainer->Value);
        XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));
    }

    // Loading is asynchronous. Resources must be created before they can be updated.
    if (!m_loadingComplete || !m_canRenderThisFrame)
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

void SceneObject::Render()
{
    // Loading is asynchronous. Resources must be created before drawing can occur.
    // Our coordinate system must also be locatable.
    if (!m_loadingComplete || !m_canRenderThisFrame)
    {
        return;
    }

    const auto context = m_deviceResources->GetD3DDeviceContext();

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

    if (m_useWireframe)
    {
        // Use a wireframe rasterizer state.
        m_deviceResources->GetD3DDeviceContext()->RSSetState(m_wireframeRasterizerState.Get());
    }
    else
    {
        // Use the default rasterizer state.
        m_deviceResources->GetD3DDeviceContext()->RSSetState(m_defaultRasterizerState.Get());
    }

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

void SceneObject::CreateDeviceDependentResources()
{
    m_usingVprtShaders = m_deviceResources->GetDeviceSupportsVprt();

    // On devices that do support the D3D11_FEATURE_D3D11_OPTIONS3::
    // VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature
    // we can avoid using a pass-through geometry shader to set the render
    // target array index, thus avoiding any overhead that would be 
    // incurred by setting the geometry shader stage.
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

        constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> vertexDesc =
            { {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            } };

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateInputLayout(
                vertexDesc.data(),
                static_cast<UINT>(vertexDesc.size()),
                fileData.data(),
                static_cast<UINT>(fileData.size()),
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
        // After the pass-through geometry shader file is loaded, create the shader.
        createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData)
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
    task<void> createGeometryTask = shaderTaskGroup.then([this]()
    {
        // Load mesh vertices. Each vertex has a position and a color.
        D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
        vertexBufferData.pSysMem = m_vertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;
        const CD3D11_BUFFER_DESC vertexBufferDesc(
            sizeof(VertexPositionColor) * static_cast<UINT>(m_vertices.size()), 
            D3D11_BIND_VERTEX_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &m_vertexBuffer
            )
        );

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = m_indices.data();
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;
        CD3D11_BUFFER_DESC indexBufferDesc(
            sizeof(unsigned short) * static_cast<UINT>(m_indices.size()),
            D3D11_BIND_INDEX_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &m_indexBuffer
            )
        );
    });

    // Once the geometry is loaded, the object is ready to be rendered.
    createGeometryTask.then([this]()
    {
        // Create the default rasterizer state.
        D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
        m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, m_defaultRasterizerState.GetAddressOf());

        // Create a wireframe rasterizer state.
        rasterizerDesc.AntialiasedLineEnable = true;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
        m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, m_wireframeRasterizerState.GetAddressOf());

        m_loadingComplete = true;
    });
}

void SceneObject::ReleaseDeviceDependentResources()
{
    m_loadingComplete = false;
    m_usingVprtShaders = false;
    m_vertexShader.Reset();
    m_inputLayout.Reset();
    m_pixelShader.Reset();
    m_geometryShader.Reset();
    m_modelConstantBuffer.Reset();
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    m_defaultRasterizerState.Reset();
    m_wireframeRasterizerState.Reset();
}
