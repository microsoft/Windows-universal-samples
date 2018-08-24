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
#include "GpuCoordinateMapper.h"
#include "Common\DirectXHelper.h"

#include "Shaders\RasterizeDepthMeshVS.h"
#include "Shaders\RasterizeDepthMeshPS.h"

#include "Shaders\VisualizeDepthMeshVS.h"
#include "Shaders\VisualizeDepthMeshPS.h"

using namespace CameraStreamCoordinateMapper;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::Foundation::Numerics;

namespace 
{
    CameraIntrinsics_Shader GetCameraIntrinsicsConstantsForShader(const GpuCoordinateMapper::CameraIntrinsics& intrinsics)
    {
        CameraIntrinsics_Shader constants;

        constants.imageWidth = intrinsics.imageWidth;
        constants.imageHeight = intrinsics.imageHeight;

        return constants;
    }

    RasterizeDepthMeshConstants_Shader GetRasterizeDepthMeshConstantsForShader(const GpuCoordinateMapper::RasterizedFrameBlob& frameBlob)
    {
        RasterizeDepthMeshConstants_Shader constants;

        constants.depthIntrinsics = GetCameraIntrinsicsConstantsForShader(frameBlob.depthIntrinsics);
        constants.targetIntrinsics = GetCameraIntrinsicsConstantsForShader(frameBlob.targetIntrinsics);
        constants.sourceIntrinsics = GetCameraIntrinsicsConstantsForShader(frameBlob.sourceIntrinsics);
        constants.depthToSource = frameBlob.depthToSource;
        constants.depthToTarget = frameBlob.depthToTarget;
        constants.sourceProjectionMatrix = frameBlob.sourceProjectionMatrix;
        constants.targetProjectionMatrix = frameBlob.targetProjectionMatrix;

        // DepthMaps are currently D16 format: [0,65535], depthScaleInMeters is how large each unit is in meters. 
        constants.depthMapUnitsToMeters = frameBlob.depthScaleInMeters * std::numeric_limits<UINT16>::max();
        constants.depthRangeMinimumInMeters = frameBlob.depthRangeMinimumInMeters;
        constants.depthRangeMaximumInMeters = frameBlob.depthRangeMaximumInMeters;
        constants.depthMapImageWidthMinusOne = frameBlob.depthIntrinsics.imageWidth - 1u;

        return constants;
    }

    VisualizeDepthMeshConstants_Shader GetVisualizeDepthMeshConstantsForShader(const GpuCoordinateMapper::VisualizeFrameBlob& frameBlob)
    {
        VisualizeDepthMeshConstants_Shader constants;

        constants.worldToProj = frameBlob.worldToView * frameBlob.viewToProj;

        return constants;
    }
}

GpuCoordinateMapper::GpuCoordinateMapper(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    CreateDeviceDependentResources();
}

