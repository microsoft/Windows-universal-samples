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

#include "DepthBufferPlaneFinder\ApplyDepthWeights.h"

namespace DX
{
    class DeviceResources;

    // Constant buffer used to send the view-projection matrices to the shader pipeline.
    struct ViewProjectionConstantBuffer
    {
        DirectX::XMFLOAT4X4 viewProjection[2];
    };

    // Assert that the constant buffer remains 16-byte aligned (best practice).
    static_assert((sizeof(ViewProjectionConstantBuffer) % (sizeof(float) * 4)) == 0, "ViewProjection constant buffer size must be 16-byte aligned (16 bytes is the length of four floats).");

    // Manages DirectX device resources that are specific to a holographic camera, such as the
    // back buffer, ViewProjection constant buffer, and viewport.
    class CameraResources
    {
    public:
        CameraResources(Windows::Graphics::Holographic::HolographicCamera^ holographicCamera);

        void CreateResourcesForBackBuffer(
            DX::DeviceResources* pDeviceResources,
            Windows::Graphics::Holographic::HolographicCameraRenderingParameters^ cameraParameters
            );
        void ReleaseResourcesForBackBuffer(
            DX::DeviceResources* pDeviceResources
            );

        void UpdateViewProjectionBuffer(
            std::shared_ptr<DX::DeviceResources> deviceResources,
            Windows::Graphics::Holographic::HolographicCameraPose^ cameraPose,
            Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem);

        bool AttachViewProjectionBuffer(
            std::shared_ptr<DX::DeviceResources> deviceResources);

        // Direct3D device resources.
        ID3D11RenderTargetView*     GetBackBufferRenderTargetView()  const { return m_d3dRenderTargetView.Get();         }
        ID3D11DepthStencilView*     GetDepthStencilView()            const { return m_d3dDepthStencilView.Get();         }
        ID3D11Texture2D*            GetBackBufferTexture2D()         const { return m_d3dBackBuffer.Get();               }
        D3D11_VIEWPORT              GetViewport()                    const { return m_d3dViewport;                       }
        DXGI_FORMAT                 GetBackBufferDXGIFormat()        const { return m_dxgiFormat;                        }
        ID3D11ShaderResourceView*   GetDepthShaderResourceView()     const { return m_d3dDepthShaderResourceView.Get();  }

        ID3D11Buffer *              GetResolvedDepthTexture()        const { return m_d3dResolvedDepthBuffer.Get();     }
        ID3D11Buffer *              GetCPUResolvedDepthTexture()     const { return m_d3dCPUResolvedDepthTexture.Get();  }
        ID3D11Buffer *              GetMappableDefaultDepthTexture() const { return m_d3dResolvedDepthBufferMappable.Get(); }
        ID3D11UnorderedAccessView * GetResolvedDepthTextureView()    const { return m_d3dResolvedDepthView.Get();        }
        ID3D11UnorderedAccessView * GetResolvedDepthTextureViewMappable()       const { return m_d3dResolvedDepthViewMappable.Get();}
        
        Windows::Foundation::Numerics::float4x4 const& GetViewMatrix()          const { return m_view; }
        Windows::Foundation::Numerics::float4x4 const& GetInverseViewMatrix()   const { return m_viewInverse; }
        Windows::Foundation::Numerics::float4x4 const& GetProjectionMatrix()    const { return m_projection; }
        float const& GetNearPlane()         const { return m_nearPlaneDistance; }
        float const& GetFarPlane()          const { return m_farPlaneDistance; }
        float const* GetDepthWeightArray()  const { return m_depthWeightArray; }

        // Render target properties.
        Windows::Foundation::Size GetRenderTargetSize()               const { return m_d3dRenderTargetSize;               }
        bool                      IsRenderingStereoscopic()           const { return m_isStereo;                          }

        // The holographic camera these resources are for.
        Windows::Graphics::Holographic::HolographicCamera^ GetHolographicCamera() const { return m_holographicCamera;   }

    private:
        // Direct3D rendering objects. Required for 3D.
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_d3dRenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>      m_d3dDepthStencilView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_d3dBackBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_d3dDepthShaderResourceView;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_d3dResolvedDepthBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_d3dCPUResolvedDepthTexture;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_d3dResolvedDepthView;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_d3dResolvedDepthBufferMappable;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_d3dResolvedDepthViewMappable;

        // Device resource to store view and projection matrices.
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_viewProjectionConstantBuffer;
        
        Windows::Foundation::Numerics::float4x4             m_view = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
        Windows::Foundation::Numerics::float4x4             m_viewInverse = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
        Windows::Foundation::Numerics::float4x4             m_projection = { 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 };

        // Direct3D rendering properties.
        DXGI_FORMAT                                         m_dxgiFormat;
        Windows::Foundation::Size                           m_d3dRenderTargetSize;
        D3D11_VIEWPORT                                      m_d3dViewport;

        // Indicates whether the camera supports stereoscopic rendering.
        bool                                                m_isStereo = false;

        // Indicates whether this camera has a pending frame.
        bool                                                m_framePending = false;

        // Pointer to the holographic camera these resources are for.
        Windows::Graphics::Holographic::HolographicCamera^  m_holographicCamera = nullptr;

        // The precomputed array of depth weights that are used for this camera.
        float m_depthWeightArray[DEPTH_ARRAY_SIZE];
        
        // The near and far plane.
        const float m_nearPlaneDistance = 0.1f;
        const float m_farPlaneDistance = 20.f;
    };
}
