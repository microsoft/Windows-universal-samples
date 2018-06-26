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

#include "CameraCoordinateMapper.h"
#include "CameraStreamProcessor.h"
#include "GpuCoordinateMapper.h"

#include <MemoryBuffer.h>

using namespace CameraStreamCoordinateMapper;
using namespace concurrency;

using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Media::Devices::Core;
using namespace Windows::Media::MediaProperties;

using namespace Microsoft::WRL;

namespace
{
    template<typename Func>
    void AccessSoftwareBitmap(SoftwareBitmap^ bitmap, Func&& func)
    {
        if (BitmapBuffer^ bitmapBuffer = bitmap->LockBuffer(BitmapBufferAccessMode::Read))
        {
            if (IMemoryBufferReference^ bitmapRef = bitmapBuffer->CreateReference())
            {
                ComPtr<IMemoryBufferByteAccess> memoryBufferByteAccess;
                const HRESULT hr = reinterpret_cast<IInspectable*>(bitmapRef)->QueryInterface(IID_PPV_ARGS(&memoryBufferByteAccess));

                if (SUCCEEDED(hr) && memoryBufferByteAccess)
                {
                    byte* pSourceBuffer;
                    UINT32 capacity;
                    if (SUCCEEDED(memoryBufferByteAccess->GetBuffer(&pSourceBuffer, &capacity)) && pSourceBuffer)
                    {
                        func(pSourceBuffer, capacity);
                    }
                }
            }
        }
    }

    void CopySoftwareBitmapToDirect3DTexture(SoftwareBitmap^ softwareBitmap, ID3D11DeviceContext* context, ID3D11Texture2D* texture)
    {
        AccessSoftwareBitmap(softwareBitmap, [&](byte* buffer, UINT32 capacity)
        {
            D3D11_MAPPED_SUBRESOURCE subresource;
            if (SUCCEEDED(context->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource)))
            {
                std::memcpy(subresource.pData, buffer, capacity);
                context->Unmap(texture, 0);
            }
        });
    };

    ////////////////////////////////////////////////////////////////////////////////
    // This is used to generate look-up tables based on the camera intrinsics
    // 
    // For every point on the CameraIntrinsics from [0, imageWidth] and [0, imageHeight] 
    // "func" will be called, and the result stored in the corresponding location in the
    // gpu texture. This allows us to utilize the camera intrinsics in shaders. 
    template<typename Func>
    ComPtr<ID3D11Texture2D> GenerateIntrinsicsMapping(ID3D11Device* device, CameraIntrinsics^ cameraIntrinsics, Func&& func)
    {
        using OutputType = decltype(func(cameraIntrinsics, Point{}));
        static_assert(sizeof(OutputType) == sizeof(float2), "Func should return the equivalent of 2 floats");

        const uint32_t imageWidth = cameraIntrinsics->ImageWidth;
        const uint32_t imageHeight = cameraIntrinsics->ImageHeight;

        const size_t numElements = imageWidth * imageHeight;

        auto spMap = std::make_unique<OutputType[]>(numElements);

        for (uint32_t y = 0; y < imageHeight; ++y)
        {
            for (uint32_t x = 0; x < imageWidth; ++x)
            {
                spMap[y * imageWidth + x] = func(cameraIntrinsics, Point(float(x), float(y)));
            }
        }

        DX::TextureData data{ spMap.get(), sizeof(OutputType) };
        return DX::CreateTexture(
            device,
            imageWidth,
            imageHeight,
            DXGI_FORMAT_R32G32_FLOAT,
            data,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_USAGE_IMMUTABLE);
    }

    bool IsValidColorFrame(MediaFrameReference^ frame)
    {
        // These properties are required for our coordinate mapping algorithm
        return frame &&
               frame->CoordinateSystem &&
               frame->VideoMediaFrame &&
               frame->VideoMediaFrame->CameraIntrinsics &&
               frame->VideoMediaFrame->SoftwareBitmap;
    }

    bool IsValidDepthFrame(MediaFrameReference^ frame)
    {
        // A valid depth frame has the same properties of a color frame, as well as a valid DepthMediaFrame property
        return IsValidColorFrame(frame) && frame->VideoMediaFrame->DepthMediaFrame;
    }
}