void CameraStreamCoordinateMapper::GpuCoordinateMapper::CreateDeviceDependentResources()
{
    ID3D11Device* device = m_deviceResources->GetD3DDevice();

    static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    m_rasterizeDepthMeshVertexShader = DX::CreateVertexShader(
        device,
        g_RasterizeDepthMeshVS,
        _countof(g_RasterizeDepthMeshVS));

    m_rasterizeDepthMeshInputLayout = DX::CreateInputLayout(
        device,
        vertexDesc,
        _countof(vertexDesc),
        g_RasterizeDepthMeshVS,
        _countof(g_RasterizeDepthMeshVS));

    m_rasterizeDepthMeshPixelShader = DX::CreatePixelShader(
        device,
        g_RasterizeDepthMeshPS,
        _countof(g_RasterizeDepthMeshPS));

    m_visualizeDepthMeshVertexShader = DX::CreateVertexShader(
        device,
        g_VisualizeDepthMeshVS,
        _countof(g_VisualizeDepthMeshVS));

    m_visualizeDepthMeshInputLayout = DX::CreateInputLayout(
        device,
        vertexDesc,
        _countof(vertexDesc),
        g_VisualizeDepthMeshVS,
        _countof(g_VisualizeDepthMeshVS));

    m_visualizeDepthMeshPixelShader = DX::CreatePixelShader(
        device,
        g_VisualizeDepthMeshPS,
        _countof(g_VisualizeDepthMeshPS));

    m_rasterizeDepthMeshConstants = DX::CreateBuffer(
        device,
        sizeof(RasterizeDepthMeshConstants_Shader),
        nullptr,
        D3D11_BIND_CONSTANT_BUFFER);

    m_visualizeDepthMeshConstants = DX::CreateBuffer(
        device,
        sizeof(VisualizeDepthMeshConstants_Shader),
        nullptr,
        D3D11_BIND_CONSTANT_BUFFER);

    m_samplerState = DX::CreateSamplerState(device);

    static const float4 quadVertices[] = {
        float4(-1.0f, +1.0f, 0.0f, 1.0f), // top left
        float4(+1.0f, +1.0f, 0.0f, 1.0f), // top right
        float4(-1.0f, -1.0f, 0.0f, 1.0f), // bottom left

        float4(+1.0f, +1.0f, 0.0f, 1.0f), // top right
        float4(+1.0f, -1.0f, 0.0f, 1.0f), // bottom right
        float4(-1.0f, -1.0f, 0.0f, 1.0f), // bottom left
    };

    m_vertexCount = _countof(quadVertices);

    m_vertexBuffer = DX::CreateBuffer(
        device,
        sizeof(quadVertices),
        quadVertices,
        D3D11_BIND_VERTEX_BUFFER);
}

