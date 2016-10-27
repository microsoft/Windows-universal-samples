
#pragma once

#include "CameraResources.h"
#include <ppltasks.h>

namespace DX
{
    typedef enum
    {
        VertexShader_Simple = 0,
        VertexShader_VPRT,
        VertexShader_Texture,
        VertexShader_TextureVPRT,
        VertexShader_Max
    } VertexShaderIndex;

    typedef enum
    {
        PixelShader_Simple = 0,
        PixelShader_Cursor,
        PixelShader_Texture,
        PixelShader_Max
    } PixelShaderIndex;

    typedef enum
    {
        GeometryShader_Simple = 0,
        GeometryShader_Texture,
        GeometryShader_Max
    } GeometryShaderIndex;

    typedef enum
    {
        Texture_Cursor = 0,
        Texture_Init,
        Texture_Hologram,
        Texture_SysAudio,
        Texture_Photo,
        Texture_Video,
        Texture_Max
    } TextureIndex;

    // Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
    interface IDeviceNotify
    {
        virtual void OnDeviceLost()     = 0;
        virtual void OnDeviceRestored() = 0;
    };

    // Creates and manages a Direct3D device and immediate context, Direct2D device and context (for debug), and the holographic swap chain.
    class DeviceResources
    {
    public:
        DeviceResources();

        // Public methods related to Direct3D devices.
        void HandleDeviceLost();
        void RegisterDeviceNotify(IDeviceNotify* deviceNotify);
        void Trim();
        void Present(Windows::Graphics::Holographic::HolographicFrame^ frame);

        // Public methods related to holographic devices.
        Concurrency::task<void> SetHolographicSpace(Windows::Graphics::Holographic::HolographicSpace^ space);
        void EnsureCameraResources(
            Windows::Graphics::Holographic::HolographicFrame^ frame,
            Windows::Graphics::Holographic::HolographicFramePrediction^ prediction);

        void AddHolographicCamera(Windows::Graphics::Holographic::HolographicCamera^ camera);
        void RemoveHolographicCamera(Windows::Graphics::Holographic::HolographicCamera^ camera);

        // Holographic accessors.
        template<typename RetType, typename LCallback>
        RetType                 UseHolographicCameraResources(const LCallback& callback);

        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice^
                                GetD3DInteropDevice() const             { return m_d3dInteropDevice;    }

        // D3D accessors.
        ID3D11Device4*          GetD3DDevice() const                    { return m_d3dDevice.Get();     }
        ID3D11DeviceContext3*   GetD3DDeviceContext() const             { return m_d3dContext.Get();    }
        D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const           { return m_d3dFeatureLevel;     }
        bool                    GetDeviceSupportsVprt() const           { return m_supportsVprt;        }

        // DXGI acessors.
        IDXGIAdapter3*          GetDXGIAdapter() const                  { return m_dxgiAdapter.Get();   }

        // D2D accessors.
        ID2D1Factory2*          GetD2DFactory() const                   { return m_d2dFactory.Get();    }
        IDWriteFactory2*        GetDWriteFactory() const                { return m_dwriteFactory.Get(); }
        IWICImagingFactory2*    GetWicImagingFactory() const            { return m_wicFactory.Get();    }

        // Shader accessors.
        ID3D11VertexShader*     GetVertexShader(const VertexShaderIndex index)      { return m_vertexShaders[index].Get(); }
        ID3D11InputLayout*      GetInputLayout(const VertexShaderIndex index)       { return m_inputLayouts[index].Get(); }
        ID3D11PixelShader*      GetPixelShader(const PixelShaderIndex index)        { return m_pixelShaders[index].Get(); }
        ID3D11GeometryShader*   GetGeometryShader(const GeometryShaderIndex index)  { return m_geometryShaders[index].Get(); }
        ID3D11Resource*         GetTexture(const TextureIndex index)                { return m_textures[index].Get(); }

    private:
        // Private methods related to the Direct3D device, and resources based on that device.
        void CreateDeviceIndependentResources();
        void InitializeUsingHolographicSpace();
        void CreateDeviceResources();
        Concurrency::task<void> LoadShaders();

        // Direct3D objects.
        Microsoft::WRL::ComPtr<ID3D11Device4>                   m_d3dDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext3>            m_d3dContext;
        Microsoft::WRL::ComPtr<IDXGIAdapter3>                   m_dxgiAdapter;

        // Direct3D interop objects.
        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice^ m_d3dInteropDevice;

        // Direct2D factories.
        Microsoft::WRL::ComPtr<ID2D1Factory2>                   m_d2dFactory;
        Microsoft::WRL::ComPtr<IDWriteFactory2>                 m_dwriteFactory;
        Microsoft::WRL::ComPtr<IWICImagingFactory2>             m_wicFactory;

        // The holographic space provides a preferred DXGI adapter ID.
        Windows::Graphics::Holographic::HolographicSpace^       m_holographicSpace = nullptr;

        // Properties of the Direct3D device currently in use.
        D3D_FEATURE_LEVEL                                       m_d3dFeatureLevel = D3D_FEATURE_LEVEL_10_0;

        // The IDeviceNotify can be held directly as it owns the DeviceResources.
        IDeviceNotify*                                          m_deviceNotify = nullptr;

        // Whether or not the current Direct3D device supports the optional feature 
        // for setting the render target array index from the vertex shader stage.
        bool                                                    m_supportsVprt = false;

        // Back buffer resources, etc. for attached holographic cameras.
        std::map<UINT32, std::unique_ptr<CameraResources>>      m_cameraResources;

        // Shader resources
        Microsoft::WRL::ComPtr<ID3D11InputLayout>               m_inputLayouts[VertexShader_Max];
        Microsoft::WRL::ComPtr<ID3D11VertexShader>              m_vertexShaders[VertexShader_Max];
        Microsoft::WRL::ComPtr<ID3D11PixelShader>               m_pixelShaders[PixelShader_Max];
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>            m_geometryShaders[GeometryShader_Max];
        Microsoft::WRL::ComPtr<ID3D11Resource>                  m_textures[Texture_Max];

        std::mutex                                              m_cameraResourcesLock;
    };
}

// Device-based resources for holographic cameras are stored in a std::map. Access this list by providing a
// callback to this function, and the std::map will be guarded from add and remove
// events until the callback returns. The callback is processed immediately and must
// not contain any nested calls to UseHolographicCameraResources.
// The callback takes a parameter of type std::map<UINT32, std::unique_ptr<DX::CameraResources>>&
// through which the list of cameras will be accessed.
template<typename RetType, typename LCallback>
RetType DX::DeviceResources::UseHolographicCameraResources(const LCallback& callback)
{
    std::lock_guard<std::mutex> guard(m_cameraResourcesLock);
    return callback(m_cameraResources);
}

