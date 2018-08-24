#pragma once

#include "Common\DeviceResources.h"
#include "360VideoPlaybackMain.h"

namespace _360VideoPlayback
{
    // IFrameworkView class. Connects the app with the Windows shell and handles application lifecycle events.
    ref class AppView sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        AppView(Windows::Foundation::Uri^ sourceUri);

        // IFrameworkView methods.
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();
        static Windows::Media::Playback::MediaPlayer^ GetMediaPlayer();

    protected:
        // Application lifecycle event handlers.
        void OnViewActivated(Windows::ApplicationModel::Core::CoreApplicationView^ sender, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
        void OnMainViewActivated(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowActivatedEventArgs^ args);
        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
        void OnResuming(Platform::Object^ sender, Platform::Object^ args);

        // Window event handlers.
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
        void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

        // CoreWindow input event handlers.
        void OnKeyPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args);

    private:
        std::unique_ptr<_360VideoPlaybackMain> m_main;
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        bool m_windowClosed = false;
        bool m_windowVisible = true;
        bool m_ShadersNeed = false;
        bool m_ShadersCreated = false;
        bool m_appViewConsolidated = false;
        bool m_mainWindowReactivated = false;
        // The holographic space the app will use for rendering.
        Windows::Graphics::Holographic::HolographicSpace^ m_holographicSpace = nullptr;
        static Windows::Media::Playback::MediaPlayer^ m_mediaPlayer;
        void OnConsolidated(Windows::UI::ViewManagement::ApplicationView^ sender, Windows::UI::ViewManagement::ApplicationViewConsolidatedEventArgs^ args);
        void OnMediaOpened(Windows::Media::Playback::MediaPlayer^ sender, Platform::Object^ args);
        void TryCloseWindow();
        Windows::Foundation::Uri^ m_sourceUri;
    };

    // The entry point for the app.
    ref class AppViewSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
    {
    public:
        AppViewSource(Windows::Foundation::Uri^ sourceUri);
        virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
    private:
        Windows::Foundation::Uri^ m_sourceUri;
    };
}

