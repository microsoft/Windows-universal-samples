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
#include "Scenario3_Size.h"
#include "Scenario3_Size.g.cpp"
#include "SampleConfiguration.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::WindowManagement;
using namespace Windows::UI::WindowManagement::Preview;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_Size::Scenario3_Size()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario3_Size::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (appWindow != nullptr)
        {
            co_await appWindow.CloseAsync();
        }
    }

    // Lower bounds on window size to keep things from being unreasonably tiny.
    constexpr float MinWindowWidth = 192;
    constexpr float MinWindowHeight = 48;

    fire_and_forget Scenario3_Size::ShowWindowBtn_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Clear any previous message.
        auto rootPage = MainPage::Current();
        rootPage.NotifyUser({}, NotifyType::StatusMessage);

        float windowWidth = 0.0f;
        float windowHeight = 0.0f;
        if (!TryParseFloat(windowWidthTxt().Text(), windowWidth) ||
            !TryParseFloat(windowHeightTxt().Text(), windowHeight) ||
            windowWidth < MinWindowWidth || windowHeight < MinWindowHeight)
        {
            rootPage.NotifyUser(L"Please specify a width of at least " + to_hstring(MinWindowWidth) +
                L" and a height of at least " + to_hstring(MinWindowHeight) + L".", NotifyType::ErrorMessage);
            co_return;
        }
        unique_disabled_control re_enabled{ showWindowBtn() };

        // Only ever create one window. If the AppWindow already exists call TryShow on it to bring it to foreground.
        if (appWindow == nullptr)
        {
            // Create a new window
            appWindow = co_await AppWindow::TryCreateAsync();
            // Clean up when the window is closced
            appWindow.Closed({ get_weak(), &Scenario3_Size::OnWindowClosed });
            // Navigate the frame to the page we want to show in the new window
            appWindowFrame.Navigate(xaml_typename<SDKTemplate::SecondaryAppWindowPage>());
            // Attach the XAML content to our window
            ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowFrame);
        }

        // If specified size is smaller than the default min size for a window we need to set a new preferred min size first.
        // Let's set it to the smallest allowed and leave it at that.
        if (windowWidth < 500 || windowHeight < 320)
        {
            WindowManagementPreview::SetPreferredMinSize(appWindow, { MinWindowWidth, MinWindowHeight });
        }
        // Request the size of our window
        appWindow.RequestSize({ windowWidth, windowHeight });
        // Attach the XAML content to our window
        ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowFrame);

        // If the window is not visible, show it and/or bring it to foreground
        if (!appWindow.IsVisible())
        {
            co_await appWindow.TryShowAsync();
        }
    }

    void Scenario3_Size::OnWindowClosed(AppWindow const&, IInspectable const&)
    {
        // Make sure we release the reference to this window, and release XAML resources.
        appWindow = nullptr;
        appWindowFrame.Content(nullptr);
    }
}
