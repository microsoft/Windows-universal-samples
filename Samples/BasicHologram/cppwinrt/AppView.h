#pragma once

#include "Common\DeviceResources.h"
#include "BasicHologramMain.h"

namespace BasicHologram
{
    // IFrameworkView class. Connects the app with the Windows shell and handles application lifecycle events.
    class AppView sealed : public winrt::implements<AppView, winrt::Windows::ApplicationModel::Core::IFrameworkView>
    {
    public:
        // IFrameworkView methods.
        void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView);
        void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
        void Load(winrt::hstring const& entryPoint);
        void Run();
        void Uninitialize();

    protected:
        // Application lifecycle event handlers.
        void OnLaunched(winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const& args);
        void OnViewActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& sender, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
        void OnSuspending(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args);
        void OnResuming(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args);

        // Window event handlers.
        void OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args);
        void OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::CoreWindowEventArgs const& args);

        // CoreWindow input event handlers.
        void OnKeyPressed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args);
        void OnPointerPressed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);

    private:
        std::unique_ptr<BasicHologramMain> m_main;

        std::shared_ptr<DX::DeviceResources>                    m_deviceResources;
        bool                                                    m_windowClosed = false;
        bool                                                    m_windowVisible = true;

        // Event registration tokens.
        winrt::event_token                                      m_suspendingEventToken;
        winrt::event_token                                      m_resumingEventToken;
        winrt::event_token                                      m_keyDownEventToken;
        winrt::event_token                                      m_pointerPressedEventToken;
        winrt::event_token                                      m_windowClosedEventToken;
        winrt::event_token                                      m_visibilityChangedEventToken;

        // The holographic space the app will use for rendering.
        winrt::Windows::Graphics::Holographic::HolographicSpace m_holographicSpace = nullptr;
    };

    class AppViewSource sealed : public winrt::implements<AppViewSource, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource>
    {
    public:
        // IFrameworkViewSource method.
        winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView();

    private:
        AppView holographicView;
    };
}
