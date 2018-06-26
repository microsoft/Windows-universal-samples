#include "pch.h"
#include "Common\DirectXHelper.h"
#include "Common\DeviceResources.h"

using namespace Microsoft::WRL;

ComPtr<ID3D11VertexShader> DX::CreateVertexShader(
    ID3D11Device* device, 
    const void* data, 
    size_t dataSize)
{
    if (dataSize > std::numeric_limits<UINT>::max())
    {
        throw std::invalid_argument("size is larger than UINT");
    }

    ComPtr<ID3D11VertexShader> result;
    DX::ThrowIfFailed(device->CreateVertexShader(
        data,
        static_cast<UINT>(dataSize),
        nullptr,
        &result));

    return result;
}

ComPtr<ID3D11PixelShader> DX::CreatePixelShader(
    ID3D11Device* device, 
    const void* data, 
    size_t dataSize)
{
    if (dataSize > std::numeric_limits<UINT>::max())
    {
        throw std::invalid_argument("size is larger than UINT");
    }

    ComPtr<ID3D11PixelShader> result;
    DX::ThrowIfFailed(device->CreatePixelShader(
        data,
        static_cast<UINT>(dataSize),
        nullptr,
        &result));

    return result;
}

ComPtr<ID3D11InputLayout> DX::CreateInputLayout(
    ID3D11Device* device, 
    const D3D11_INPUT_ELEMENT_DESC* vertexDesc, 
    size_t vertexDescCount, 
    const void* vertexShaderData, 
    size_t vertexShaderDataSize)
{
    if (vertexShaderDataSize > std::numeric_limits<UINT>::max() ||
        vertexDescCount > std::numeric_limits<UINT>::max())
    {
        throw std::invalid_argument("size is larger than UINT");
    }

    ComPtr<ID3D11InputLayout> result;
    DX::ThrowIfFailed(device->CreateInputLayout(
        vertexDesc,
        static_cast<UINT>(vertexDescCount),
        vertexShaderData,
        static_cast<UINT>(vertexShaderDataSize),
        &result));

    return result;
}

ComPtr<ID3D11SamplerState> DX::CreateSamplerState(
    ID3D11Device* device, 
    const D3D11_SAMPLER_DESC& samplerDesc)
{
    ComPtr<ID3D11SamplerState> result;

    DX::ThrowIfFailed(device->CreateSamplerState(
        &samplerDesc,
        &result));

    return result;
}

ComPtr<ID3D11Texture2D> DX::CreateTexture(
    ID3D11Device* device, 
    UINT width, 
    UINT height, 
    DXGI_FORMAT format, 
    std::optional<DX::TextureData> textureData, 
    std::optional<UINT> bindFlags, 
    std::optional<D3D11_USAGE> usage, 
    std::optional<UINT> cpuFlags)
{
    const CD3D11_TEXTURE2D_DESC desc{
        format,
        width,
        height,
        1u,
        1u,
        bindFlags.value_or(D3D11_BIND_SHADER_RESOURCE),
        usage.value_or(D3D11_USAGE_DEFAULT),
        cpuFlags.value_or(0x0)
    };

    D3D11_SUBRESOURCE_DATA data = {};

    if (textureData)
    {
        data.pSysMem = textureData->buffer;
        data.SysMemPitch = textureData->bytesPerPixel * width;
    }

    const D3D11_SUBRESOURCE_DATA* pData = (textureData) ? &data : nullptr;

    ComPtr<ID3D11Texture2D> result;
    DX::ThrowIfFailed(device->CreateTexture2D(&desc, pData, &result));
    return result;
}

ComPtr<ID3D11Buffer> DX::CreateBuffer(
    ID3D11Device* device,
    const size_t size,
    const void* buffer,
    std::optional<UINT> bindFlags,
    std::optional<D3D11_USAGE> usage,
    std::optional<UINT> cpuFlags)
{
    if (size > std::numeric_limits<UINT>::max())
    {
        throw std::invalid_argument("size is larger than UINT");
    }

    const CD3D11_BUFFER_DESC desc{
        static_cast<UINT>(size),
        bindFlags.value_or(D3D11_BIND_CONSTANT_BUFFER),
        usage.value_or(D3D11_USAGE_DEFAULT),
        cpuFlags.value_or(0x0),
    };

    const D3D11_SUBRESOURCE_DATA data{
        buffer,
        0u,
        0u
    };

    const D3D11_SUBRESOURCE_DATA* pData = (buffer) ? &data : nullptr;

    ComPtr<ID3D11Buffer> result;
    DX::ThrowIfFailed(device->CreateBuffer(&desc, pData, &result));
    return result;
}

ComPtr<ID3D11ShaderResourceView> DX::CreateShaderResourceView(
    ID3D11Texture2D* texture,
    std::optional<DXGI_FORMAT> format)
{
    ComPtr<ID3D11Device> device;
    texture->GetDevice(&device);

    const CD3D11_SHADER_RESOURCE_VIEW_DESC desc{ 
        texture, 
        D3D11_SRV_DIMENSION_TEXTURE2D,
        format.value_or(DXGI_FORMAT_UNKNOWN)
    };

    ComPtr<ID3D11ShaderResourceView> result;
    DX::ThrowIfFailed(device->CreateShaderResourceView(texture, &desc, &result));
    return result;
}

