#include "pch.h"
#include "VideoRenderer.h"
#include "Common\DirectXHelper.h"
#include "Windows.Graphics.DirectX.Direct3D11.interop.h"
#include "AppView.h"

using namespace _360VideoPlayback;
using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std::placeholders;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Media::Core;
using namespace Windows::Media::Playback;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
VideoRenderer::VideoRenderer()

{
    m_deviceResources = _360VideoPlaybackMain::GetDeviceResources();
    const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &constantBufferDesc,
            nullptr,
            &m_modelConstantBuffer));
}

// Called once per frame.
void VideoRenderer::Update(const DX::StepTimer& timer)
{
    float3 scale = 10.0f;
    const XMMATRIX modelScale = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
    const XMMATRIX localTransform = modelScale;
    XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(localTransform));
}

// Renders one frame using the vertex and pixel shaders.
// On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
// a pass-through geometry shader is also used to set the render
// target array index.
void VideoRenderer::Render()
{
    critical_section::scoped_lock lock(m_critical);
    // Loading is asynchronous. Resources must be created before drawing can occur.
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


    // Each vertex is one instance of the VertexPositionColor struct.
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

    // Attach the pixel shader.
    context->PSSetShader(
        m_pixelShader.Get(),
        nullptr,
        0
    );

    // Set the Texture Shader resource and samplers
    context->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
    context->PSSetSamplers(
        0,
        1,
        m_quadTextureSamplerState.GetAddressOf()
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

void VideoRenderer::CreateDeviceDependentResources()
{
    m_usingVprtShaders = m_deviceResources->GetDeviceSupportsVprt();

    // Create the Texture, ShaderResource and Sampler state

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateTexture2D(
            &CD3D11_TEXTURE2D_DESC(
                DXGI_FORMAT_R8G8B8A8_UNORM,
                AppView::GetMediaPlayer()->PlaybackSession->NaturalVideoWidth,        // Width
                AppView::GetMediaPlayer()->PlaybackSession->NaturalVideoHeight,        // Height
                1,          // MipLevels
                1,          // ArraySize
                D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
            ),
            nullptr,
            &m_texture
        )
    );

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateShaderResourceView(
            m_texture.Get(), nullptr,
            &m_textureView
        )
    );


    D3D11_SAMPLER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));


    desc.Filter = D3D11_FILTER_ANISOTROPIC;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.MaxAnisotropy = 3;
    desc.MinLOD = 0;
    desc.MaxLOD = 3;
    desc.MipLODBias = 0.f;
    desc.BorderColor[0] = 0.f;
    desc.BorderColor[1] = 0.f;
    desc.BorderColor[2] = 0.f;
    desc.BorderColor[3] = 0.f;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateSamplerState(
            &desc,
            &m_quadTextureSamplerState
        )
    );

    CreateD3D11Surface();
    LoadShaders();
}

void VideoRenderer::LoadShaders()
{
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
                        { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
    task<void> createSphereTask = shaderTaskGroup.then([this]()
    {
        ComputeSphere(16, true);
    });

    // Once the Sphere is created, the object is ready to be rendered.
    createSphereTask.then([this]()
    {
        m_loadingComplete = true;
    });
}

void VideoRenderer::ReleaseDeviceDependentResources()
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
    if (m_videoFrameAvailToken.Value)
    {
        if (AppView::GetMediaPlayer() != nullptr)
        {
            AppView::GetMediaPlayer()->VideoFrameAvailable -= m_videoFrameAvailToken;
        }
    }
}

void VideoRenderer::ComputeSphere(unsigned short tessellation, bool invertn)
{
    if (tessellation < 3)
        throw std::out_of_range("tesselation parameter out of range");

    unsigned short verticalSegments = tessellation;
    unsigned short horizontalSegments = tessellation * 2;
    const unsigned short vertCount = (verticalSegments + 1) * (horizontalSegments + 1);
    unsigned short indexCount = (verticalSegments) * (horizontalSegments + 1) * 6;

    float radius = 0.5f; // Diameter of the default Sphere will always be 1 to stay aligned

    std::vector<VertexPositionTexture> sphereVertices;

    for (unsigned short i = 0; i <= verticalSegments; i++)
    {
        float v = 1 - (float)i / verticalSegments;

        float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
        float dy, dxz;

        XMScalarSinCos(&dy, &dxz, latitude);

        // Create a single ring of vertices at this latitude.
        for (unsigned short j = 0; j <= horizontalSegments; j++)
        {
            float u = (float)j / horizontalSegments;

            float longitude = j * XM_2PI / horizontalSegments;
            float dx, dz;

            XMScalarSinCos(&dx, &dz, longitude);

            dx *= dxz;
            dz *= dxz;

            XMVECTOR normal = XMVectorSet(dx, dy, dz, 0) * radius;
            XMFLOAT3 normalFloat3;
            XMStoreFloat3(&normalFloat3, normal);
            VertexPositionTexture vert;
            vert.position = normalFloat3;

            vert.textureCoordinate = XMFLOAT2(1.0f - u, v);

            sphereVertices.push_back(vert);
        }
    }


    std::vector<unsigned short> sphereIndices;

    // Fill the index buffer with triangles joining each pair of latitude rings.
    unsigned short stride = horizontalSegments + 1;
    for (unsigned short i = 0; i < verticalSegments; i++)
    {
        for (unsigned short j = 0; j <= horizontalSegments; j++)
        {
            unsigned short nextI = i + 1;
            unsigned short nextJ = (j + 1) % stride;
            if (!invertn)
            {
                sphereIndices.push_back(i * stride + j);
                sphereIndices.push_back(nextI * stride + j);
                sphereIndices.push_back(i * stride + nextJ);

                sphereIndices.push_back(i * stride + nextJ);
                sphereIndices.push_back(nextI * stride + j);
                sphereIndices.push_back(nextI * stride + nextJ);
            }
            else
            {
                sphereIndices.push_back(nextI * stride + j);
                sphereIndices.push_back(i * stride + j);
                sphereIndices.push_back(i * stride + nextJ);

                sphereIndices.push_back(nextI * stride + j);
                sphereIndices.push_back(i * stride + nextJ);
                sphereIndices.push_back(nextI * stride + nextJ);
            }
        }
    }

    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    vertexBufferData.pSysMem = sphereVertices.data();
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionTexture) * vertCount, D3D11_BIND_VERTEX_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            &m_vertexBuffer
        )
    );

    m_indexCount = indexCount;

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = sphereIndices.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;
    CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * indexCount, D3D11_BIND_INDEX_BUFFER);
    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            &m_indexBuffer
        )
    );
}

void VideoRenderer::CreateD3D11Surface()
{
    Microsoft::WRL::ComPtr<IDXGISurface> spDXGIInterfaceAccess;
    DX::ThrowIfFailed(m_texture->QueryInterface(IID_PPV_ARGS(&spDXGIInterfaceAccess)));
    m_surface = CreateDirect3DSurface(spDXGIInterfaceAccess.Get());
    m_videoFrameAvailToken =
        AppView::GetMediaPlayer()->VideoFrameAvailable +=
        ref new TypedEventHandler<MediaPlayer^, Platform::Object^>(
            std::bind(&VideoRenderer::OnVideoFrameAvailable, this));
    AppView::GetMediaPlayer()->Play();
}

void VideoRenderer::OnVideoFrameAvailable()
{
    critical_section::scoped_lock lock(m_critical);
    AppView::GetMediaPlayer()->CopyFrameToVideoSurface(m_surface);
}

