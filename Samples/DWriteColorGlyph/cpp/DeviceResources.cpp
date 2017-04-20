//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "DeviceResources.h"

#include "DirectXHelper.h"
#include <windows.ui.xaml.media.dxinterop.h>

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;

// Constants used to calculate screen rotations
namespace ScreenRotation
{
    // 0-degree Z-rotation
    static const XMFLOAT4X4 Rotation0(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    // 90-degree Z-rotation
    static const XMFLOAT4X4 Rotation90(
        0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    // 180-degree Z-rotation
    static const XMFLOAT4X4 Rotation180(
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    // 270-degree Z-rotation
    static const XMFLOAT4X4 Rotation270(
        0.0f, -1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );
};

// Constructor for DeviceResources.
DX::DeviceResources::DeviceResources() :
    m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
    m_d3dRenderTargetSize(),
    m_outputSize(),
    m_logicalSize(),
    m_nativeOrientation(DisplayOrientations::None),
    m_currentOrientation(DisplayOrientations::None),
    m_dpi(-1.0f),
    m_deviceNotify(nullptr)
{
    CreateDeviceIndependentResources();
    CreateDeviceResources();
}

// Configures resources that don't depend on the Direct3D device.
void DX::DeviceResources::CreateDeviceIndependentResources()
{
    // Initialize Direct2D resources.
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
    // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    // Initialize the Direct2D Factory.
    DX::ThrowIfFailed(
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory5),
            &options,
            &m_d2dFactory
            )
        );

    // Initialize the DirectWrite Factory.
    DX::ThrowIfFailed(
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory4),
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
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Create the Direct3D 11 API device object and a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    HRESULT hr = D3D11CreateDevice(
        nullptr,                    // Specify nullptr to use the default adapter.
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
                nullptr,
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

    // Store pointers to the Direct3D 11.1 API device and immediate context.
    DX::ThrowIfFailed(
        device.As(&m_d3dDevice)
        );

    DX::ThrowIfFailed(
        context.As(&m_d3dContext)
        );

    // Create the Direct2D device object and a corresponding context.
    ComPtr<IDXGIDevice3> dxgiDevice;
    DX::ThrowIfFailed(
        m_d3dDevice.As(&dxgiDevice)
        );

    DX::ThrowIfFailed(
        m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)
        );

    DX::ThrowIfFailed(
        m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dContext
            )
        );
}

