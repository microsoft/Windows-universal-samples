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
#include "ShaderStructures.h"

namespace CameraStreamCoordinateMapper
{
    ////////////////////////////////////////////////////////////////////////////////
    // GpuCoordinateMapper
    //
    // This class is the core class for mapping image data between the correlated cameras
    // 
    // It uses triangle rasterization on the gpu to increase performance of processing
    // the depth image. This is particuarly useful when the depth + color cameras do 
    // not have matching frame sizes. For example on Kinect V2 the depth camera is 512x424 
    // and the color camera is 1920x1080. 
    //
    // This means there are almost 4 color pixels for each depth pixel (along the x-axis). 
    // We can use triangle rasterization to interpolate the missing values to give us the 
    // most accurate result. 

    class GpuCoordinateMapper
    {
    public:

        // This is used to detect when the CameraIntrinsics from the VideoMediaFrame have
        // changed. We store the underlying intrinsics values to compare against on a
        // frame-by-frame basis. If they change, then we can recreate resources that were
        // dependent on the intrinsics values (e.g. Distortion and Unprojection look up tables)
        struct CameraIntrinsics
        {
            CameraIntrinsics() = default;

            explicit CameraIntrinsics(Windows::Media::Devices::Core::CameraIntrinsics^ intrinsics) :
                imageWidth(intrinsics->ImageWidth),
                imageHeight(intrinsics->ImageHeight),
                principalPoint(intrinsics->PrincipalPoint),
                focalLength(intrinsics->FocalLength),
                radialDistortion(intrinsics->RadialDistortion),
                tangentialDistortion(intrinsics->TangentialDistortion)
            {}

            inline bool operator!=(const CameraIntrinsics& rhs) const
            {
                return std::memcmp(this, &rhs, sizeof(CameraIntrinsics)) != 0;
            }

            uint32_t imageWidth;
            uint32_t imageHeight;
            Windows::Foundation::Numerics::float2 principalPoint;
            Windows::Foundation::Numerics::float2 focalLength;
            Windows::Foundation::Numerics::float3 radialDistortion;
            Windows::Foundation::Numerics::float2 tangentialDistortion;
        };

        struct RasterizedFrameBlob {
            CameraIntrinsics depthIntrinsics;
            CameraIntrinsics sourceIntrinsics;
            CameraIntrinsics targetIntrinsics;
            Windows::Foundation::Numerics::float4x4 depthToSource;
            Windows::Foundation::Numerics::float4x4 depthToTarget;
            Windows::Foundation::Numerics::float4x4 sourceProjectionMatrix;
            Windows::Foundation::Numerics::float4x4 targetProjectionMatrix;
            float depthScaleInMeters;
            float depthRangeMinimumInMeters;
            float depthRangeMaximumInMeters;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthImage;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthUnprojectionMap;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sourceImage;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sourceDistortionMap;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetImage;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> targetRasterizedDepth;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> targetDistortionMap;
        };

        struct VisualizeFrameBlob {
            Windows::Foundation::Numerics::float4x4 worldToView;
            Windows::Foundation::Numerics::float4x4 viewToProj;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> targetImage;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> targetImageDepthBuffer;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> renderTargetDepthBuffer;
            D3D11_VIEWPORT viewport;
        };

        GpuCoordinateMapper(
            const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();

        ////////////////////////////////////////////////////////////////////////////////
        // RunCoordinateMapping - executes the correlation algorithm for this set of frame data
        void RunCoordinateMapping(
            const RasterizedFrameBlob& frameBlob);

        ////////////////////////////////////////////////////////////////////////////////
        // VisualizeCoordinateMapping - executes the correlation algorithm for this set of frame data
        //   except visualized from another perspective, using depth information from the previous pass
        //   in order to occlude pixels that weren't visible from the original target camera's perspective
        void VisualizeCoordinateMapping(
            const RasterizedFrameBlob& rasterizedFrameBlob,
            const VisualizeFrameBlob& visualizeFrameBlob);

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_rasterizeDepthMeshVertexShader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_rasterizeDepthMeshInputLayout;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_rasterizeDepthMeshPixelShader;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_visualizeDepthMeshVertexShader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_visualizeDepthMeshInputLayout;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_visualizeDepthMeshPixelShader;

        Microsoft::WRL::ComPtr<ID3D11Buffer>       m_rasterizeDepthMeshConstants;
        Microsoft::WRL::ComPtr<ID3D11Buffer>       m_visualizeDepthMeshConstants;
        Microsoft::WRL::ComPtr<ID3D11Buffer>       m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

        uint32_t m_vertexCount;
    };
}