void CameraStreamCoordinateMapper::GpuCoordinateMapper::ReleaseDeviceDependentResources()
{
    m_rasterizeDepthMeshVertexShader = nullptr;
    m_rasterizeDepthMeshInputLayout = nullptr;
    m_rasterizeDepthMeshPixelShader = nullptr;

    m_visualizeDepthMeshVertexShader = nullptr;
    m_visualizeDepthMeshInputLayout = nullptr;
    m_visualizeDepthMeshPixelShader = nullptr;

    m_rasterizeDepthMeshConstants = nullptr;
    m_visualizeDepthMeshConstants = nullptr;
    m_vertexBuffer = nullptr;
    m_samplerState = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// RunCoordinateMapping - executes the correlation algorithm for this set of frame data
//
// The inputs come from the "depth*", "source*", and "target*" parameters, output is written to the "targetImage*" parameters
//
// The core algorithm:
// 1.) Read each point from the depth image and undistort + unprojects it into 3D space (using depthUnprojectionMap)
// 2.) Transforms that 3D point into "source" and "target" camera space (using depthToSource and depthToTarget matrices)
// 3.) Projects + distorts it into source camera 2D space (using sourceProjectionMatrix and sourceDistortionMap) 
//     to determine which pixel to sample from 
// 4.) Projects + distorts it into target camera 2D space (using targetProjectionMatrix and targetDistortionMap) 
//     to determine where the final pixel is rendered on screen. 
//
// The most common scenario has "source" and "target" as the same camera (Color), but 
// the algorithm is generic to allow other scenarios. 

void CameraStreamCoordinateMapper::GpuCoordinateMapper::RunCoordinateMapping(
    const RasterizedFrameBlob& frameBlob)
{
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();
    
    const RasterizeDepthMeshConstants_Shader constants = GetRasterizeDepthMeshConstantsForShader(frameBlob);

    context->UpdateSubresource(m_rasterizeDepthMeshConstants.Get(), 0, nullptr, &constants, 0, 0);

    context->ClearRenderTargetView(frameBlob.targetImage.Get(), DirectX::Colors::Black);
    context->ClearDepthStencilView(frameBlob.targetRasterizedDepth.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    ////////////////////////////////////////////////////////////////////////////////
    // Input Assembler + Rasterizer
    const UINT stride = sizeof(float4);
    const UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(m_rasterizeDepthMeshInputLayout.Get());

    const D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        static_cast<float>(frameBlob.targetIntrinsics.imageWidth),
        static_cast<float>(frameBlob.targetIntrinsics.imageHeight)
    );

    context->RSSetViewports(1, &viewport);

    ////////////////////////////////////////////////////////////////////////////////
    // Vertex Shader
    context->VSSetShader(m_rasterizeDepthMeshVertexShader.Get(), nullptr, 0);

    ID3D11Buffer* vertexConstantBuffers[] = {
        m_rasterizeDepthMeshConstants.Get()
    };

    context->VSSetConstantBuffers(0, 1, m_rasterizeDepthMeshConstants.GetAddressOf());

    ID3D11ShaderResourceView* vertexShaderResources[] = {
        frameBlob.depthImage.Get(),           // Depth Texture
        frameBlob.depthUnprojectionMap.Get(), // Depth Unprojection Map
        frameBlob.sourceDistortionMap.Get(),  // Source Distortion Map
        frameBlob.targetDistortionMap.Get()   // Target Distortion Map
    };

    context->VSSetShaderResources(0, _countof(vertexShaderResources), vertexShaderResources);
    context->VSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    ////////////////////////////////////////////////////////////////////////////////
    // Pixel Shader
    context->PSSetShader(m_rasterizeDepthMeshPixelShader.Get(), nullptr, 0);

    ID3D11ShaderResourceView* pixelShaderResources[] = {
        frameBlob.sourceImage.Get() // Source Texture
    };

    context->PSSetShaderResources(0, _countof(pixelShaderResources), pixelShaderResources);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    ////////////////////////////////////////////////////////////////////////////////
    // Output Merger
    ID3D11RenderTargetView* outputRenderTargets[] = {
        frameBlob.targetImage.Get() // Target Texture
    };

    context->OMSetRenderTargets(_countof(outputRenderTargets), outputRenderTargets, frameBlob.targetRasterizedDepth.Get());

    ////////////////////////////////////////////////////////////////////////////////
    // Render
    const uint32_t instanceCount = (frameBlob.depthIntrinsics.imageWidth - 1) * (frameBlob.depthIntrinsics.imageHeight - 1);

    context->DrawInstanced(m_vertexCount, instanceCount, 0, 0);

    ////////////////////////////////////////////////////////////////////////////////
    // Cleanup: Unbind resources that might be used in later passes
    decltype(outputRenderTargets) null_outputRenderTargets = { nullptr };
    context->OMSetRenderTargets(_countof(null_outputRenderTargets), null_outputRenderTargets, nullptr);

    decltype(pixelShaderResources) null_pixelShaderResources = { nullptr };
    context->PSSetShaderResources(0, _countof(null_pixelShaderResources), null_pixelShaderResources);

    decltype(vertexShaderResources) null_vertexShaderResources = { nullptr };
    context->VSSetShaderResources(0, _countof(null_vertexShaderResources), null_vertexShaderResources);
}

////////////////////////////////////////////////////////////////////////////////
// VisualizeCoordinateMapping - executes the correlation algorithm for this set of frame data
//   except visualized from another perspective, using depth information from the previous pass
//   in order to occlude pixels that weren't visible from the original target camera's perspective
//
// The inputs come from the "depth*" and "target*" parameters inside of the rasterizedFrameBlob.
// The output is written to the visualizeFrameBlob "renderTarget*" parameters
// 
// The core algorithm for visualizing from alternate perspectives:
// 1.) Read each point from the depth image and undistort + unprojects it into 3D space (using depthUnprojectionMap)
// 2.) Transforms that 3D point into "target" camera space (using depthToTarget matrices)
// 3.) Projects + distorts it into target camera 2D space (using targetProjectionMatrix and targetDistortionMap) 
//     to determine where the final pixel was rendered on screen (so we can sample the depth buffer in the pixel shader)
// 4.) Compute the depth of this point in "target" camera space and store it in the vertex
// 5.) Compute the position of the 3D point given the app provided camera/projection

void GpuCoordinateMapper::VisualizeCoordinateMapping(
    const RasterizedFrameBlob& rasterizedFrameBlob,
    const VisualizeFrameBlob& visualizeFrameBlob)
{
    ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

    const RasterizeDepthMeshConstants_Shader rasterizeConstants = GetRasterizeDepthMeshConstantsForShader(rasterizedFrameBlob);
    context->UpdateSubresource(m_rasterizeDepthMeshConstants.Get(), 0, nullptr, &rasterizeConstants, 0, 0);

    const VisualizeDepthMeshConstants_Shader visualizeConstants = GetVisualizeDepthMeshConstantsForShader(visualizeFrameBlob);
    context->UpdateSubresource(m_visualizeDepthMeshConstants.Get(), 0, nullptr, &visualizeConstants, 0, 0);

    context->ClearRenderTargetView(visualizeFrameBlob.renderTarget.Get(), DirectX::Colors::Black);
    context->ClearDepthStencilView(visualizeFrameBlob.renderTargetDepthBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    ////////////////////////////////////////////////////////////////////////////////
    // Input Assembler + Rasterizer
    const UINT stride = sizeof(float4);
    const UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(m_rasterizeDepthMeshInputLayout.Get());

    context->RSSetViewports(1, &visualizeFrameBlob.viewport);

    ////////////////////////////////////////////////////////////////////////////////
    // Vertex Shader
    context->VSSetShader(m_visualizeDepthMeshVertexShader.Get(), nullptr, 0);

    ID3D11Buffer* const vertexConstantBuffers[] = {
        m_rasterizeDepthMeshConstants.Get(),
        m_visualizeDepthMeshConstants.Get()
    };

    context->VSSetConstantBuffers(0, _countof(vertexConstantBuffers), vertexConstantBuffers);

    ID3D11ShaderResourceView* const vertexShaderResources[] = {
        rasterizedFrameBlob.depthImage.Get(),           // Depth Texture
        rasterizedFrameBlob.depthUnprojectionMap.Get(), // Depth Unprojection Map
        rasterizedFrameBlob.targetDistortionMap.Get()   // Target Distortion Map
    };

    context->VSSetShaderResources(0, _countof(vertexShaderResources), vertexShaderResources);
    context->VSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    ////////////////////////////////////////////////////////////////////////////////
    // Pixel Shader
    context->PSSetShader(m_visualizeDepthMeshPixelShader.Get(), nullptr, 0);

    ID3D11Buffer* const pixelConstantBuffers[] = {
        m_rasterizeDepthMeshConstants.Get()
    };

    context->PSSetConstantBuffers(0, _countof(pixelConstantBuffers), pixelConstantBuffers);

    ID3D11ShaderResourceView* const pixelShaderResources[] = {
        visualizeFrameBlob.targetImage.Get(),           // Target Texture
        visualizeFrameBlob.targetImageDepthBuffer.Get() // Target Depth Buffer
    };

    context->PSSetShaderResources(0, _countof(pixelShaderResources), pixelShaderResources);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    ////////////////////////////////////////////////////////////////////////////////
    // Output Merger
    ID3D11RenderTargetView* const outputRenderTargets[] = {
        visualizeFrameBlob.renderTarget.Get()
    };

    context->OMSetRenderTargets(_countof(outputRenderTargets), outputRenderTargets, visualizeFrameBlob.renderTargetDepthBuffer.Get());

    ////////////////////////////////////////////////////////////////////////////////
    // Render
    const uint32_t instanceCount = (rasterizedFrameBlob.depthIntrinsics.imageWidth - 1) * (rasterizedFrameBlob.depthIntrinsics.imageHeight - 1);

    context->DrawInstanced(m_vertexCount, instanceCount, 0, 0);

    ////////////////////////////////////////////////////////////////////////////////
    // Cleanup: Unbind resources that might be used in later passes
    decltype(outputRenderTargets) null_outputRenderTargets = { nullptr };
    context->OMSetRenderTargets(_countof(null_outputRenderTargets), null_outputRenderTargets, nullptr);

    decltype(pixelShaderResources) null_pixelShaderResources = { nullptr };
    context->PSSetShaderResources(0, _countof(null_pixelShaderResources), null_pixelShaderResources);

    decltype(vertexShaderResources) null_vertexShaderResources = { nullptr };
    context->VSSetShaderResources(0, _countof(null_vertexShaderResources), null_vertexShaderResources);
}