ComPtr<ID3D11RenderTargetView> DX::CreateRenderTargetView(
    ID3D11Texture2D* texture)
{
    ComPtr<ID3D11Device> device;
    texture->GetDevice(&device);

    const CD3D11_RENDER_TARGET_VIEW_DESC desc{ 
        texture, 
        D3D11_RTV_DIMENSION_TEXTURE2D 
    };

    ComPtr<ID3D11RenderTargetView> result;
    DX::ThrowIfFailed(device->CreateRenderTargetView(texture, &desc, &result));
    return result;
}

ComPtr<ID3D11DepthStencilView> DX::CreateDepthStencilView(
    ID3D11Texture2D* texture,
    std::optional<DXGI_FORMAT> format)
{
    ComPtr<ID3D11Device> device;
    texture->GetDevice(&device);

    const CD3D11_DEPTH_STENCIL_VIEW_DESC desc{ 
        texture, 
        D3D11_DSV_DIMENSION_TEXTURE2D,
        format.value_or(DXGI_FORMAT_UNKNOWN)
    };

    ComPtr<ID3D11DepthStencilView> result;
    DX::ThrowIfFailed(device->CreateDepthStencilView(texture, &desc, &result));
    return result;
}

DX::GpuPerformanceTimer::GpuPerformanceTimer(
    const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    ID3D11Device* device = m_deviceResources->GetD3DDevice();

    const D3D11_QUERY_DESC timestampQuery = CD3D11_QUERY_DESC(D3D11_QUERY_TIMESTAMP);
    const D3D11_QUERY_DESC disjointQuery = CD3D11_QUERY_DESC(D3D11_QUERY_TIMESTAMP_DISJOINT);

    for (uint32_t i = 0; i < QueryCount; ++i)
    {
        device->CreateQuery(&timestampQuery, m_startTimestampQuery[i].ReleaseAndGetAddressOf());
        device->CreateQuery(&timestampQuery, m_endTimestampQuery[i].ReleaseAndGetAddressOf());
        device->CreateQuery(&disjointQuery, m_disjointQuery[i].ReleaseAndGetAddressOf());
    }
}

void DX::GpuPerformanceTimer::StartTimerForFrame()
{
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

    // Just some book-keeping to see how long this algorithm is running 
    m_currentQuery = (m_currentQuery + 1) % QueryCount;
    const uint32_t ReadQuery = (m_currentQuery + 1) % QueryCount;

    if (m_currentFrameIndex >= QueryCount)
    {
        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
        while (context->GetData(m_disjointQuery[ReadQuery].Get(), &disjointData, sizeof(disjointData), 0) != S_OK);

        if (disjointData.Disjoint == 0)
        {
            uint64_t start;
            while (context->GetData(m_startTimestampQuery[ReadQuery].Get(), &start, sizeof(start), 0) != S_OK);

            uint64_t end;
            while (context->GetData(m_endTimestampQuery[ReadQuery].Get(), &end, sizeof(end), 0) != S_OK);

            constexpr float secondsToMilliseconds = 1000.0f;
            const float ticksToSeconds = 1.0f / static_cast<float>(disjointData.Frequency);
            const uint64_t ticksElapsed = end - start;

            m_processingTimeMs = static_cast<float>(ticksElapsed) * ticksToSeconds * secondsToMilliseconds;

            // Reset every ~10 seconds so the min/max has a chance to update
            if (m_currentFrameIndex % 300 == 0)
            {
                m_processingTimeMinMs = +std::numeric_limits<float>::infinity();
                m_processingTimeMaxMs = -std::numeric_limits<float>::infinity();
            }

            m_processingTimeMinMs = std::min(m_processingTimeMinMs, m_processingTimeMs);
            m_processingTimeMaxMs = std::max(m_processingTimeMaxMs, m_processingTimeMs);
        }
        else
        {
            m_processingTimeMs = std::numeric_limits<float>::quiet_NaN();;
        }

        m_processingTimeHistory[m_processingTimeHistoryIndex] = m_processingTimeMs;
        m_processingTimeHistoryIndex = (m_processingTimeHistoryIndex + 1) % TimeHistoryCount;

        m_processingTimeAvgMs = std::accumulate(m_processingTimeHistory, m_processingTimeHistory + TimeHistoryCount, 0.0f) / static_cast<float>(TimeHistoryCount);
    }

    ++m_currentFrameIndex;

    context->Begin(m_disjointQuery[m_currentQuery].Get());
    context->End(m_startTimestampQuery[m_currentQuery].Get());
}

void DX::GpuPerformanceTimer::EndTimerForFrame()
{
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();
    context->End(m_endTimestampQuery[m_currentQuery].Get());
    context->End(m_disjointQuery[m_currentQuery].Get());
}

float DX::GpuPerformanceTimer::GetFrameTime() const
{
    return m_processingTimeMs;
}

float DX::GpuPerformanceTimer::GetAvgFrameTime() const
{
    return m_processingTimeAvgMs;
}

float DX::GpuPerformanceTimer::GetMinFrameTime() const
{
    return m_processingTimeMinMs;
}

float DX::GpuPerformanceTimer::GetMaxFrameTime() const
{
    return m_processingTimeMaxMs;
}