task<std::shared_ptr<CameraCoordinateMapper>> CameraCoordinateMapper::CreateAndStartAsync(std::shared_ptr<DX::DeviceResources> deviceResources)
{
    // Future work: Support more color formats
    std::shared_ptr<CameraStreamProcessor> colorStreamProcessor;
    std::shared_ptr<CameraStreamProcessor> depthStreamProcessor;

    // Creating the camera streams is not required to succeed, if there are failures we display an error message to the user
    try
    {
        colorStreamProcessor = co_await CameraStreamProcessor::CreateAndStartAsync(MediaFrameSourceKind::Color, MediaEncodingSubtypes::Bgra8);
        depthStreamProcessor = co_await CameraStreamProcessor::CreateAndStartAsync(MediaFrameSourceKind::Depth, MediaEncodingSubtypes::D16);
    }
    catch (Platform::Exception^ exception)
    {
        OutputDebugString(exception->Message->Data());
    }

    return std::make_shared<CameraCoordinateMapper>(
        std::move(deviceResources),
        std::move(colorStreamProcessor),
        std::move(depthStreamProcessor));
}

CameraCoordinateMapper::CameraCoordinateMapper(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<CameraStreamProcessor> colorStreamProcessor, std::shared_ptr<CameraStreamProcessor> depthStreamProcessor) :
    m_deviceResources(std::move(deviceResources)),
    m_colorCameraStreamProcessor(std::move(colorStreamProcessor)),
    m_depthCameraStreamProcessor(std::move(depthStreamProcessor))
{
    m_gpuCoordinateMapper = std::make_unique<GpuCoordinateMapper>(m_deviceResources);
}

void CameraCoordinateMapper::CreateDeviceDependentResources()
{
    m_gpuCoordinateMapper->CreateDeviceDependentResources();
}

void CameraCoordinateMapper::ReleaseDeviceDependentResources()
{
    m_depthTexture = nullptr;
    m_depthTextureSRV = nullptr;

    m_colorTexture = nullptr;
    m_colorTextureSRV = nullptr;

    m_targetTexture = nullptr;
    m_targetTextureSRV = nullptr;
    m_targetTextureRTV = nullptr;

    m_targetRasterizedDepthTexture = nullptr;
    m_targetRasterizedDepthTextureDSV = nullptr;
    m_targetRasterizedDepthTextureSRV = nullptr;

    m_depthUnprojectionMapTexture = nullptr;
    m_depthUnprojectionMapTextureSRV = nullptr;

    m_colorDistortionMapTexture = nullptr;
    m_colorDistortionMapTextureSRV = nullptr;

    m_rasterizeFrameBlob = {};
    m_visualizeFrameBlob = {};

    m_depthIntrinsics = {};
    m_colorIntrinsics = {};

    m_depthFrameIndex = {};
    m_colorFrameIndex = {};

    m_gpuCoordinateMapper->ReleaseDeviceDependentResources();
}

task<void> CameraCoordinateMapper::StartAsync()
{
    if (m_colorCameraStreamProcessor)
    {
        co_await m_colorCameraStreamProcessor->StartAsync();
    }

    if (m_depthCameraStreamProcessor)
    {
        co_await m_depthCameraStreamProcessor->StartAsync();
    }
}

task<void> CameraCoordinateMapper::StopAsync()
{
    if (m_colorCameraStreamProcessor)
    {
        co_await m_colorCameraStreamProcessor->StopAsync();
    }

    if (m_depthCameraStreamProcessor)
    {
        co_await m_depthCameraStreamProcessor->StopAsync();
    }
}

