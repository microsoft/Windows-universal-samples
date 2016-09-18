
#include "pch.h"
#include "DeviceResources.h"
#include "DirectXHelper.h"

#include <Collection.h>
#include <windows.graphics.directx.direct3d11.interop.h>

using namespace D2D1;
using namespace Microsoft::WRL;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Holographic;

static const std::wstring s_VertexShaderFiles[DX::VertexShader_Max] = 
{
    L"ms-appx:///VertexShader.cso",
    L"ms-appx:///VprtVertexShader.cso",
    L"ms-appx:///VertexShaderTexture.cso",
    L"ms-appx:///VprtVertexShaderTexture.cso"
};

static const std::wstring s_PixelShaderFiles[DX::PixelShader_Max] = 
{
    L"ms-appx:///PixelShader.cso",
    L"ms-appx:///PixelShaderCursor.cso",
    L"ms-appx:///PixelShaderTexture.cso",
};

static const std::wstring s_GeometryShaderFiles[DX::GeometryShader_Max] = 
{
    L"ms-appx:///GeometryShader.cso",
    L"ms-appx:///GeometryShaderTexture.cso"
};

static const std::wstring s_TextureFiles[DX::Texture_Max] = 
{
    L"ms-appx:////Assets//Cursor.png",
    L"ms-appx:////Assets//Init.png",
    L"ms-appx:////Assets//Hologram.png",
    L"ms-appx:////Assets//SysAudio.png",
    L"ms-appx:////Assets//Photo.png",
    L"ms-appx:////Assets//Video.png"
};

// Constructor for DeviceResources.
DX::DeviceResources::DeviceResources()
{
    CreateDeviceIndependentResources();
}

// Configures resources that don't depend on the Direct3D device.
void DX::DeviceResources::CreateDeviceIndependentResources()
{
    // Initialize Direct2D resources.
    D2D1_FACTORY_OPTIONS options {};

#if defined(_DEBUG)
    // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    // Initialize the Direct2D Factory.
    DX::ThrowIfFailed(
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory2),
            &options,
            &m_d2dFactory
            )
        );

    // Initialize the DirectWrite Factory.
    DX::ThrowIfFailed(
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory2),
            &m_dwriteFactory
            )
        );

    // Initialize the Windows Imaging Component (WIC) Factory.
    DX::ThrowIfFailed(
        CoCreateInstance(
            CLSID_WICImagingFactory2,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_wicFactory)
            )
        );
}

Concurrency::task<void> DX::DeviceResources::SetHolographicSpace(HolographicSpace^ holographicSpace)
{
    // Cache the holographic space. Used to re-initalize during device-lost scenarios.
    m_holographicSpace = holographicSpace;

    InitializeUsingHolographicSpace();

    return LoadShaders();
}

