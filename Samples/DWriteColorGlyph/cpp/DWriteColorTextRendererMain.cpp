//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "DWriteColorTextRendererMain.h"

#include <DirectXColors.h>
#include "DirectXHelper.h"

using namespace DWriteColorTextRenderer;

// Loads and initializes application assets when the application is loaded.
DWriteColorTextRendererMain::DWriteColorTextRendererMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the Device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    m_sceneRenderer = std::unique_ptr<DWriteColorTextRendererRenderer>(new DWriteColorTextRendererRenderer(m_deviceResources));
    m_sampleOverlay = std::unique_ptr<SampleOverlay>(new SampleOverlay(m_deviceResources, L"Direct2D color fonts sample"));
}

DWriteColorTextRendererMain::~DWriteColorTextRendererMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void DWriteColorTextRendererMain::UpdateForWindowSizeChange()
{
    m_sceneRenderer->CreateWindowSizeDependentResources();
    m_sampleOverlay->CreateWindowSizeDependentResources();
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool DWriteColorTextRendererMain::Render()
{
    // Render the scene objects.
    m_sceneRenderer->Render();
    m_sampleOverlay->Render();

    return true;
}

// Notifies renderers that device resources need to be released.
void DWriteColorTextRendererMain::OnDeviceLost()
{
    m_sceneRenderer->ReleaseDeviceDependentResources();
    m_sampleOverlay->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be re-created.
void DWriteColorTextRendererMain::OnDeviceRestored()
{
    m_sceneRenderer->CreateDeviceDependentResources();
    m_sampleOverlay->CreateDeviceDependentResources();
    UpdateForWindowSizeChange();
}