bool CameraCoordinateMapper::TryAcquireLatestFrameData() try {

    if (!m_colorCameraStreamProcessor || !m_depthCameraStreamProcessor) 
    {
        return false;
    }

    CameraStreamProcessor::FrameData colorFrameBlob = m_colorCameraStreamProcessor->GetLatestFrame();
    CameraStreamProcessor::FrameData depthFrameBlob = m_depthCameraStreamProcessor->GetLatestFrame();

    MediaFrameReference^ colorFrame = colorFrameBlob.frame;
    MediaFrameReference^ depthFrame = depthFrameBlob.frame;

    // Invalid color or depth frame
    if (!IsValidColorFrame(colorFrame) || !IsValidDepthFrame(depthFrame)) 
    {
        return false;
    }

    const bool sameColorFrame = m_colorFrameIndex.has_value() && colorFrameBlob.index == m_colorFrameIndex.value();
    const bool sameDepthFrame = m_depthFrameIndex.has_value() && depthFrameBlob.index == m_depthFrameIndex.value();

    // Already processed this frame
    if (sameColorFrame && sameDepthFrame) 
    {
        return false;
    }

    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

    if (!sameColorFrame)
    {
        UpdateColorResources(colorFrame);
        CopySoftwareBitmapToDirect3DTexture(colorFrame->VideoMediaFrame->SoftwareBitmap, context, m_colorTexture.Get());

        m_rasterizeFrameBlob.sourceIntrinsics = m_colorIntrinsics;
        m_rasterizeFrameBlob.targetIntrinsics = m_colorIntrinsics;

        const float4x4 colorProjectionMatrix = colorFrame->VideoMediaFrame->CameraIntrinsics->UndistortedProjectionTransform;

        m_rasterizeFrameBlob.sourceProjectionMatrix = colorProjectionMatrix;
        m_rasterizeFrameBlob.targetProjectionMatrix = colorProjectionMatrix;
        m_rasterizeFrameBlob.sourceImage = m_colorTextureSRV;
        m_rasterizeFrameBlob.sourceDistortionMap = m_colorDistortionMapTextureSRV;
        m_rasterizeFrameBlob.targetImage = m_targetTextureRTV;
        m_rasterizeFrameBlob.targetRasterizedDepth = m_targetRasterizedDepthTextureDSV;
        m_rasterizeFrameBlob.targetDistortionMap = m_colorDistortionMapTextureSRV;

        m_colorFrameIndex = colorFrameBlob.index;
    }

    if (!sameDepthFrame)
    {
        UpdateDepthResources(depthFrame);
        CopySoftwareBitmapToDirect3DTexture(depthFrame->VideoMediaFrame->SoftwareBitmap, context, m_depthTexture.Get());

        const float depthScaleInMeters = static_cast<float>(depthFrame->VideoMediaFrame->DepthMediaFrame->DepthFormat->DepthScaleInMeters);

        m_rasterizeFrameBlob.depthIntrinsics = m_depthIntrinsics;
        m_rasterizeFrameBlob.depthScaleInMeters = depthScaleInMeters;
        m_rasterizeFrameBlob.depthRangeMinimumInMeters = static_cast<float>(depthFrame->VideoMediaFrame->DepthMediaFrame->MinReliableDepth) * depthScaleInMeters;
        m_rasterizeFrameBlob.depthRangeMaximumInMeters = static_cast<float>(depthFrame->VideoMediaFrame->DepthMediaFrame->MaxReliableDepth) * depthScaleInMeters;
        m_rasterizeFrameBlob.depthImage = m_depthTextureSRV;
        m_rasterizeFrameBlob.depthUnprojectionMap = m_depthUnprojectionMapTextureSRV;

        m_depthFrameIndex = depthFrameBlob.index;
    }

    Platform::IBox<float4x4>^ depthToColorRef = depthFrame->CoordinateSystem->TryGetTransformTo(colorFrame->CoordinateSystem);

    if (depthToColorRef)
    {
        m_rasterizeFrameBlob.depthToSource = depthToColorRef->Value;
        m_rasterizeFrameBlob.depthToTarget = depthToColorRef->Value;
    }
    else
    {
        throw std::exception("These camera frames can't be correlated");
    }

    return true;
}
// If we're too slow while processing frames (i.e. running under a debugger), then we may access a VideoMediaFrame that was already closed.
catch (Platform::ObjectDisposedException^)
{
    return false;
}

void CameraCoordinateMapper::RunCoordinateMapping()
{
    m_gpuCoordinateMapper->RunCoordinateMapping(m_rasterizeFrameBlob);
}

void CameraCoordinateMapper::VisualizeCoordinateMapping(ID3D11RenderTargetView * renderTarget, ID3D11DepthStencilView * depthBuffer, const D3D11_VIEWPORT & viewport, const Windows::Foundation::Numerics::float4x4 & worldToView)
{
    const float aspectRatio = float(viewport.Width) / float(viewport.Height);

    const float4x4 viewToProj = make_float4x4_perspective_field_of_view(DirectX::XMConvertToRadians(90.0f), aspectRatio, 0.001f, 100.0f);

    m_visualizeFrameBlob.renderTarget = renderTarget;
    m_visualizeFrameBlob.renderTargetDepthBuffer = depthBuffer;
    m_visualizeFrameBlob.worldToView = worldToView;
    m_visualizeFrameBlob.viewToProj = viewToProj;
    m_visualizeFrameBlob.viewport = viewport;

    m_gpuCoordinateMapper->VisualizeCoordinateMapping(m_rasterizeFrameBlob, m_visualizeFrameBlob);

    m_visualizeFrameBlob.renderTargetDepthBuffer = nullptr;
    m_visualizeFrameBlob.renderTarget = nullptr;
}

bool CameraCoordinateMapper::AreCamerasStreaming() const
{
    const bool colorIsStreaming = m_colorCameraStreamProcessor && m_colorCameraStreamProcessor->IsStreaming();
    const bool depthIsStreaming = m_depthCameraStreamProcessor && m_depthCameraStreamProcessor->IsStreaming();

    return colorIsStreaming && depthIsStreaming;
}

ID3D11ShaderResourceView* CameraCoordinateMapper::GetTargetShaderResourceView() const
{
    return m_targetTextureSRV.Get();
}

float CameraCoordinateMapper::GetTargetAspectRatio() const
{
    return static_cast<float>(m_colorIntrinsics.imageWidth) / static_cast<float>(m_colorIntrinsics.imageHeight);
}

