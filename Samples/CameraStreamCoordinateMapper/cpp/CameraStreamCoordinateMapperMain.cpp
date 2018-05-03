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
#include "CameraStreamCoordinateMapperMain.h"

#include "Common\DirectXHelper.h"
#include "Common\GraphicsCamera.h"

#include "Content\QuadRenderer.h"
#include "Content\TextRenderer.h"
#include "Content\CameraCoordinateMapper.h"

using namespace CameraStreamCoordinateMapper;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::Storage;
using namespace Windows::System::Threading;

using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

using namespace concurrency;
using namespace DirectX;

CameraStreamCoordinateMapperMain::CameraStreamCoordinateMapperMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    m_camera = ref new FirstPersonCamera();

    m_gpuPerformanceTimer = std::make_unique<DX::GpuPerformanceTimer>(m_deviceResources);
    m_quadRenderer = std::make_unique<QuadRenderer>(m_deviceResources);
    m_statisticsTextRenderer = std::make_unique<TextRenderer>(m_deviceResources);
    m_helpTextRenderer = std::make_unique<TextRenderer>(m_deviceResources);

    static constexpr const wchar_t* c_HelpTextDisabledStr = 
        L"'F1' or 'Gamepad Menu' to toggle help \n";

    static constexpr const wchar_t* c_HelpTextEnabledStr =
        L"'F1' or 'Gamepad Menu' to toggle help \n"
        L"'Space' or 'Gamepad A' to toggle streaming \n"
        L"'R' or 'Left Thumbstick' to reset the camera \n"
        L"'Y' or 'Gamepad Y' to toggle camera mode \n"
        L"'W/A/S/D/Q/E' to translate the free-roam camera \n"
        L"'Left thumbstick' to translate the free-roam camera \n"
        L"'Left mouse button' to rotate the free-roam camera \n"
        L"'Right thumbstick' to rotate the free-roam camera \n";

    m_helpTextRenderer->Update(c_HelpTextDisabledStr);

    CoreWindow::GetForCurrentThread()->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>([this](CoreWindow^ window, KeyEventArgs^ args)
    {
        if (args->VirtualKey == Windows::System::VirtualKey::Space || 
            args->VirtualKey == Windows::System::VirtualKey::GamepadA)
        {
            m_streamingEnabled = !m_streamingEnabled;
        }

        if (args->VirtualKey == Windows::System::VirtualKey::R ||
            args->VirtualKey == Windows::System::VirtualKey::GamepadLeftThumbstickButton)
        {
            m_camera->Reset();
        }

        if (args->VirtualKey == Windows::System::VirtualKey::Y ||
            args->VirtualKey == Windows::System::VirtualKey::GamepadY)
        {
            m_freeRoamCameraEnabled = !m_freeRoamCameraEnabled;
            m_camera->Reset();
        }

        if (args->VirtualKey == Windows::System::VirtualKey::F1 ||
            args->VirtualKey == Windows::System::VirtualKey::GamepadMenu)
        {
            m_showHelpText = !m_showHelpText;

            if (m_showHelpText)
            {
                m_helpTextRenderer->Update(c_HelpTextEnabledStr);
            }
            else
            {
                m_helpTextRenderer->Update(c_HelpTextDisabledStr);
            }
        }
    });

    CreateResourcesAsync().then([this]
    {
        m_readyToRender = true;
    });

    // Register to be notified if the Device is lost or recreated
    m_deviceResources->RegisterDeviceNotify(this);
}

task<void> CameraStreamCoordinateMapper::CameraStreamCoordinateMapperMain::CreateResourcesAsync()
{
    m_cameraCoordinateMapper = co_await CameraCoordinateMapper::CreateAndStartAsync(m_deviceResources);
}