void DX::DeviceResources::InitializeUsingHolographicSpace()
{
    // The holographic space might need to determine which adapter supports
    // holograms, in which case it will specify a non-zero PrimaryAdapterId.
    LUID id =
    {
        m_holographicSpace->PrimaryAdapterId.LowPart,
        m_holographicSpace->PrimaryAdapterId.HighPart
    };

    // When a primary adapter ID is given to the app, the app should find
    // the corresponding DXGI adapter and use it to create Direct3D devices
    // and device contexts. Otherwise, there is no restriction on the DXGI
    // adapter the app can use.
    if ((id.HighPart != 0) && (id.LowPart != 0))
    {
        UINT createFlags = 0;
#ifdef DEBUG
        if (DX::SdkLayersAvailable())
        {
            createFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif
        // Create the DXGI factory.
        ComPtr<IDXGIFactory1> dxgiFactory;
        DX::ThrowIfFailed(
            CreateDXGIFactory2(
                createFlags,
                IID_PPV_ARGS(&dxgiFactory)
                )
            );
        ComPtr<IDXGIFactory4> dxgiFactory4;
        DX::ThrowIfFailed(dxgiFactory.As(&dxgiFactory4));

        // Retrieve the adapter specified by the holographic space.
        DX::ThrowIfFailed(
            dxgiFactory4->EnumAdapterByLuid(
                id,
                IID_PPV_ARGS(&m_dxgiAdapter)
                )
            );
    }
    else
    {
        m_dxgiAdapter.Reset();
    }

    CreateDeviceResources();

    m_holographicSpace->SetDirect3D11Device(m_d3dInteropDevice);
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DX::DeviceResources::CreateDeviceResources()
{
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    if (DX::SdkLayersAvailable())
    {
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
#endif

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Note that HoloLens supports feature level 11.1. The HoloLens emulator is also capable
    // of running on graphics cards starting with feature level 10.0.
    D3D_FEATURE_LEVEL featureLevels [] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    // Create the Direct3D 11 API device object and a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    const HRESULT hr = D3D11CreateDevice(
        m_dxgiAdapter.Get(),        // Either nullptr, or the primary adapter determined by Windows Holographic.
        D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
        0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags,              // Set debug and Direct2D compatibility flags.
        featureLevels,              // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),   // Size of the list above.
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        &device,                    // Returns the Direct3D device created.
        &m_d3dFeatureLevel,         // Returns feature level of device created.
        &context                    // Returns the device immediate context.
        );

    if (FAILED(hr))
    {
        // If the initialization fails, fall back to the WARP device.
        // For more information on WARP, see:
        // http://go.microsoft.com/fwlink/?LinkId=286690
        DX::ThrowIfFailed(
            D3D11CreateDevice(
                nullptr,              // Use the default DXGI adapter for WARP.
                D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
                0,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &device,
                &m_d3dFeatureLevel,
                &context
                )
            );
    }

    // Store pointers to the Direct3D device and immediate context.
    DX::ThrowIfFailed(
        device.As(&m_d3dDevice)
        );

    DX::ThrowIfFailed(
        context.As(&m_d3dContext)
        );

    // Acquire the DXGI interface for the Direct3D device.
    ComPtr<IDXGIDevice3> dxgiDevice;
    DX::ThrowIfFailed(
        m_d3dDevice.As(&dxgiDevice)
        );

    // Wrap the native device using a WinRT interop object.
    m_d3dInteropDevice = CreateDirect3DDevice(dxgiDevice.Get());

    // Cache the DXGI adapter.
    // This is for the case of no preferred DXGI adapter, or fallback to WARP.
    ComPtr<IDXGIAdapter> dxgiAdapter;
    DX::ThrowIfFailed(
        dxgiDevice->GetAdapter(&dxgiAdapter)
        );
    DX::ThrowIfFailed(
        dxgiAdapter.As(&m_dxgiAdapter)
        );

    // Check for device support for the optional feature that allows setting the render target array index from the vertex shader stage.
    D3D11_FEATURE_DATA_D3D11_OPTIONS3 options;
    m_d3dDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &options, sizeof(options));
    if (options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer)
    {
        m_supportsVprt = true;
    }
}

// Validates the back buffer for each HolographicCamera and recreates
// resources for back buffers that have changed.
// Locks the set of holographic camera resources until the function exits.
void DX::DeviceResources::EnsureCameraResources(
    HolographicFrame^ frame,
    HolographicFramePrediction^ prediction)
{
    UseHolographicCameraResources<void>([this, frame, prediction](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap)
    {
        for (HolographicCameraPose^ pose : prediction->CameraPoses)
        {
            HolographicCameraRenderingParameters^ renderingParameters = frame->GetRenderingParameters(pose);
            CameraResources* pCameraResources = cameraResourceMap[pose->HolographicCamera->Id].get();

            pCameraResources->CreateResourcesForBackBuffer(this, renderingParameters);
        }
    });
}

// Prepares to allocate resources and adds resource views for a camera.
// Locks the set of holographic camera resources until the function exits.
void DX::DeviceResources::AddHolographicCamera(HolographicCamera^ camera)
{
    UseHolographicCameraResources<void>([this, camera](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap)
    {
        cameraResourceMap[camera->Id] = std::make_unique<CameraResources>(camera);
    });
}

// Deallocates resources for a camera and removes the camera from the set.
// Locks the set of holographic camera resources until the function exits.
void DX::DeviceResources::RemoveHolographicCamera(HolographicCamera^ camera)
{
    UseHolographicCameraResources<void>([this, camera](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap)
    {
        CameraResources* pCameraResources = cameraResourceMap[camera->Id].get();

        if (pCameraResources != nullptr)
        {
            pCameraResources->ReleaseResourcesForBackBuffer(this);
            cameraResourceMap.erase(camera->Id);
        }
    });
}

// Recreate all device resources and set them back to the current state.
// Locks the set of holographic camera resources until the function exits.
void DX::DeviceResources::HandleDeviceLost()
{
    if (m_deviceNotify != nullptr)
    {
        m_deviceNotify->OnDeviceLost();
    }

    UseHolographicCameraResources<void>([this](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap)
    {
        for (auto& pair : cameraResourceMap)
        {
            CameraResources* pCameraResources = pair.second.get();
            pCameraResources->ReleaseResourcesForBackBuffer(this);
        }
    });

    InitializeUsingHolographicSpace();

    LoadShaders().then([this]()
    {
        if (m_deviceNotify != nullptr)
        {
            m_deviceNotify->OnDeviceRestored();
        }
    });
}

// Register our DeviceNotify to be informed on device lost and creation.
void DX::DeviceResources::RegisterDeviceNotify(DX::IDeviceNotify* deviceNotify)
{
    m_deviceNotify = deviceNotify;
}

// Call this method when the app suspends. It provides a hint to the driver that the app
// is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
void DX::DeviceResources::Trim()
{
    m_d3dContext->ClearState();

    ComPtr<IDXGIDevice3> dxgiDevice;
    DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));
    dxgiDevice->Trim();
}