// These resources need to be recreated every time the window size is changed.
void DX::DeviceResources::CreateWindowSizeDependentResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    m_d2dContext->SetTarget(nullptr);
    m_d2dTargetBitmap = nullptr;
    m_d3dContext->Flush();

    // Calculate the necessary render target size in pixels.
    m_outputSize.Width = DX::ConvertDipsToPixels(m_logicalSize.Width, m_dpi);
    m_outputSize.Height = DX::ConvertDipsToPixels(m_logicalSize.Height, m_dpi);

    // Prevent zero size DirectX content from being created.
    m_outputSize.Width = max(m_outputSize.Width, 1);
    m_outputSize.Height = max(m_outputSize.Height, 1);

    // The width and height of the swap chain must be based on the window's
    // natively-oriented width and height. If the window is not in the native
    // orientation, the dimensions must be reversed.
    DXGI_MODE_ROTATION displayRotation = ComputeDisplayRotation();

    bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
    m_d3dRenderTargetSize.Width = swapDimensions ? m_outputSize.Height : m_outputSize.Width;
    m_d3dRenderTargetSize.Height = swapDimensions ? m_outputSize.Width : m_outputSize.Height;

    if (m_swapChain != nullptr)
    {
        // If the swap chain already exists, resize it.
        HRESULT hr = m_swapChain->ResizeBuffers(
            2, // Double-buffered swap chain.
            static_cast<UINT>(m_d3dRenderTargetSize.Width),
            static_cast<UINT>(m_d3dRenderTargetSize.Height),
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0
            );

        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            HandleDeviceLost();

            // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // Otherwise, create a new one using the same adapter as the existing Direct3D device.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};

        swapChainDesc.Width = static_cast<UINT>(m_d3dRenderTargetSize.Width); // Match the size of the window.
        swapChainDesc.Height = static_cast<UINT>(m_d3dRenderTargetSize.Height);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
        swapChainDesc.Flags = 0;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE; // When using XAML interop, this value cannot be DXGI_ALPHA_MODE_PREMULTIPLIED.
        swapChainDesc.Scaling = DXGI_SCALING_NONE;

        // This sequence obtains the DXGI factory that was used to create the Direct3D device above.
        ComPtr<IDXGIDevice3> dxgiDevice;
        DX::ThrowIfFailed(
            m_d3dDevice.As(&dxgiDevice)
            );

        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(
            dxgiDevice->GetAdapter(&dxgiAdapter)
            );

        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(
            dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
            );

        DX::ThrowIfFailed(
            dxgiFactory->CreateSwapChainForCoreWindow(
                m_d3dDevice.Get(),
                reinterpret_cast<IUnknown*>(m_window.Get()),
                &swapChainDesc,
                nullptr,
                &m_swapChain
                )
            );

        // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
        // ensures that the application will only render after each VSync, minimizing power consumption.
        DX::ThrowIfFailed(
            dxgiDevice->SetMaximumFrameLatency(1)
            );
    }

    // Set the proper orientation for the swap chain, and generate 2D and
    // 3D matrix transformations for rendering to the rotated swap chain.
    // Note the rotation angle for the 2D and 3D transforms are different.
    // This is due to the difference in coordinate spaces.  Additionally,
    // the 3D matrix is specified explicitly to avoid rounding errors.

    switch (displayRotation)
    {
    case DXGI_MODE_ROTATION_IDENTITY:
        m_orientationTransform2D = Matrix3x2F::Identity();
        m_orientationTransform3D = ScreenRotation::Rotation0;
        break;

    case DXGI_MODE_ROTATION_ROTATE90:
        m_orientationTransform2D =
            Matrix3x2F::Rotation(90.0f) *
            Matrix3x2F::Translation(m_logicalSize.Height, 0.0f);
        m_orientationTransform3D = ScreenRotation::Rotation270;
        break;

    case DXGI_MODE_ROTATION_ROTATE180:
        m_orientationTransform2D =
            Matrix3x2F::Rotation(180.0f) *
            Matrix3x2F::Translation(m_logicalSize.Width, m_logicalSize.Height);
        m_orientationTransform3D = ScreenRotation::Rotation180;
        break;

    case DXGI_MODE_ROTATION_ROTATE270:
        m_orientationTransform2D =
            Matrix3x2F::Rotation(270.0f) *
            Matrix3x2F::Translation(0.0f, m_logicalSize.Width);
        m_orientationTransform3D = ScreenRotation::Rotation90;
        break;

    default:
        throw ref new FailureException();
    }

    DX::ThrowIfFailed(
        m_swapChain->SetRotation(displayRotation)
        );

    // Create a Direct2D target bitmap associated with the
    // swap chain back buffer and set it as the current target.
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            m_dpi,
            m_dpi
            );

    ComPtr<IDXGISurface2> dxgiBackBuffer;
    DX::ThrowIfFailed(
        m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
        );

    DX::ThrowIfFailed(
        m_d2dContext->CreateBitmapFromDxgiSurface(
            dxgiBackBuffer.Get(),
            &bitmapProperties,
            &m_d2dTargetBitmap
            )
        );

    m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());

    // Grayscale text anti-aliasing is recommended for all Windows Store apps.
    m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

// This method is called when the CoreWindow is created (or re-created).
void DX::DeviceResources::SetWindow(CoreWindow^ window)
{
    m_window = window;

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    m_logicalSize = Windows::Foundation::Size(window->Bounds.Width, window->Bounds.Height);
    m_nativeOrientation = currentDisplayInformation->NativeOrientation;
    m_currentOrientation = currentDisplayInformation->CurrentOrientation;
    m_dpi = currentDisplayInformation->LogicalDpi;

    m_d2dContext->SetDpi(m_dpi, m_dpi);
    CreateWindowSizeDependentResources();
}

// This method is called in the event handler for the SizeChanged event.
void DX::DeviceResources::SetLogicalSize(Windows::Foundation::Size logicalSize)
{
    if (m_logicalSize != logicalSize)
    {
        m_logicalSize = logicalSize;
        CreateWindowSizeDependentResources();
    }
}

// This method is called in the event handler for the DpiChanged event.
void DX::DeviceResources::SetDpi(float dpi)
{
    if (dpi != m_dpi)
    {
        m_dpi = dpi;

        // When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
        m_logicalSize = Windows::Foundation::Size(m_window->Bounds.Width, m_window->Bounds.Height);

        m_d2dContext->SetDpi(m_dpi, m_dpi);
        CreateWindowSizeDependentResources();
    }
}

// This method is called in the event handler for the OrientationChanged event.
void DX::DeviceResources::SetCurrentOrientation(DisplayOrientations currentOrientation)
{
    if (m_currentOrientation != currentOrientation)
    {
        m_currentOrientation = currentOrientation;
        CreateWindowSizeDependentResources();
    }
}

