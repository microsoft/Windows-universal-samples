#include "pch.h"
#include "CustomComputeShaderMain.h"
#include "DirectXHelper.h"

using namespace CustomComputeShader;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
CustomComputeShaderMain::CustomComputeShaderMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the Device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    m_sceneRenderer = std::unique_ptr<CustomComputeShaderRenderer>(new CustomComputeShaderRenderer(m_deviceResources));
    m_sampleOverlay = std::unique_ptr<SampleOverlay>(new SampleOverlay(m_deviceResources, L"Direct2D Custom Compute Shader Effect"));
}

CustomComputeShaderMain::~CustomComputeShaderMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void CustomComputeShaderMain::CreateWindowSizeDependentResources()
{
    m_sceneRenderer->CreateWindowSizeDependentResources();
    m_sampleOverlay->CreateWindowSizeDependentResources();
}

void CustomComputeShaderMain::Update()
{
    // Unlike the pixel and vertex shader effects, the compute shader effect does not animate over time.
    // Therefore, render state is only updated on events such as CreateWindowSizeDependentResources().
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool CustomComputeShaderMain::Render()
{
    m_sceneRenderer->Render();
    m_sampleOverlay->Render();

    return true;
}

// Notifies renderers that device resources need to be released.
void CustomComputeShaderMain::OnDeviceLost()
{
    m_sceneRenderer->ReleaseDeviceDependentResources();
    m_sampleOverlay->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void CustomComputeShaderMain::OnDeviceRestored()
{
    m_sceneRenderer->CreateDeviceDependentResources();
    m_sampleOverlay->CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void CustomComputeShaderMain::SaveInternalState(_In_ IPropertySet ^ state)
{
    m_sceneRenderer->SaveInternalState(state);
}

void CustomComputeShaderMain::LoadInternalState(_In_ IPropertySet ^ state)
{
    m_sceneRenderer->LoadInternalState(state);
}
