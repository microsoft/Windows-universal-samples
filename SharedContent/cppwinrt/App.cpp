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
#include "SampleConfiguration.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace SDKTemplate;
using namespace SDKTemplate::implementation;

// These placeholder functions are used if the sample does not
// implement the corresponding methods. This allows us to simulate
// C# partial methods in C++.

namespace
{
    [[maybe_unused]] void App_Construct(App*) { }
    [[maybe_unused]] bool App_OverrideOnLaunched(LaunchActivatedEventArgs const&) { return false; }
    [[maybe_unused]] void App_LaunchCompleted(LaunchActivatedEventArgs const&) { }
    [[maybe_unused]] void App_OnActivated(IActivatedEventArgs const&) { }
    [[maybe_unused]] void App_OnFileActivated(FileActivatedEventArgs const&) { }
}

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif

    App_Construct(this);
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const& e)
{
    // Samples have an opportunity to take over OnLaunched.
    if (App_OverrideOnLaunched(e))
    {
        return;
    }

    Frame rootFrame = CreateRootFrame();
    if (rootFrame.Content() == nullptr)
    {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a navigation
        // parameter
        rootFrame.Navigate(xaml_typename<SDKTemplate::MainPage>(), box_value(e.Arguments()));
    }

    // Ensure the current window is active
    Window::Current().Activate();

    // Some samples want access to the LaunchActivatedEventArgs after the launch is done.
    App_LaunchCompleted(e);
}

Frame App::CreateRootFrame()
{
    Frame rootFrame{ nullptr };
    auto content = Window::Current().Content();
    if (content)
    {
        rootFrame = content.try_as<Frame>();
    }

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        rootFrame = Frame();

        rootFrame.NavigationFailed({ this, &App::OnNavigationFailed });

        // Place the frame in the current Window
        Window::Current().Content(rootFrame);
    }

    return rootFrame;
}

// Some samples want to handle certain activations.
void App::OnActivated(IActivatedEventArgs const& e)
{
    App_OnActivated(e);
}

// Some samples want to handle file activations.
void App::OnFileActivated(FileActivatedEventArgs const& e)
{
    App_OnFileActivated(e);
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(IInspectable const&, NavigationFailedEventArgs const& e)
{
    throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}