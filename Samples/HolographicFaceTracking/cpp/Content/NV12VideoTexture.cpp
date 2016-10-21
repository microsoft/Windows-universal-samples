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
#include "NV12VideoTexture.h"
#include "Common\DirectXHelper.h"

#include <MemoryBuffer.h> // IMemoryBufferByteAccess

using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

using namespace HolographicFaceTracker;

NV12VideoTexture::NV12VideoTexture(std::shared_ptr<DX::DeviceResources> deviceResources, uint32_t width, uint32_t height)
    : DX::Resource(std::move(deviceResources))
    , m_width(width)
    , m_height(height)
{
}

// Opens up the underlying buffer of a SoftwareBitmap and copies to our D3D11 Texture
void NV12VideoTexture::CopyFromVideoMediaFrame(Windows::Media::Capture::Frames::VideoMediaFrame^ source)
{
    SoftwareBitmap^ softwareBitmap = source->SoftwareBitmap;

    if (softwareBitmap == nullptr)
    {
        OutputDebugString(L"SoftwareBitmap was null\r\n");
        return;
    }

    if (softwareBitmap->BitmapPixelFormat != BitmapPixelFormat::Nv12)
    {
        OutputDebugString(L"BitmapPixelFormat was not NV12\r\n");
        return;
    }

    BitmapBuffer^ bitmapBuffer = softwareBitmap->LockBuffer(BitmapBufferAccessMode::Read);
    IMemoryBufferReference^ bufferRef = bitmapBuffer->CreateReference();

    ComPtr<IMemoryBufferByteAccess> memoryBufferByteAccess;
    if (SUCCEEDED(reinterpret_cast<IInspectable*>(bufferRef)->QueryInterface(IID_PPV_ARGS(&memoryBufferByteAccess))))
    {
        BYTE* pSourceBuffer = nullptr;
        UINT32 sourceCapacity = 0;
        if (SUCCEEDED(memoryBufferByteAccess->GetBuffer(&pSourceBuffer, &sourceCapacity)) && pSourceBuffer)
        {
            auto const context = m_deviceResources->GetD3DDeviceContext();

            // Copy the new video frame over.
            D3D11_MAPPED_SUBRESOURCE subResource;
            if (SUCCEEDED(context->Map(m_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource)))
            {
                std::memcpy(subResource.pData, pSourceBuffer, sourceCapacity);
                context->Unmap(m_texture.Get(), 0);
            }
        }
    }
}

concurrency::task<void> NV12VideoTexture::CreateDeviceDependentResourcesAsync()
{
    return concurrency::create_task([this]
    {
        D3D11_TEXTURE2D_DESC const texDesc = CD3D11_TEXTURE2D_DESC(
            DXGI_FORMAT_NV12,           // HoloLens PV camera format, common for video sources
            m_width,                    // Width of the video frames
            m_height,                   // Height of the video frames
            1,                          // Number of textures in the array
            1,                          // Number of miplevels in each texture
            D3D11_BIND_SHADER_RESOURCE, // We read from this texture in the shader
            D3D11_USAGE_DYNAMIC,        // Because we'll be copying from CPU memory
            D3D11_CPU_ACCESS_WRITE      // We only need to write into the texture
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateTexture2D(
                &texDesc,
                nullptr,
                &m_texture
            )
        );

        // https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
        // To access DXGI_FORMAT_NV12 in the shader, we need to map the luminance channel and the chrominance channels
        // into a format that shaders can understand.
        // In the case of NV12, DirectX understands how the texture is laid out, so we can create these
        // shader resource views which represent the two channels of the NV12 texture.
        // Then inside the shader we convert YUV into RGB so we can render.

        // DirectX specifies the view format to be DXGI_FORMAT_R8_UNORM for NV12 luminance channel.
        // Luminance is 8 bits per pixel. DirectX will handle converting 8-bit integers into normalized
        // floats for use in the shader.
        D3D11_SHADER_RESOURCE_VIEW_DESC const luminancePlaneDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(
            m_texture.Get(),
            D3D11_SRV_DIMENSION_TEXTURE2D,
            DXGI_FORMAT_R8_UNORM
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(
                m_texture.Get(),
                &luminancePlaneDesc,
                &m_luminanceView
            )
        );

        // DirectX specifies the view format to be DXGI_FORMAT_R8G8_UNORM for NV12 chrominance channel.
        // Chrominance has 4 bits for U and 4 bits for V per pixel. DirectX will handle converting 4-bit
        // integers into normalized floats for use in the shader.
        D3D11_SHADER_RESOURCE_VIEW_DESC const chrominancePlaneDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(
            m_texture.Get(),
            D3D11_SRV_DIMENSION_TEXTURE2D,
            DXGI_FORMAT_R8G8_UNORM
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(
                m_texture.Get(),
                &chrominancePlaneDesc,
                &m_chrominanceView
            )
        );
    });
}

void NV12VideoTexture::ReleaseDeviceDependentResources()
{
    m_chrominanceView.Reset();
    m_luminanceView.Reset();
    m_texture.Reset();
}
