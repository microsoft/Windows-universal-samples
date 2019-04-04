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
#include <ppltasks.h>

namespace WindowsHolographicCodeSamples
{
    struct SurfaceMeshProperties
    {
        Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem = nullptr;
        Windows::Foundation::Numerics::float3 vertexPositionScale = Windows::Foundation::Numerics::float3::one();
        unsigned int vertexStride = 0;
        unsigned int normalStride = 0;
        unsigned int indexCount   = 0;
        DXGI_FORMAT  indexFormat  = DXGI_FORMAT_UNKNOWN;
    };

    class SurfaceMesh final
    {
    public:
        SurfaceMesh();
        ~SurfaceMesh();

        void UpdateSurface(Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^ surface);
        void UpdateTransform(
            ID3D11Device* device, 
            ID3D11DeviceContext* context,
            DX::StepTimer const& timer,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ baseCoordinateSystem
            );

        void Draw(ID3D11Device* device, ID3D11DeviceContext* context, bool usingVprtShaders, bool isStereo);

        void UpdateVertexResources(ID3D11Device* device);
        void CreateDeviceDependentResources(ID3D11Device* device);
        void ReleaseVertexResources();
        void ReleaseDeviceDependentResources();

        const bool&                             GetIsActive()       const { return m_isActive;       }
        const float&                            GetLastActiveTime() const { return m_lastActiveTime; }
        const Windows::Foundation::DateTime&    GetLastUpdateTime() const { return m_lastUpdateTime; }

        void SetIsActive(const bool& isActive)          { m_isActive = isActive;                                    }
        void SetColorFadeTimer(const float& duration)   { m_colorFadeTimeout = duration; m_colorFadeTimer = 0.f;    }

    private:
        void SwapVertexBuffers();
        void CreateDirectXBuffer(
            ID3D11Device* device,
            D3D11_BIND_FLAG binding,
            Windows::Storage::Streams::IBuffer^ buffer,
            ID3D11Buffer** target
            );

        concurrency::task<void> m_updateVertexResourcesTask = concurrency::task_from_result();

        Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^ m_pendingSurfaceMesh = nullptr;
        Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^ m_surfaceMesh = nullptr;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexPositions;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexNormals;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_triangleIndices;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_updatedVertexPositions;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_updatedVertexNormals;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_updatedTriangleIndices;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_modelTransformBuffer;

        Windows::Foundation::DateTime m_lastUpdateTime;

        SurfaceMeshProperties m_meshProperties;
        SurfaceMeshProperties m_updatedMeshProperties;

        ModelNormalConstantBuffer m_constantBufferData;

        bool   m_constantBufferCreated = false;
        bool   m_loadingComplete    = false;
        bool   m_updateReady        = false;
        bool   m_isActive           = false;
        float  m_lastActiveTime     = -1.f;
        float  m_colorFadeTimer     = -1.f;
        float  m_colorFadeTimeout   = -1.f;

        std::mutex m_meshResourcesMutex;
    };
}
