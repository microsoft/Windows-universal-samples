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
#include "Scenario5_RelativePositioning.h"
#include "Scenario5_RelativePositioning.g.cpp"
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
    Scenario5_RelativePositioning::Scenario5_RelativePositioning()
    {
        InitializeComponent();
    }

    fire_and_forget Scenario5_RelativePositioning::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (appWindow != nullptr)
        {
            co_await appWindow.CloseAsync();
        }
    }

    fire_and_forget Scenario5_RelativePositioning::ShowWindowBtn_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        // Clear any previous message.
        auto rootPage = MainPage::Current();
        rootPage.NotifyUser({}, NotifyType::StatusMessage);

        float horizontalOffset = 0.0f;
        float verticalOffset = 0.0f;
        if (!TryParseFloat(windowOffsetXTxt().Text(), horizontalOffset) ||
            !TryParseFloat(windowOffsetYTxt().Text(), verticalOffset))
        {
            rootPage.NotifyUser(L"Please specify valid numeric offsets.", NotifyType::ErrorMessage);
            co_return;
        }

        unique_disabled_control re_enabled{ showWindowBtn() };

        // Only ever create and show one window.
        if (appWindow == nullptr)
        {
            // Create a new window
            appWindow = co_await AppWindow::TryCreateAsync();
            // Clean up when the window is closed
            appWindow.Closed({ get_weak(), &Scenario5_RelativePositioning::OnWindowClosed });
            // Navigate the frame to the page we want to show in the new window
            appWindowFrame.Navigate(xaml_typename<SDKTemplate::SecondaryAppWindowPage>());
            // Attach the XAML content to our window
            ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowFrame);
        }

        // Make a point for our offset.
        Point offset{ horizontalOffset, verticalOffset };

        // Check if we should be setting our position relative to ApplicationView or DisplayRegion
        if (positionOffsetDisplayRegionRB().IsChecked().Value())
        {
            // Request an offset relative to the DisplayRegion we're on
            appWindow.RequestMoveRelativeToDisplayRegion(ApplicationView::GetForCurrentView().GetDisplayRegions().GetAt(0), offset);
        }
        else
        {
            // Relative to our ApplicationView
            appWindow.RequestMoveRelativeToCurrentViewContent(offset);
        }

        // If the window is not visible, show it and/or bring it to foreground
        if (!appWindow.IsVisible())
        {
            co_await appWindow.TryShowAsync();
        }
    }

    void Scenario5_RelativePositioning::OnWindowClosed(AppWindow const&, IInspectable const&)
    {
        // Make sure we release the reference to this window, and release XAML resources.
        appWindow = nullptr;
        appWindowFrame.Content(nullptr);
    }
}