CameraStreamCoordinateMapperMain::~CameraStreamCoordinateMapperMain()
{
    // Deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void CameraStreamCoordinateMapperMain::CreateWindowSizeDependentResources() 
{
    m_quadRenderer->CreateWindowSizeDependentResources();
}

// Updates the application state once per frame.
void CameraStreamCoordinateMapperMain::Update() 
{
    if (!m_readyToRender) 
    {
        return;
    }

    // Update scene objects.
    m_timer.Tick([&]()
    {
        m_camera->Update(static_cast<float>(m_timer.GetElapsedSeconds()));

        if (m_cameraCoordinateMapper->AreCamerasStreaming())
        {
            static wchar_t buffer[256] = {};

            const wchar_t* cameraModeStr = m_freeRoamCameraEnabled ? L"[Free-roam camera]" : L"[Target camera]";
            const wchar_t* streamingModeStr = m_streamingEnabled ? L"[Streaming]" : L"[Paused]";
            const float gpuTime = m_gpuPerformanceTimer->GetAvgFrameTime();

            swprintf_s(buffer, L"%s %s [Gpu time: %.3fms] [FPS: %.4u]", streamingModeStr, cameraModeStr, gpuTime, m_timer.GetFramesPerSecond());

            m_statisticsTextRenderer->Update(buffer);
        }
        else
        {
            m_statisticsTextRenderer->Update(L"No compatible/accessible was camera found\nEnsure the application has permission to access the camera");
        }
    });

    if (!m_streamingEnabled) 
    {
        return;
    }

    if (m_cameraCoordinateMapper->TryAcquireLatestFrameData())
    {
        m_gpuPerformanceTimer->StartTimerForFrame();

        m_cameraCoordinateMapper->RunCoordinateMapping();

        m_gpuPerformanceTimer->EndTimerForFrame();
    }
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool CameraStreamCoordinateMapperMain::Render() 
{
    if (!m_readyToRender) 
    {
        return false;
    }

    if (m_freeRoamCameraEnabled && m_cameraCoordinateMapper->AreCamerasStreaming())
    {
        m_cameraCoordinateMapper->VisualizeCoordinateMapping(
            m_deviceResources->GetBackBufferRenderTargetView(),
            m_deviceResources->GetDepthStencilView(),
            m_deviceResources->GetScreenViewport(),
            m_camera->GetWorldToCamera());
    }
    else
    {
        ID3D11DeviceContext* context = m_deviceResources->GetD3DDeviceContext();

        // Reset the viewport to target the whole screen.
        const D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
        context->RSSetViewports(1, &viewport);

        // Reset render targets to the screen.
        ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
        context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

        // Clear the back imageBuffer and depth stencil view.
        context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
        context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        if (m_cameraCoordinateMapper->AreCamerasStreaming())
        {
            ID3D11ShaderResourceView* targetTexture = m_cameraCoordinateMapper->GetTargetShaderResourceView();
            const float targetAspectRatio = m_cameraCoordinateMapper->GetTargetAspectRatio();

            // Render the resulting target image as a quad
            if (targetTexture)
            {
                m_quadRenderer->Render(targetTexture, targetAspectRatio);
            }
        }
    }

    m_statisticsTextRenderer->Render(TextRenderer::TextAlignment::BottomRight);
    m_helpTextRenderer->Render(TextRenderer::TextAlignment::TopLeft);

    return true;
}

task<void> CameraStreamCoordinateMapperMain::OnSuspendingAsync()
{
    m_readyToRender = false;
    return m_cameraCoordinateMapper->StopAsync();
}

void CameraStreamCoordinateMapperMain::OnResuming()
{
    m_cameraCoordinateMapper->StartAsync().then([this] 
    {
        m_readyToRender = true;
    });
}

// Notifies renderers that device resources need to be released.
void CameraStreamCoordinateMapperMain::OnDeviceLost()
{
    m_readyToRender = false;

    m_quadRenderer->ReleaseDeviceDependentResources();
    m_statisticsTextRenderer->ReleaseDeviceDependentResources();
    m_helpTextRenderer->ReleaseDeviceDependentResources();
    m_cameraCoordinateMapper->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void CameraStreamCoordinateMapperMain::OnDeviceRestored()
{
    m_cameraCoordinateMapper->CreateDeviceDependentResources();
    m_helpTextRenderer->CreateDeviceDependentResources();
    m_statisticsTextRenderer->CreateDeviceDependentResources();
    m_quadRenderer->CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();

    m_readyToRender = true;
}
