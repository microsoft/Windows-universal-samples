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
#include "App.h"

using namespace concurrency;
using namespace DirectX;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Simple3DGameDX;
using namespace GameControl;

//--------------------------------------------------------------------------------------

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto directXApplicationSource = ref new DirectXApplicationSource();
    CoreApplication::Run(directXApplicationSource);
    return 0;
}

//--------------------------------------------------------------------------------------

IFrameworkView^ DirectXApplicationSource::CreateView()
{
    return ref new App();
}

//--------------------------------------------------------------------------------------

App::App()
{
}

//--------------------------------------------------------------------------------------

void App::Initialize(
    CoreApplicationView^ applicationView
    )
{
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

    // At this point we have access to the device. 
    // We can create the device-dependent resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();
}

//--------------------------------------------------------------------------------------

void App::SetWindow(
    CoreWindow^ window
    )
{
    window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);

    PointerVisualizationSettings^ visualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    visualizationSettings->IsContactFeedbackEnabled = false;
    visualizationSettings->IsBarrelButtonFeedbackEnabled = false;

    m_deviceResources->SetWindow(window);

    window->Activated +=
        ref new TypedEventHandler<CoreWindow^, WindowActivatedEventArgs^>(this, &App::OnWindowActivationChanged);

    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Platform::Object^>(this, &App::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

    currentDisplayInformation->StereoEnabledChanged +=
        ref new TypedEventHandler<DisplayInformation^, Platform::Object^>(this, &App::OnStereoEnabledChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Platform::Object^>(this, &App::OnDisplayContentsInvalidated);
}

//--------------------------------------------------------------------------------------

void App::Load(
    Platform::String^ /* entryPoint */
    )
{
    if (!m_main)
    {
        m_main = std::unique_ptr<GameMain>(new GameMain(m_deviceResources));
    }
}

//--------------------------------------------------------------------------------------

void App::Run()
{
    m_main->Run();
}

//--------------------------------------------------------------------------------------

void App::Uninitialize()
{
}

//----------------------------------------------------------------------

void App::OnStereoEnabledChanged(DisplayInformation^ /* sender */, Platform::Object^ /* args */)
{
    m_deviceResources->UpdateStereoState();
    m_main->CreateWindowSizeDependentResources();
}

//--------------------------------------------------------------------------------------

void App::OnWindowSizeChanged(
    CoreWindow^ window,
    WindowSizeChangedEventArgs^  args
    )
{
    m_deviceResources->SetLogicalSize(args->Size);
    m_main->CreateWindowSizeDependentResources();
}

//--------------------------------------------------------------------------------------

void App::OnWindowClosed(
    CoreWindow^ /* sender */,
    CoreWindowEventArgs^ /* args */
    )
{
    m_main->Close();
}

//--------------------------------------------------------------------------------------

void App::OnDpiChanged(DisplayInformation^ sender, Platform::Object^ /* args */)
{
    m_deviceResources->SetDpi(sender->LogicalDpi);
    m_main->CreateWindowSizeDependentResources();
}

//--------------------------------------------------------------------------------------

void App::OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args)
{
    m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
    m_main->CreateWindowSizeDependentResources();
}
//--------------------------------------------------------------------------------------

void App::OnDisplayContentsInvalidated(DisplayInformation^ /* sender */, Platform::Object^ /* args */)
{
    m_deviceResources->ValidateDevice();
}

//--------------------------------------------------------------------------------------

void App::OnActivated(
    CoreApplicationView^ /* applicationView */,
    IActivatedEventArgs^ /* args */
    )
{
    CoreWindow::GetForCurrentThread()->Activate();
}

//--------------------------------------------------------------------------------------

void App::OnVisibilityChanged(
    CoreWindow^ /* sender */,
    VisibilityChangedEventArgs^ args
    )
{
    m_main->Visibility(args->Visible);
}

//--------------------------------------------------------------------------------------

void App::OnWindowActivationChanged(
    Windows::UI::Core::CoreWindow^ /* sender */,
    Windows::UI::Core::WindowActivatedEventArgs^ args
    )
{
    m_main->WindowActivationChanged(args->WindowActivationState);
}

//--------------------------------------------------------------------------------------

void App::OnSuspending(
    Platform::Object^ /* sender */,
    SuspendingEventArgs^ args
    )
{
    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral]()
    {
        m_deviceResources->Trim();

        m_main->Suspend();

        deferral->Complete();
    });
}

//--------------------------------------------------------------------------------------

void App::OnResuming(
    Platform::Object^ /* sender */,
    Platform::Object^ /* args */
    )
{
    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.
    m_main->Resume();
}

