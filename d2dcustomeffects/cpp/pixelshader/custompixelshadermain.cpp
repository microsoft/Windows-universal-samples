#include "pch.h"
#include "CustomPixelShaderMain.h"
#include "DirectXHelper.h"

using namespace CustomPixelShader;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
CustomPixelShaderMain::CustomPixelShaderMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    // Register to be notified if the Device is lost or recreated.
    m_deviceResources->RegisterDeviceNotify(this);

    m_sceneRenderer = std::unique_ptr<CustomPixelShaderRenderer>(new CustomPixelShaderRenderer(m_deviceResources));
    m_sampleOverlay = std::unique_ptr<SampleOverlay>(new SampleOverlay(m_deviceResources, L"Direct2D Custom Pixel Shader Effect"));
}

CustomPixelShaderMain::~CustomPixelShaderMain()
{
    // Deregister device notification.
    m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void CustomPixelShaderMain::CreateWindowSizeDependentResources()
{
    m_sceneRenderer->CreateWindowSizeDependentResources();
    m_sampleOverlay->CreateWindowSizeDependentResources();
}

void CustomPixelShaderMain::UpdatePointer(_In_ PointerEventArgs^ args)
{
    m_sceneRenderer->UpdatePointer(args);
}

// Updates the application state once per frame.
void CustomPixelShaderMain::Update()
{
    m_sceneRenderer->Update();
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool CustomPixelShaderMain::Render()
{
    m_sceneRenderer->Render();
    m_sampleOverlay->Render();

    return true;
}

// Notifies renderers that device resources need to be released.
void CustomPixelShaderMain::OnDeviceLost()
{
    m_sceneRenderer->ReleaseDeviceDependentResources();
    m_sampleOverlay->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void CustomPixelShaderMain::OnDeviceRestored()
{
    m_sceneRenderer->CreateDeviceDependentResources();
    m_sampleOverlay->CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void CustomPixelShaderMain::SaveInternalState(_In_ IPropertySet ^ state)
{
    m_sceneRenderer->SaveInternalState(state);
}

void CustomPixelShaderMain::LoadInternalState(_In_ IPropertySet ^ state)
{
    m_sceneRenderer->LoadInternalState(state);
}