// This method is called in the event handler for the DisplayContentsInvalidated event.
void DX::DeviceResources::ValidateDevice()
{
    // The D3D Device is no longer valid if the default adapter changed since the device
    // was created or if the device has been removed.

    // First, get the information for the default adapter from when the device was created.

    ComPtr<IDXGIDevice3> dxgiDevice;
    DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

    ComPtr<IDXGIAdapter> deviceAdapter;
    DX::ThrowIfFailed(dxgiDevice->GetAdapter(&deviceAdapter));

    ComPtr<IDXGIFactory2> deviceFactory;
    DX::ThrowIfFailed(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));

    ComPtr<IDXGIAdapter1> previousDefaultAdapter;
    DX::ThrowIfFailed(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));

    DXGI_ADAPTER_DESC previousDesc;
    DX::ThrowIfFailed(previousDefaultAdapter->GetDesc(&previousDesc));

    // Next, get the information for the current default adapter.

    ComPtr<IDXGIFactory2> currentFactory;
    DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

    ComPtr<IDXGIAdapter1> currentDefaultAdapter;
    DX::ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

    DXGI_ADAPTER_DESC currentDesc;
    DX::ThrowIfFailed(currentDefaultAdapter->GetDesc(&currentDesc));

    // If the adapter LUIDs don't match, or if the device reports that it has been removed,
    // a new D3D device must be created.

    if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
        previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
        FAILED(m_d3dDevice->GetDeviceRemovedReason()))
    {
        // Release references to resources related to the old device.
        dxgiDevice = nullptr;
        deviceAdapter = nullptr;
        deviceFactory = nullptr;
        previousDefaultAdapter = nullptr;

        // Create a new device and swap chain.
        HandleDeviceLost();
    }
}

// Recreate all device resources and set them back to the current state.
void DX::DeviceResources::HandleDeviceLost()
{
    m_swapChain = nullptr;

    if (m_deviceNotify != nullptr)
    {
        // Notify the renderers that device resources need to be released.
        // This ensures all references to the existing swap chain are released so that a new one can be created.
        m_deviceNotify->OnDeviceLost();
    }

    // Create the new device and swap chain.
    CreateDeviceResources();
    m_d2dContext->SetDpi(m_dpi, m_dpi);
    CreateWindowSizeDependentResources();

    if (m_deviceNotify != nullptr)
    {
        // Notify the renderers that resources can now be created again.
        m_deviceNotify->OnDeviceRestored();
    }
}

// Register our DeviceNotify to be informed on device lost and creation.
void DX::DeviceResources::RegisterDeviceNotify(DX::IDeviceNotify *deviceNotify)
{
    m_deviceNotify = deviceNotify;
}

// Call this method when the app suspends. It provides a hint to the driver that the app 
// is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
void DX::DeviceResources::Trim()
{
    ComPtr<IDXGIDevice3> dxgiDevice;
    m_d3dDevice.As(&dxgiDevice);

    dxgiDevice->Trim();
}

// Present the contents of the swap chain to the screen.
void DX::DeviceResources::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was removed either by a disconnection or a driver upgrade, we
    // must recreate all device resources.
    if (hr == DXGI_ERROR_DEVICE_REMOVED)
    {
        HandleDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// This method determines the rotation between the display device's native Orientation and the
// current display orientation.
DXGI_MODE_ROTATION DX::DeviceResources::ComputeDisplayRotation()
{
    DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

    // Note: NativeOrientation can only be Landscape or Portrait even though
    // the DisplayOrientations enum has other values.
    switch (m_nativeOrientation)
    {
    case DisplayOrientations::Landscape:
        switch (m_currentOrientation)
        {
        case DisplayOrientations::Landscape:
            rotation = DXGI_MODE_ROTATION_IDENTITY;
            break;

        case DisplayOrientations::Portrait:
            rotation = DXGI_MODE_ROTATION_ROTATE270;
            break;

        case DisplayOrientations::LandscapeFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE180;
            break;

        case DisplayOrientations::PortraitFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE90;
            break;
        }
        break;

    case DisplayOrientations::Portrait:
        switch (m_currentOrientation)
        {
        case DisplayOrientations::Landscape:
            rotation = DXGI_MODE_ROTATION_ROTATE90;
            break;

        case DisplayOrientations::Portrait:
            rotation = DXGI_MODE_ROTATION_IDENTITY;
            break;

        case DisplayOrientations::LandscapeFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE270;
            break;

        case DisplayOrientations::PortraitFlipped:
            rotation = DXGI_MODE_ROTATION_ROTATE180;
            break;
        }
        break;
    }
    return rotation;
}