void CameraCoordinateMapper::UpdateDepthResources(Windows::Media::Capture::Frames::MediaFrameReference ^ depthFrame)
{
    ID3D11Device* device = m_deviceResources->GetD3DDevice();
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

    CameraIntrinsics^ depthIntrinsicsRt = depthFrame->VideoMediaFrame->CameraIntrinsics;
    const GpuCoordinateMapper::CameraIntrinsics depthIntrinsics(depthIntrinsicsRt);

    // Only need to update the internal data if the intrinsics have changed
    if (depthIntrinsics != m_depthIntrinsics)
    {
        if (depthFrame->VideoMediaFrame->VideoFormat->MediaFrameFormat->Subtype != Windows::Media::MediaProperties::MediaEncodingSubtypes::D16) 
        {
            throw std::invalid_argument("This depth format is not yet supported");
        }

        m_depthTexture = DX::CreateTexture(
            device,
            depthIntrinsics.imageWidth,
            depthIntrinsics.imageHeight,
            DXGI_FORMAT_R16_UNORM,
            std::nullopt,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE);

        m_depthTextureSRV = DX::CreateShaderResourceView(m_depthTexture.Get());

        m_depthUnprojectionMapTexture = GenerateIntrinsicsMapping(device, depthIntrinsicsRt,
            [](CameraIntrinsics^ intrinsics, Point point)
        {
            return intrinsics->UnprojectAtUnitDepth(point);
        });

        m_depthUnprojectionMapTextureSRV = DX::CreateShaderResourceView(m_depthUnprojectionMapTexture.Get());

        m_depthIntrinsics = depthIntrinsics;
    }
}

void CameraCoordinateMapper::UpdateColorResources(Windows::Media::Capture::Frames::MediaFrameReference ^ colorFrame)
{
    ID3D11Device* device = m_deviceResources->GetD3DDevice();
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

    CameraIntrinsics^ colorIntrinsicsRt = colorFrame->VideoMediaFrame->CameraIntrinsics;
    const GpuCoordinateMapper::CameraIntrinsics colorIntrinsics(colorIntrinsicsRt);

    // Only need to update the internal data if the intrinsics have changed
    if (colorIntrinsics != m_colorIntrinsics)
    {
        // Future work: Support more color formats
        if (colorFrame->VideoMediaFrame->VideoFormat->MediaFrameFormat->Subtype != L"ARGB32") 
        {
            throw std::invalid_argument("This video format is not yet supported");
        }

        m_colorTexture = DX::CreateTexture(
            device,
            colorIntrinsics.imageWidth,
            colorIntrinsics.imageHeight,
            DXGI_FORMAT_B8G8R8A8_UNORM,
            std::nullopt,
            D3D11_BIND_SHADER_RESOURCE,
            D3D11_USAGE_DYNAMIC,
            D3D11_CPU_ACCESS_WRITE);

        m_colorTextureSRV = DX::CreateShaderResourceView(m_colorTexture.Get());

        m_colorDistortionMapTexture = GenerateIntrinsicsMapping(device, colorIntrinsicsRt,
            [](CameraIntrinsics^ intrinsics, Point point)
        {
            return intrinsics->DistortPoint(point);
        });

        m_colorDistortionMapTextureSRV = DX::CreateShaderResourceView(m_colorDistortionMapTexture.Get());

        m_targetTexture = DX::CreateTexture(
            device,
            colorIntrinsics.imageWidth,
            colorIntrinsics.imageHeight,
            DXGI_FORMAT_B8G8R8A8_UNORM,
            std::nullopt,
            D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

        m_targetTextureSRV = DX::CreateShaderResourceView(m_targetTexture.Get());
        m_targetTextureRTV = DX::CreateRenderTargetView(m_targetTexture.Get());

        m_targetRasterizedDepthTexture = DX::CreateTexture(
            device,
            colorIntrinsics.imageWidth,
            colorIntrinsics.imageHeight,
            DXGI_FORMAT_R24G8_TYPELESS,
            std::nullopt,
            D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL);

        m_targetRasterizedDepthTextureDSV = DX::CreateDepthStencilView(
            m_targetRasterizedDepthTexture.Get(),
            DXGI_FORMAT_D24_UNORM_S8_UINT);

        m_targetRasterizedDepthTextureSRV = DX::CreateShaderResourceView(
            m_targetRasterizedDepthTexture.Get(),
            DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

        m_visualizeFrameBlob.targetImage = m_targetTextureSRV;
        m_visualizeFrameBlob.targetImageDepthBuffer = m_targetRasterizedDepthTextureSRV;

        m_colorIntrinsics = colorIntrinsics;
    }
}
