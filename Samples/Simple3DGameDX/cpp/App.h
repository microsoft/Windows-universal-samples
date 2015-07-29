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

#pragma once

#include "DeviceResources.h"
#include "GameMain.h"

namespace Simple3DGameDX
{
    ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    public:
        App();

        // IFrameworkView Methods
        virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
        virtual void Load(Platform::String^ entryPoint);
        virtual void Run();
        virtual void Uninitialize();

    protected:
        // Application lifecycle event handlers.
        void OnActivated(
            Windows::ApplicationModel::Core::CoreApplicationView^ applicationView,
            Windows::ApplicationModel::Activation::IActivatedEventArgs^ args
            );
        void OnSuspending(
            Platform::Object^ sender,
            Windows::ApplicationModel::SuspendingEventArgs^ args
            );
        void OnResuming(
            Platform::Object^ sender,
            Platform::Object^ args
            );

        // Window event handlers.
        void OnWindowSizeChanged(
            Windows::UI::Core::CoreWindow^ sender,
            Windows::UI::Core::WindowSizeChangedEventArgs^ args
            );
        void OnVisibilityChanged(
            Windows::UI::Core::CoreWindow^ sender,
            Windows::UI::Core::VisibilityChangedEventArgs^ args
            );
        void OnWindowClosed(
            Windows::UI::Core::CoreWindow^ sender,
            Windows::UI::Core::CoreWindowEventArgs^ args
            );
        void OnWindowActivationChanged(
            Windows::UI::Core::CoreWindow^ sender,
            Windows::UI::Core::WindowActivatedEventArgs^ args
            );

        // DisplayInformation event handlers.
        void OnDpiChanged(
            Windows::Graphics::Display::DisplayInformation^ sender,
            Platform::Object^ args
            );
        void OnOrientationChanged(
            Windows::Graphics::Display::DisplayInformation^ sender,
            Platform::Object^ args
            );
        void OnDisplayContentsInvalidated( Windows::Graphics::Display::DisplayInformation^ sender,
            Platform::Object^ args
            );
        void OnStereoEnabledChanged(
            Windows::Graphics::Display::DisplayInformation^ sender,
            Platform::Object^ args
            );

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<GameMain> m_main;
    };
};

ref class DirectXApplicationSource : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};

