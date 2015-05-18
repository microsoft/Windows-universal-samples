#pragma once

#include "pch.h"
#include "DeviceResources.h"
#include "CustomVertexShaderMain.h"

namespace CustomVertexShader
{
    // Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
    ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        App();

        // IFrameworkView Methods.
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

    protected:
        // Application lifecycle event handlers.
        void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
        void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
        void OnResuming(Platform::Object^ sender, Platform::Object^ args);

        // Window event handlers.
        void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
        void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
        void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

        // DisplayInformation event handlers.
        void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
        void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
        void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

        // Input and gesture event handlers.
        void OnPointerMoved(
            _In_ Windows::UI::Core::CoreWindow^ sender,
            _In_ Windows::UI::Core::PointerEventArgs^ args
            );

        void OnPointerPressed(
            _In_ Windows::UI::Core::CoreWindow^ sender,
            _In_ Windows::UI::Core::PointerEventArgs^ args
            );

        void OnPointerReleased(
            _In_ Windows::UI::Core::CoreWindow^ sender,
            _In_ Windows::UI::Core::PointerEventArgs^ args
            );

        void OnPointerWheelChanged(
            _In_ Windows::UI::Core::CoreWindow^ sender,
            _In_ Windows::UI::Core::PointerEventArgs^ args
            );

        void OnManipulationUpdated(
            _In_ Windows::UI::Input::GestureRecognizer^ sender,
            _In_ Windows::UI::Input::ManipulationUpdatedEventArgs^ args
            );

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<CustomVertexShaderMain> m_main;
        bool m_windowClosed;
        bool m_windowVisible;
        Windows::UI::Input::GestureRecognizer^ m_gestureRecognizer;
    };
}

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
