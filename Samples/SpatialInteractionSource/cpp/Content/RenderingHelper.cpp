#include "pch.h"
#include "RenderingHelper.h"
#include "Common\DirectXHelper.h"

using namespace HolographicPaint;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
RenderingHelper::RenderingHelper(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    CreateDeviceDependentResources();
}

void RenderingHelper::SetConstantBuffer(ModelConstantBuffer* pModelConstantBuffer)
{
    const auto context = m_deviceResources->GetD3DDeviceContext();
    context->UpdateSubresource(m_modelConstantBuffer.Get(), 0, nullptr, pModelConstantBuffer, 0, 0);
}

bool RenderingHelper::PrepareRendering()
{
    // Loading is asynchronous. Resources must be created before drawing can occur.
    if (!m_loadingComplete)
    {
        return false;
    }

    const auto context = m_deviceResources->GetD3DDeviceContext();

    // Each vertex is one instance of the VertexPosition struct.
    const UINT stride = sizeof(VertexPosition);
    const UINT offset = 0;
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

    // Attach the pixel shader.
    context->PSSetShader(
        m_pixelShader.Get(),
        nullptr,
        0
        );
    return true;
}

void RenderingHelper::CreateDeviceDependentResources()
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

        constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 1> vertexDesc =
        {{
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        }};

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
        // After the pass-through geometry shader file is loaded, create the shader.
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
    task<void> createCubeTask  = shaderTaskGroup.then([this] ()
    {
        // Load mesh vertices. Each vertex has a position and a color.
        // Note that the cube size has changed from the default DirectX app
        // template. Windows Holographic is scaled in meters, so to draw the
        // cube at a comfortable size we made the cube width 0.2 m (20 cm).
        static const std::array<VertexPosition, 8> cubeVertices =
        {{
            { XMFLOAT3(-0.5f, -0.5f, -0.5f) },
            { XMFLOAT3(-0.5f, -0.5f,  0.5f) },
            { XMFLOAT3(-0.5f,  0.5f, -0.5f) },
            { XMFLOAT3(-0.5f,  0.5f,  0.5f) },
            { XMFLOAT3( 0.5f, -0.5f, -0.5f) },
            { XMFLOAT3( 0.5f, -0.5f,  0.5f) },
            { XMFLOAT3( 0.5f,  0.5f, -0.5f) },
            { XMFLOAT3( 0.5f,  0.5f,  0.5f) },
        }};

        D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
        vertexBufferData.pSysMem                = cubeVertices.data();
        vertexBufferData.SysMemPitch            = 0;
        vertexBufferData.SysMemSlicePitch       = 0;
        const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPosition) * static_cast<UINT>(cubeVertices.size()), D3D11_BIND_VERTEX_BUFFER);
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
        constexpr std::array<VertexIndex_t, 36> cubeIndices =
        {{
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
        }};

        m_indexCount = static_cast<unsigned int>(cubeIndices.size());

        D3D11_SUBRESOURCE_DATA indexBufferData  = {0};
        indexBufferData.pSysMem                 = cubeIndices.data();
        indexBufferData.SysMemPitch             = 0;
        indexBufferData.SysMemSlicePitch        = 0;
        CD3D11_BUFFER_DESC indexBufferDesc(sizeof(VertexIndex_t) * static_cast<UINT>(cubeIndices.size()), D3D11_BIND_INDEX_BUFFER);
        DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &m_indexBuffer
                )
            );
    });

    // Once the cube is loaded, the object is ready to be rendered.
    createCubeTask.then([this] ()
    {
        m_loadingComplete = true;
    });
}

void RenderingHelper::DrawCube()
{
    const auto context = m_deviceResources->GetD3DDeviceContext();
    const UINT stride = sizeof(VertexPosition);
    const UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DX::GetDxgiFormat(), 0);
    context->DrawIndexedInstanced(m_indexCount, 2, 0, 0, 0);
}

void RenderingHelper::ReleaseDeviceDependentResources()
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
}

