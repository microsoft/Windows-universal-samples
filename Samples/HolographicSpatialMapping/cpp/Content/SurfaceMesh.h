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

namespace WindowsHolographicCodeSamples
{
    class SurfaceMesh final
    {
    public:
        SurfaceMesh();
        ~SurfaceMesh();

        void UpdateSurface(Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^ surface);
        void UpdateDeviceBasedResources(ID3D11Device* device);
        void UpdateTransform(
            ID3D11DeviceContext* context,
            DX::StepTimer const& timer,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ baseCoordinateSystem
            );

        void Draw(ID3D11Device* device, ID3D11DeviceContext* context, bool usingVprtShaders, bool isStereo);

        void CreateVertexResources(ID3D11Device* device);
        void CreateDeviceDependentResources(ID3D11Device* device);
        void ReleaseVertexResources();
        void ReleaseDeviceDependentResources();

        const bool&  GetIsActive()       const          { return m_isActive;            }
        const float& GetLastActiveTime() const          { return m_lastActiveTime;      }

        void SetIsActive(const bool& isActive)          { m_isActive = isActive;        }
        void SetColorFadeTimer(const float& duration)   { m_colorFadeTimeout = duration; m_colorFadeTimer = 0.f; }

    private:
        void CreateDirectXBuffer(
            ID3D11Device* device,
            D3D11_BIND_FLAG binding,
            Windows::Storage::Streams::IBuffer^ buffer,
            ID3D11Buffer** target
            );

        Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^ m_surfaceMesh = nullptr;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexPositions;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexNormals;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_triangleIndices;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_modelTransformBuffer;

        ModelNormalConstantBuffer  m_constantBufferData;

        unsigned int m_vertexStride = 0;
        unsigned int m_normalStride = 0;
        DXGI_FORMAT  m_indexFormat  = DXGI_FORMAT_UNKNOWN;

        bool   m_loadingComplete    = false;
        bool   m_updateNeeded       = false;
        bool   m_isActive           = false;
        float  m_lastActiveTime     = -1.f;
        float  m_colorFadeTimer     = -1.f;
        float  m_colorFadeTimeout   = -1.f;
        uint32 m_indexCount         = 0;
    };
}
