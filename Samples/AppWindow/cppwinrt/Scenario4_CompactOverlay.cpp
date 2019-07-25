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
#include "Scenario4_CompactOverlay.h"
#include "Scenario4_CompactOverlay.g.cpp"
#include "MainPage.h"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::WindowManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_CompactOverlay::Scenario4_CompactOverlay()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario4_CompactOverlay::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (appWindow != nullptr)
        {
            co_await appWindow.CloseAsync();
        }
    }

    fire_and_forget Scenario4_CompactOverlay::ShowWindowBtn_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Is CompactOverlay supported for our main window?
        // If so, it will be supported for a new window as well.
        // If it isn't, it will not be supported for new windows either so we cannot proceed.
        if (!ApplicationView::GetForCurrentView().IsViewModeSupported(ApplicationViewMode::CompactOverlay))
        {
            return;
        }

        unique_disabled_control re_enabled{ showWindowBtn() };

        // Only ever create one window. If the AppWindow already exists call TryShow on it to bring it to foreground.
        if (appWindow == nullptr)
        {
            // Create a new window
            appWindow = co_await AppWindow::TryCreateAsync();
            // Clean up when the window is closced
            appWindow.Closed({ get_weak(), &Scenario4_CompactOverlay::OnWindowClosed });

            // Is CompactOverlay supported for this AppWindow? If not, then stop.
            if (appWindow.Presenter().IsPresentationSupported(AppWindowPresentationKind::CompactOverlay))
            {
                // Create a new frame for the window
                // Navigate the frame to the CompactOverlay page inside it.
                appWindowFrame.Navigate(xaml_typename<SDKTemplate::SecondaryAppWindowPage>());
                // Attach the frame to the window
                ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowFrame);
                // Let's set the title so that we can tell the windows apart
                appWindow.Title(L"CompactOverlay Window");
                // Request the Presentation of the window to CompactOverlay
                bool switched = appWindow.Presenter().RequestPresentation(AppWindowPresentationKind::CompactOverlay);
                if (switched)
                {
                    // If the request was satisfied, show the window
                    co_await appWindow.TryShowAsync();
                }
            }
        }
        else
        {
            co_await appWindow.TryShowAsync();
        }
    }

    void Scenario4_CompactOverlay::OnWindowClosed(AppWindow const&, IInspectable const&)
    {
        // Make sure we release the reference to this window, and release XAML resources.
        appWindow = nullptr;
        appWindowFrame.Content(nullptr);
    }
}
