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
#include "AppView.h"

using namespace HolographicFaceTracker;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Holographic;
using namespace Windows::UI::Core;

// The main function is only used to initialize our IFrameworkView class.
// Under most circumstances, you should not need to modify this function.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    AppViewSource^ appViewSource = ref new ::AppViewSource();
    CoreApplication::Run(appViewSource);
    return 0;
}

IFrameworkView^ AppViewSource::CreateView()
{
    return ref new AppView();
}

AppView::AppView()
{
}


// IFrameworkView methods

// The first method called when the IFrameworkView is being created.
// Use this method to subscribe for Windows shell events and to initialize your app.
void AppView::Initialize(CoreApplicationView^ applicationView)
{
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &AppView::OnViewActivated);

    // Register event handlers for app lifecycle.
    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &AppView::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &AppView::OnResuming);

    // At this point we have access to the device and we can create device-dependent
    // resources.
    m_deviceResources = std::make_shared<DX::DeviceResources>();

    m_main = std::make_unique<HolographicFaceTrackerMain>(m_deviceResources);
}

// Called when the CoreWindow object is created (or re-created).
void AppView::SetWindow(CoreWindow^ window)
{
    // Register for keypress notifications.
    window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &AppView::OnKeyPressed);

    // Register for notification that the app window is being closed.
    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &AppView::OnWindowClosed);

    // Register for notifications that the app window is losing focus.
    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &AppView::OnVisibilityChanged);

    // Create a holographic space for the core window for the current view.
    // Presenting holographic frames that are created by this holographic space will put
    // the app into exclusive mode.
    m_holographicSpace = HolographicSpace::CreateForCoreWindow(window);

    // The DeviceResources class uses the preferred DXGI adapter ID from the holographic
    // space (when available) to create a Direct3D device. The HolographicSpace
    // uses this ID3D11Device to create and manage device-based resources such as
    // swap chains.
    m_deviceResources->SetHolographicSpace(m_holographicSpace);

    // The main class uses the holographic space for updates and rendering.
    m_main->SetHolographicSpace(m_holographicSpace);
}

// The Load method can be used to initialize scene resources or to load a
// previously saved app state.
void AppView::Load(Platform::String^ entryPoint)
{
}

// This method is called after the window becomes active. It oversees the
// update, draw, and present loop, and it also oversees window message processing.
void AppView::Run()
{
    while (!m_windowClosed)
    {
        if (m_windowVisible)
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            HolographicFrame^ holographicFrame = m_main->Update();

            if (m_main->Render(holographicFrame))
            {
                m_deviceResources->Present(holographicFrame);
            }
        }
        else
        {
            CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
        }
    }
}

// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
// This method is not often used, but IFrameworkView requires it and it will be called for
// holographic apps.
void AppView::Uninitialize()
{

}


// Application lifecycle event handlers

// Called when the app view is activated. Activates the app's CoreWindow.
void AppView::OnViewActivated(CoreApplicationView^ sender, IActivatedEventArgs^ args)
{
    // Run() won't start until the CoreWindow is activated.
    sender->CoreWindow->Activate();
}

void AppView::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely; after about five seconds,
    // the app will be forced to exit.
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral] ()
    {
        m_deviceResources->Trim();

        if (m_main != nullptr)
        {
            m_main->SaveAppState();
        }

        deferral->Complete();
    });
}

void AppView::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.

    if (m_main != nullptr)
    {
        m_main->LoadAppState();
    }
}


// Window event handlers

void AppView::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    m_windowVisible = args->Visible;
}

void AppView::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    m_windowClosed = true;
}


// Input event handlers

void AppView::OnKeyPressed(CoreWindow^ sender, KeyEventArgs^ args)
{
    // This code sample does not use keyboard input.
}
