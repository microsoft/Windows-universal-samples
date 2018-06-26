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

#pragma once

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch Win32 API errors.
            throw Platform::Exception::CreateException(hr);
        }
    }

    // Function that reads from a binary file asynchronously.
    inline Concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
    {
        using namespace Windows::Storage;
        using namespace Concurrency;

        Windows::Storage::StorageFolder^ folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

        return create_task(folder->GetFileAsync(Platform::StringReference(filename.c_str()))).then([] (StorageFile^ file) 
        {
            return FileIO::ReadBufferAsync(file);
        }).then([] (Streams::IBuffer^ fileBuffer) -> std::vector<byte> 
        {
            std::vector<byte> returnBuffer;
            returnBuffer.resize(fileBuffer->Length);
            Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), fileBuffer->Length));
            return returnBuffer;
        });
    }

    // Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
    inline float ConvertDipsToPixels(float dips, float dpi)
    {
        static const float dipsPerInch = 96.0f;
        return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
    }

#if defined(_DEBUG)
    // Check for SDK Layer support.
    inline bool SdkLayersAvailable()
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
            0,
            D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
            nullptr,                    // Any feature level will do.
            0,
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            nullptr,                    // No need to keep the D3D device reference.
            nullptr,                    // No need to know the feature level.
            nullptr                     // No need to keep the D3D device context reference.
            );

        return SUCCEEDED(hr);
    }
#endif

    struct TextureData
    {
        const void* buffer;
        const UINT bytesPerPixel;
    };

    Microsoft::WRL::ComPtr<ID3D11VertexShader> CreateVertexShader(
        ID3D11Device* device,
        const void* data,
        size_t dataSize);

    Microsoft::WRL::ComPtr<ID3D11PixelShader> CreatePixelShader(
        ID3D11Device* device,
        const void* data,
        size_t dataSize);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> CreateInputLayout(
        ID3D11Device* device,
        const D3D11_INPUT_ELEMENT_DESC* vertexDesc,
        size_t vertexDescCount,
        const void* vertexShaderData,
        size_t vertexShaderDataSize);

    Microsoft::WRL::ComPtr<ID3D11SamplerState> CreateSamplerState(
        ID3D11Device* device,
        const D3D11_SAMPLER_DESC& samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} });

    Microsoft::WRL::ComPtr<ID3D11Texture2D> CreateTexture(
        ID3D11Device* device,
        UINT width,
        UINT height,
        DXGI_FORMAT format,
        std::optional<TextureData> textureData = std::nullopt,
        std::optional<UINT> bindFlags = std::nullopt,
        std::optional<D3D11_USAGE> usage = std::nullopt,
        std::optional<UINT> cpuFlags = std::nullopt);

    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateBuffer(
        ID3D11Device* device,
        const size_t size,
        const void* buffer = nullptr,
        std::optional<UINT> bindFlags = std::nullopt,
        std::optional<D3D11_USAGE> usage = std::nullopt,
        std::optional<UINT> cpuFlags = std::nullopt);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateShaderResourceView(
        ID3D11Texture2D* texture,
        std::optional<DXGI_FORMAT> format = std::nullopt);

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> CreateRenderTargetView(
        ID3D11Texture2D* texture);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> CreateDepthStencilView(
        ID3D11Texture2D* texture,
        std::optional<DXGI_FORMAT> format = std::nullopt);

    class DeviceResources;

    // Used to track time spent executing Gpu work
    class GpuPerformanceTimer
    {
    public:
        GpuPerformanceTimer(
            const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void StartTimerForFrame();
        void EndTimerForFrame();

        float GetFrameTime() const;
        float GetAvgFrameTime() const;
        float GetMinFrameTime() const;
        float GetMaxFrameTime() const;

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        static constexpr uint32_t QueryCount = 5u;

        Microsoft::WRL::ComPtr<ID3D11Query> m_startTimestampQuery[QueryCount];
        Microsoft::WRL::ComPtr<ID3D11Query> m_endTimestampQuery[QueryCount];
        Microsoft::WRL::ComPtr<ID3D11Query> m_disjointQuery[QueryCount];

        uint32_t m_currentQuery = 0u;
        uint32_t m_currentFrameIndex = 0u;

        static constexpr uint32_t TimeHistoryCount = 64u;

        uint32_t m_processingTimeHistoryIndex = 0u;
        float m_processingTimeHistory[TimeHistoryCount] = {};

        float m_processingTimeMs = 0.0f;
        float m_processingTimeAvgMs = 0.0f;
        float m_processingTimeMinMs = +std::numeric_limits<float>::infinity();
        float m_processingTimeMaxMs = -std::numeric_limits<float>::infinity();
    };
}