// Present the contents of the swap chain to the screen.
// Locks the set of holographic camera resources until the function exits.
void DX::DeviceResources::Present(HolographicFrame^ frame)
{
    // By default, this API waits for the frame to finish before it returns.
    // Holographic apps should wait for the previous frame to finish before
    // starting work on a new frame. This allows for better results from
    // holographic frame predictions.
    HolographicFramePresentResult presentResult = frame->PresentUsingCurrentPrediction();

    HolographicFramePrediction^ prediction = frame->CurrentPrediction;
    UseHolographicCameraResources<void>([this, prediction](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap)
    {
        for (auto cameraPose : prediction->CameraPoses)
        {
            // This represents the device-based resources for a HolographicCamera.
            DX::CameraResources* pCameraResources = cameraResourceMap[cameraPose->HolographicCamera->Id].get();

            // Discard the contents of the render target.
            // This is a valid operation only when the existing contents will be
            // entirely overwritten. If dirty or scroll rects are used, this call
            // should be removed.
            m_d3dContext->DiscardView(pCameraResources->GetBackBufferRenderTargetView());

            // Discard the contents of the depth stencil.
            m_d3dContext->DiscardView(pCameraResources->GetDepthStencilView());
        }
    });

    // The PresentUsingCurrentPrediction API will detect when the graphics device
    // changes or becomes invalid. When this happens, it is considered a Direct3D
    // device lost scenario.
    if (presentResult == HolographicFramePresentResult::DeviceRemoved)
    {
        // The Direct3D device, context, and resources should be recreated.
        HandleDeviceLost();
    }
}

Concurrency::task<void> DX::DeviceResources::LoadShaders()
{
    std::vector<Concurrency::task<void>> shaderTaskGroup;

    // Load vertex shaders and input layouts
    for (int i = 0; i < VertexShader_Max; i++)
    {
        Concurrency::task<std::vector<byte>> loadVSTask = DX::ReadDataAsync(s_VertexShaderFiles[i]);

        Concurrency::task<void> createVSTask = loadVSTask.then([this, i](const std::vector<byte>& fileData)
        {
            DX::ThrowIfFailed(
                m_d3dDevice->CreateVertexShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    m_vertexShaders[i].ReleaseAndGetAddressOf()));

            switch (i)
            {
            case VertexShader_Simple:
            case VertexShader_VPRT:
                {
                    const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
                    {
                        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    };

                    DX::ThrowIfFailed(
                        m_d3dDevice->CreateInputLayout(
                            vertexDesc,
                            ARRAYSIZE(vertexDesc),
                            &fileData[0],
                            fileData.size(),
                            m_inputLayouts[i].ReleaseAndGetAddressOf()));
                }
                break;
            case VertexShader_Texture:
            case VertexShader_TextureVPRT:
                {
                    const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
                    {
                        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    };

                    DX::ThrowIfFailed(
                        m_d3dDevice->CreateInputLayout(
                            vertexDesc,
                            ARRAYSIZE(vertexDesc),
                            &fileData[0],
                            fileData.size(),
                            m_inputLayouts[i].ReleaseAndGetAddressOf()));
                }
                break;
            }
        });

        shaderTaskGroup.push_back(createVSTask);
    }

    // Load pixel shaders
    for (int i = 0; i < PixelShader_Max; i++)
    {
        Concurrency::task<std::vector<byte>> loadPSTask = DX::ReadDataAsync(s_PixelShaderFiles[i]);

        Concurrency::task<void> createPSTask = loadPSTask.then([this, i](const std::vector<byte>& fileData)
        {
            DX::ThrowIfFailed(
                m_d3dDevice->CreatePixelShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    m_pixelShaders[i].ReleaseAndGetAddressOf()));
        });

        shaderTaskGroup.push_back(createPSTask);
    }

    // Load geometry shaders
    for (int i = 0; i < GeometryShader_Max; i++)
    {
        Concurrency::task<std::vector<byte>> loadGSTask = DX::ReadDataAsync(s_GeometryShaderFiles[i]);

        Concurrency::task<void> createGSTask = loadGSTask.then([this, i](const std::vector<byte>& fileData)
        {
            DX::ThrowIfFailed(
                m_d3dDevice->CreateGeometryShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    m_geometryShaders[i].ReleaseAndGetAddressOf()));
        });

        shaderTaskGroup.push_back(createGSTask);
    }

    // Load textures
    for (int i = 0; i < Texture_Max; i++)
    {
        Concurrency::task<std::vector<byte>> loadTextureTask = DX::ReadDataAsync(s_TextureFiles[i]);

        Concurrency::task<void> createTextureTask = loadTextureTask.then([this, i](const std::vector<byte>& fileData)
        {
            DX::ThrowIfFailed(
                DX::CreateWICTextureFromMemory(
                    m_d3dDevice.Get(),
                    &fileData[0],
                    fileData.size(),
                    m_textures[i].ReleaseAndGetAddressOf(),
                    nullptr));
        });

        shaderTaskGroup.push_back(createTextureTask);
    }

    return when_all(std::begin(shaderTaskGroup), std::end(shaderTaskGroup));
}
