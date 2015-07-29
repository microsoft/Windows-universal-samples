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
#include "LowLatencyInputMain.h"
#include "Common\DirectXHelper.h"
#include "Content\RectRenderer.h"

using namespace SDKTemplate;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
LowLatencyInputMain::LowLatencyInputMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    deviceResources(deviceResources)
{
    // Register to be notified if the Device is lost or recreated
    deviceResources->RegisterDeviceNotify(this);

    sceneRenderer = std::unique_ptr<RectRenderer>(new RectRenderer(deviceResources));
}

LowLatencyInputMain::~LowLatencyInputMain()
{
    // Deregister device notification
    deviceResources->RegisterDeviceNotify(nullptr);
}

void LowLatencyInputMain::StartRenderLoop()
{
    // If the animation render loop is already running then do not start another thread.
    if (renderLoopWorker != nullptr && renderLoopWorker->Status == AsyncStatus::Started)
    {
        return;
    }

    // Create a task that will be run on a background thread.
    auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
    {
        // Calculate the updated frame and render once per vertical blanking interval.
        while (action->Status == AsyncStatus::Started)
        {
            critical_section::scoped_lock lock(criticalSection);

            // Block this thread until the swap chain is finished presenting. Note that it is
            // important to call this before the first Present in order to minimize the latency
            // of the swap chain
            deviceResources->WaitOnSwapChain();
            Render();
            deviceResources->Present();
        }
    });

    // Run task on a dedicated high priority background thread.
    renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void LowLatencyInputMain::StopRenderLoop()
{
    renderLoopWorker->Cancel();
}

// Renders the current frame according to the current application state.
void LowLatencyInputMain::Render() 
{
    auto context = deviceResources->GetD3DDeviceContext();

    // Reset the viewport to target the whole screen.
    auto viewport = deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    // Reset render targets to the screen.
    ID3D11RenderTargetView *const targets[1] = { deviceResources->GetBackBufferRenderTargetView() };
    context->OMSetRenderTargets(1, targets, deviceResources->GetDepthStencilView());

    // Clear the back buffer and depth stencil view.
    context->ClearRenderTargetView(deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
    context->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Render the scene objects.
    sceneRenderer->Render();
}

// Notifies renderers that device resources need to be released.
void LowLatencyInputMain::OnDeviceLost()
{
    sceneRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void LowLatencyInputMain::OnDeviceRestored()
{
    sceneRenderer->CreateDeviceDependentResources();
}

//Passes rect data to renderer class to transform rect
void LowLatencyInputMain::UpdateRectTransform(Point newPosition, Windows::UI::Input::ManipulationDelta delta)
{
    sceneRenderer->UpdateRectTransform(newPosition, delta);
}

//Returns whether the pointer event was within the bounds of the rect 
bool LowLatencyInputMain::HitTest( Windows::Foundation::Point position)
{
    return sceneRenderer->HitTest(position);
}

void LowLatencyInputMain::SetRectSize(float newSize)
{
    sceneRenderer->SetRectSize(newSize);
}

void LowLatencyInputMain::ResetRect()
{
    sceneRenderer->ResetRect();
}

void LowLatencyInputMain::SetBrushColor(D2D1::ColorF color)
{
    sceneRenderer->SetBrushColor(color);
}