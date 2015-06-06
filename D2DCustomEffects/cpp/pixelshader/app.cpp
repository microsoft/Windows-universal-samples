#include "pch.h"
#include "App.h"

#include <ppltasks.h>

using namespace CustomPixelShader;

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::Storage;
using namespace Windows::System;

using namespace concurrency;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto direct3DApplicationSource = ref new Direct3DApplicationSource();
    CoreApplication::Run(direct3DApplicationSource);
    return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
    return ref new App();
}

App::App() :
    m_windowClosed(false),
    m_windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(_In_ CoreApplicationView^ applicationView)
{
    // Register event handlers for app lifecycle. This example includes Activated, so that we
    // can make the CoreWindow active and start rendering on the window.
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

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(_In_ CoreWindow^ window)
{
    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

    window->PointerPressed +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

    // Disable all pointer visual feedback for better performance when touching.
    auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    pointerVisualizationSettings->IsContactFeedbackEnabled = false;
    pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

    m_deviceResources->SetWindow(window);
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(_In_ Platform::String^ entryPoint)
{
    if (m_main == nullptr)
    {
        m_main = std::unique_ptr<CustomPixelShaderMain>(new CustomPixelShaderMain(m_deviceResources));
    }
}

// This method is called after the window becomes active.
void App::Run()
{
    while (!m_windowClosed)
    {
        if (m_windowVisible)
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            m_main->Update();

            if (m_main->Render())
            {
                m_deviceResources->Present();
            }
        }
        else
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
}

// Application lifecycle event handlers.

void App::OnActivated(_In_ CoreApplicationView^ applicationView, _In_ IActivatedEventArgs^ args)
{
    // Run() won't start until the CoreWindow is activated.
    CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(_In_ Platform::Object^ sender, _In_ SuspendingEventArgs^ args)
{
    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral]()
    {
        m_deviceResources->Trim();

        m_main->SaveInternalState(ApplicationData::Current->LocalSettings->Values);

        deferral->Complete();
    });
}

void App::OnResuming(_In_ Platform::Object^ sender, _In_ Platform::Object^ args)
{
    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.

    m_main->LoadInternalState(ApplicationData::Current->LocalSettings->Values);
}

// Window event handlers.

void App::OnWindowSizeChanged(_In_ CoreWindow^ sender, _In_ WindowSizeChangedEventArgs^ args)
{
    m_deviceResources->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
    m_main->CreateWindowSizeDependentResources();
}

void App::OnVisibilityChanged(_In_ CoreWindow^ sender, _In_ VisibilityChangedEventArgs^ args)
{
    m_windowVisible = args->Visible;
}

void App::OnWindowClosed(_In_ CoreWindow^ sender, _In_ CoreWindowEventArgs^ args)
{
    m_windowClosed = true;
}

void App::OnPointerPressed(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ args)
{
    m_main->UpdatePointer(args);
}

// DisplayInformation event handlers.

void App::OnDpiChanged(_In_ DisplayInformation^ sender, _In_ Object^ args)
{
    m_deviceResources->SetDpi(sender->LogicalDpi);
    m_main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(_In_ DisplayInformation^ sender, _In_ Object^ args)
{
    m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
    m_main->CreateWindowSizeDependentResources();
}

void App::OnDisplayContentsInvalidated(_In_ DisplayInformation^ sender, _In_ Object^ args)
{
    m_deviceResources->ValidateDevice();
}