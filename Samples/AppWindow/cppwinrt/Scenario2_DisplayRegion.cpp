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
#include "Scenario2_DisplayRegion.h"
#include "Scenario2_DisplayRegion.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::WindowManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_DisplayRegion::Scenario2_DisplayRegion()
    {
        InitializeComponent();
    }

    void Scenario2_DisplayRegion::OnNavigatedTo(NavigationEventArgs const&)
    {
        // Check to see if we have more than one DisplayRegion, if not we disable the button since the scenario will not work

        showWindowBtn().IsEnabled(NumberOfAvailableDisplayRegions() > 1);
    }

    fire_and_forget Scenario2_DisplayRegion::OnNavigatedFrom(NavigationEventArgs const&)
    {
        if (appWindow != nullptr)
        {
            co_await appWindow.CloseAsync();
        }
    }

    int Scenario2_DisplayRegion::NumberOfAvailableDisplayRegions()
    {
        // Get the list of all DisplayRegions defined for the WindowingEnvironment that our application is currently in
        IVectorView<DisplayRegion> displayRegions = ApplicationView::GetForCurrentView().WindowingEnvironment().GetDisplayRegions();

        int availableDisplayRegions = 0;
        // count all the DisplayRegions that are marked as available to the app for use.
        for (DisplayRegion const& displayregion : displayRegions)
        {
            if (displayregion.IsVisible())
            {
                availableDisplayRegions++;
            }
        }
        return availableDisplayRegions;
    }

    DisplayRegion Scenario2_DisplayRegion::GetOtherDisplayRegion(DisplayRegion currentAppDisplayRegion)
    {
        // Get the list of all DisplayRegions defined for the WindowingEnvironment that our application is currently in
        IVectorView<DisplayRegion> displayRegions = ApplicationView::GetForCurrentView().WindowingEnvironment().GetDisplayRegions();
        for (DisplayRegion const& displayRegion : displayRegions)
        {
            if (displayRegion != currentAppDisplayRegion && displayRegion.IsVisible())
            {
                return displayRegion;
            }
        }

        return nullptr;
    }

    fire_and_forget Scenario2_DisplayRegion::Button_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto lifetime = get_strong();

        unique_disabled_control re_enabled{ showWindowBtn() };

        // Only ever create one window. If the AppWindow already exists call TryShow on it to bring it to foreground.
        if (appWindow == nullptr)
        {
            // Create a new window
            appWindow = co_await AppWindow::TryCreateAsync();
            // Clean up when the window is closed
            appWindow.Closed({ get_weak(), &Scenario2_DisplayRegion::OnWindowClosed });
            // Navigate the frame to the page we want to show in the new window
            appWindowFrame.Navigate(xaml_typename<SDKTemplate::SecondaryAppWindowPage>());
            // Attach the XAML content to our window
            ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowFrame);
        }

        // Get DisplayRegion to position our window on
        DisplayRegion secondaryDisplayRegion = GetOtherDisplayRegion(ApplicationView::GetForCurrentView().GetDisplayRegions().GetAt(0));
        if (secondaryDisplayRegion != nullptr)
        {
            appWindow.RequestMoveToDisplayRegion(secondaryDisplayRegion);
        }

        // If the window is not visible, show it and/or bring it to foreground
        if (!appWindow.IsVisible())
        {
            co_await appWindow.TryShowAsync();
        }
    }

    void Scenario2_DisplayRegion::OnWindowClosed(AppWindow const&, IInspectable const&)
    {
        // Make sure we release the reference to this window, and release XAML resources.
        appWindow = nullptr;
        appWindowFrame.Content(nullptr);
    }

}
