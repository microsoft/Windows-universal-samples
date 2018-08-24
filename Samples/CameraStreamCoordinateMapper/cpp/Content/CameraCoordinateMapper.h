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

#include "Common\DeviceResources.h"
#include "GpuCoordinateMapper.h"

namespace CameraStreamCoordinateMapper {

    class CameraStreamProcessor;

    ////////////////////////////////////////////////////////////////////////////////
    // CameraCoordinateMapper
    //
    // This gathers frames from the CameraStreamProcessor and feeds them into the GpuCoordinateMapper
    //
    // The core algorithm resides in GpuCoordinateMapper and its respective shaders, CameraCoordinateMapper
    // is meant to manage the resources and recreate them if the camera's intrinsics or other properties change

    class CameraCoordinateMapper
    {
    public:
        static concurrency::task<std::shared_ptr<CameraCoordinateMapper>> CreateAndStartAsync(std::shared_ptr<DX::DeviceResources> deviceResources);

        CameraCoordinateMapper(
            std::shared_ptr<DX::DeviceResources> deviceResources,
            std::shared_ptr<CameraStreamProcessor> colorStreamProcessor,
            std::shared_ptr<CameraStreamProcessor> depthStreamProcessor);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

        concurrency::task<void> StartAsync();
        concurrency::task<void> StopAsync();

        bool TryAcquireLatestFrameData();

        void RunCoordinateMapping();

        void VisualizeCoordinateMapping(
            ID3D11RenderTargetView* renderTarget,
            ID3D11DepthStencilView* depthBuffer,
            const D3D11_VIEWPORT& viewport,
            const Windows::Foundation::Numerics::float4x4& worldToView);

        bool AreCamerasStreaming() const;
        ID3D11ShaderResourceView* GetTargetShaderResourceView() const;
        float GetTargetAspectRatio() const;

    private:
        void UpdateDepthResources(Windows::Media::Capture::Frames::MediaFrameReference^ depthFrame);
        void UpdateColorResources(Windows::Media::Capture::Frames::MediaFrameReference^ colorFrame);

        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        std::shared_ptr<CameraStreamProcessor> m_colorCameraStreamProcessor;
        std::shared_ptr<CameraStreamProcessor> m_depthCameraStreamProcessor;
        std::unique_ptr<GpuCoordinateMapper> m_gpuCoordinateMapper;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthTextureSRV;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_colorTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_colorTextureSRV;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_targetTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_targetTextureSRV;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_targetTextureRTV;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_targetRasterizedDepthTexture;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_targetRasterizedDepthTextureDSV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_targetRasterizedDepthTextureSRV;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthUnprojectionMapTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthUnprojectionMapTextureSRV;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_colorDistortionMapTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_colorDistortionMapTextureSRV;

        GpuCoordinateMapper::RasterizedFrameBlob m_rasterizeFrameBlob = {};
        GpuCoordinateMapper::VisualizeFrameBlob m_visualizeFrameBlob = {};

        GpuCoordinateMapper::CameraIntrinsics m_depthIntrinsics = {};
        GpuCoordinateMapper::CameraIntrinsics m_colorIntrinsics = {};

        std::optional<uint32_t> m_depthFrameIndex = {};
        std::optional<uint32_t> m_colorFrameIndex = {};
    };
}

