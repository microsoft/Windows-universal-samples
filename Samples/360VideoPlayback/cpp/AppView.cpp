#include "pch.h"
#include "AppView.h"
#include "VideoGallery.xaml.h"
#include <ppltasks.h>

using namespace _360VideoPlayback;
using namespace concurrency;
using namespace Platform;
using namespace std;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Media::Core;
using namespace Windows::Media::Playback;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;

MediaPlayer^ AppView::m_mediaPlayer = nullptr;

MediaPlayer^ AppView::GetMediaPlayer()
{
    return m_mediaPlayer;
}

AppViewSource::AppViewSource(Uri^ sourceUri)
    : m_sourceUri(sourceUri)
{
}

IFrameworkView^ AppViewSource::CreateView()
{
    return ref new AppView(m_sourceUri);
}

AppView::AppView(Uri^ sourceUri)
    : m_sourceUri(sourceUri)
{
    m_mediaPlayer = ref new MediaPlayer();
}

// IFrameworkView methods

// The first method called when the IFrameworkView is being created.
// Use this method to subscribe for Windows shell events and to initialize your app.
void AppView::Initialize(CoreApplicationView^ applicationView)
{
    CoreWindow^ window = CoreWindow::GetForCurrentThread();
    if (window != nullptr)
    {
        window->Activated += ref new TypedEventHandler<CoreWindow^, WindowActivatedEventArgs^>(this, &AppView::OnMainViewActivated);
    }
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &AppView::OnViewActivated);
}

void AppView::OnMainViewActivated(CoreWindow^ sender, WindowActivatedEventArgs^ args)
{
    m_mainWindowReactivated = true;
    m_windowClosed = true;
    TryCloseWindow();
}

// Called when the CoreWindow object is created (or re-created).
void AppView::SetWindow(CoreWindow^ window)
{
    ApplicationView::GetForCurrentView()->Consolidated +=
        ref new TypedEventHandler<ApplicationView^, ApplicationViewConsolidatedEventArgs^>(this, &AppView::OnConsolidated);

    // Register event handlers for app lifecycle.
    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &AppView::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Object^>(this, &AppView::OnResuming);

    // Register for keypress notifications.
    window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &AppView::OnKeyPressed);

    // Register for notification that the app window is being closed.
    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &AppView::OnWindowClosed);

    // Register for notifications that the app window is losing focus.
    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &AppView::OnVisibilityChanged);

    // At this point we have access to the device and we can create device-dependent
    // resources.
    m_deviceResources = _360VideoPlaybackMain::GetDeviceResources();

    m_main = std::make_unique<_360VideoPlaybackMain>();

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

    MediaSource^ source = MediaSource::CreateFromUri(m_sourceUri);
    m_mediaPlayer->MediaOpened += ref new TypedEventHandler<MediaPlayer^, Object^>(this, &AppView::OnMediaOpened);
    m_mediaPlayer->Source = source;

    m_mediaPlayer->IsVideoFrameServerEnabled = true;
    m_mediaPlayer->IsLoopingEnabled = true;
}

// The Load method can be used to initialize scene resources or to load a
// previously saved app state.
void AppView::Load(String^ entryPoint)
{
}

// This method is called after the window becomes active. It oversees the
// update, draw, and present loop, and it also oversees window message processing.
void AppView::Run()
{
    CoreWindow^ window = CoreWindow::GetForCurrentThread();
    window->Activate();

    while (!m_windowClosed)
    {
        if (m_ShadersNeed)
        {
            if (!m_ShadersCreated)
            {
                m_main->CreateVideoShaders();
                m_ShadersCreated = true;
            }
        }
        else
        {
            window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
            continue;
        }

        if (m_appViewConsolidated || m_mainWindowReactivated)
        {
            m_windowClosed = true;
            TryCloseWindow();
        }
        else
        {
            window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
            if (m_windowVisible && (m_holographicSpace != nullptr))
            {
                HolographicFrame^ holographicFrame = m_main->Update();

                if (m_main->Render(holographicFrame))
                {
                    // The holographic frame has an API that presents the swap chain for each
                    // holographic camera.
                    m_deviceResources->Present(holographicFrame);
                }
            }
        }
    }
}

// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
// This method is not often used, but IFrameworkView requires it and it will be called for
// holographic apps.
void AppView::Uninitialize()
{
    if (m_mediaPlayer != nullptr)
    {
        m_mediaPlayer->Source = nullptr;
        m_mediaPlayer = nullptr;
    }
    if (m_main != nullptr)
    {
        m_main->UnregisterHolographicEventHandlers();
    }
}


// Application lifecycle event handlers

// Called when the app view is activated. Activates the app's CoreWindow.
void AppView::OnViewActivated(CoreApplicationView^ sender, IActivatedEventArgs^ args)
{
    // Run() won't start until the CoreWindow is activated.
    sender->CoreWindow->Activate();
}

void AppView::OnSuspending(Object^ sender, SuspendingEventArgs^ args)
{
    // Save app state asynchronously after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely; after about five seconds,
    // the app will be forced to exit.
    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

    create_task([this, deferral]()
    {
        m_deviceResources->Trim();

        if (m_main != nullptr)
        {
            m_main->SaveAppState();
        }

        //
        // TODO: Insert code here to save your app state.
        //

        deferral->Complete();
    });
}

void AppView::OnResuming(Object^ sender, Object^ args)
{
    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.

    if (m_main != nullptr)
    {
        m_main->LoadAppState();
    }

    //
    // TODO: Insert code here to load your app state.
    //
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
    //
    // TODO: Bluetooth keyboards are supported by HoloLens. You can use this method for
    //       keyboard input if you want to support it as an optional input method for
    //       your holographic app.
    //

    if (args->VirtualKey == Windows::System::VirtualKey::Escape)
    {
        ApplicationViewSwitcher::SwitchAsync(VideoGallery::GetMainViewId(), ApplicationView::GetForCurrentView()->Id, ApplicationViewSwitchingOptions::ConsolidateViews);
    }
}


void AppView::OnConsolidated(ApplicationView^ sender, ApplicationViewConsolidatedEventArgs^ args)
{
    m_appViewConsolidated = true;
    m_windowClosed = true;
    TryCloseWindow();
    Uninitialize();
}

void AppView::OnMediaOpened(MediaPlayer^ sender, Object^ args)
{
    m_ShadersNeed = true;
}

void AppView::TryCloseWindow()
{
    CoreWindow^ window = CoreWindow::GetForCurrentThread();
    if (window != nullptr)
    {
        window->Close();
    }
}
