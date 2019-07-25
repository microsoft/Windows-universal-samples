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
#include "Scenario1_SecondaryWindow.h"
#include "Scenario1_SecondaryWindow.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::WindowManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_SecondaryWindow::Scenario1_SecondaryWindow()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario1_SecondaryWindow::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (appWindow != nullptr)
        {
            co_await appWindow.CloseAsync();
        }
    }

    fire_and_forget Scenario1_SecondaryWindow::Button_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        showWindowBtn().IsEnabled(false);

        // Only ever create one window. If the AppWindow already exists call TryShow on it to bring it to foreground.
        if (appWindow == nullptr)
        {
            // Create a new window
            appWindow = co_await AppWindow::TryCreateAsync();
            // Clean up when the window is closed
            appWindow.Closed({ get_weak(), &Scenario1_SecondaryWindow::OnWindowClosed });
            // Navigate the frame to the page we want to show in the new window
            appWindowFrame.Navigate(xaml_typename<SDKTemplate::SecondaryAppWindowPage>());
            // Attach the XAML content to our window
            ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowFrame);
        }

        // Now show the window
        co_await appWindow.TryShowAsync();

        showWindowBtn().IsEnabled(true);
    }

    void Scenario1_SecondaryWindow::OnWindowClosed(AppWindow const&, IInspectable const&)
    {
        // Make sure we release the reference to this window, and release XAML resources.
        appWindow = nullptr;
        appWindowFrame.Content(nullptr);
    }

}
