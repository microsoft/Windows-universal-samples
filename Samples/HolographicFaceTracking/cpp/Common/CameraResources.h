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
    class DeviceResources;

    // Constant buffer used to send the view-projection matrices to the shader pipeline.
    struct ViewProjectionConstantBuffer
    {
        Windows::Foundation::Numerics::float4x4 viewProjection[2];
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
        ID3D11RenderTargetView* GetBackBufferRenderTargetView()     const { return m_d3dRenderTargetView.Get();     }
        ID3D11DepthStencilView* GetDepthStencilView()               const { return m_d3dDepthStencilView.Get();     }
        ID3D11Texture2D*        GetBackBufferTexture2D()            const { return m_d3dBackBuffer.Get();           }
        D3D11_VIEWPORT          GetViewport()                       const { return m_d3dViewport;                   }
        DXGI_FORMAT             GetBackBufferDXGIFormat()           const { return m_dxgiFormat;                    }

        // Render target properties.
        Windows::Foundation::Size GetRenderTargetSize()             const { return m_d3dRenderTargetSize;           }
        bool                    IsRenderingStereoscopic()           const { return m_isStereo;                      }

        // The holographic camera these resources are for.
        Windows::Graphics::Holographic::HolographicCamera^ GetHolographicCamera() const { return m_holographicCamera; }

    private:
        // Direct3D rendering objects. Required for 3D.
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_d3dRenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>      m_d3dDepthStencilView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_d3dBackBuffer;

        // Device resource to store view and projection matrices.
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_viewProjectionConstantBuffer;

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
    };
}
