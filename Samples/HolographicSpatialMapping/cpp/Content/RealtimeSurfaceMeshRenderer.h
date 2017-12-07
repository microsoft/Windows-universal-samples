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
#include "Common\StepTimer.h"
#include "Content\SurfaceMesh.h"
#include "Content\ShaderStructures.h"

#include <memory>
#include <map>
#include <ppltasks.h>

namespace WindowsHolographicCodeSamples
{
    class RealtimeSurfaceMeshRenderer
    {
    public:
        RealtimeSurfaceMeshRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(
            DX::StepTimer const& timer,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem
            );
        void Render(bool isStereo, bool useWireframe);

        bool HasSurface(Platform::Guid id);
        void AddSurface(Platform::Guid id, Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ newSurface);
        void UpdateSurface(Platform::Guid id, Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ newSurface);
        void RemoveSurface(Platform::Guid id);
        void ClearSurfaces();

        Windows::Foundation::DateTime GetLastUpdateTime(Platform::Guid id);

        void HideInactiveMeshes(
            Windows::Foundation::Collections::IMapView<Platform::Guid,
            Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^>^ const& surfaceCollection);

    private:
        Concurrency::task<void> AddOrUpdateSurfaceAsync(Platform::Guid id, Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ newSurface);

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        // Direct3D resources for SR mesh rendering pipeline.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_lightingPixelShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_colorPixelShader;

        // The set of surfaces in the collection.
        std::map<Platform::Guid, SurfaceMesh>           m_meshCollection;

        // A way to lock map access.
        std::mutex                                      m_meshCollectionLock;

        // Total number of surface meshes.
        unsigned int                                    m_surfaceMeshCount;

        // Level of detail setting. The number of triangles that the system is allowed to provide per cubic meter.
        double                                          m_maxTrianglesPerCubicMeter = 1000.0;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                            m_usingVprtShaders = false;

        // Rasterizer states, for different rendering modes.
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_defaultRasterizerState;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_wireframeRasterizerState;

        // The duration of time, in seconds, a mesh is allowed to remain inactive before deletion.
        const float c_maxInactiveMeshTime = 120.f;

        // The duration of time, in seconds, taken for a new surface mesh to fade in on-screen.
        const float c_surfaceMeshFadeInTime = 3.0f;

        bool m_loadingComplete;
    };
};

