#include "pch.h"
#include "CustomVertexShaderMain.h"
#include "DirectXHelper.h"

using namespace CustomVertexShader;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System::Threading;
using namespace Windows::UI::Input;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
CustomVertexShaderMain::CustomVertexShaderMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the Device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    m_sceneRenderer = std::unique_ptr<CustomVertexShaderRenderer>(new CustomVertexShaderRenderer(m_deviceResources));
    m_sampleOverlay = std::unique_ptr<SampleOverlay>(new SampleOverlay(m_deviceResources, L"Direct2D Custom Vertex Shader Effect"));
}

CustomVertexShaderMain::~CustomVertexShaderMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void CustomVertexShaderMain::CreateWindowSizeDependentResources()
{
    m_sceneRenderer->CreateWindowSizeDependentResources();
    m_sampleOverlay->CreateWindowSizeDependentResources();
}

void CustomVertexShaderMain::UpdateManipulationState(_In_ ManipulationUpdatedEventArgs ^ args)
{
    m_sceneRenderer->UpdateManipulationState(args);
}

// Updates the application state once per frame.
void CustomVertexShaderMain::Update()
{
    m_sceneRenderer->Update();
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool CustomVertexShaderMain::Render()
{
    m_sceneRenderer->Render();
    m_sampleOverlay->Render();

    return true;
}

// Notifies renderers that device resources need to be released.
void CustomVertexShaderMain::OnDeviceLost()
{
    m_sceneRenderer->ReleaseDeviceDependentResources();
    m_sampleOverlay->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void CustomVertexShaderMain::OnDeviceRestored()
{
    m_sceneRenderer->CreateDeviceDependentResources();
    m_sampleOverlay->CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void CustomVertexShaderMain::SaveInternalState(_In_ IPropertySet ^ state)
{
    m_sceneRenderer->SaveInternalState(state);
}

void CustomVertexShaderMain::LoadInternalState(_In_ IPropertySet ^ state)
{
    m_sceneRenderer->LoadInternalState(state);
}
