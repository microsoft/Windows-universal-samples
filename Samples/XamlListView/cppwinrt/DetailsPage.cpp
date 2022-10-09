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
#include "DetailsPage.h"
#include "DetailsPage.g.cpp"

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media::Animation;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    DetailsPage::DetailsPage()
    {
        InitializeComponent();
    }

    void DetailsPage::OnLoaded(IInspectable const&, RoutedEventArgs const&)
    {
        if (ShouldGoToWideState())
        {
            // We shouldn't see this page since we are in "wide master-detail" mode.
            // Play a transition as we are navigating from a separate page.
            NavigateBackForWideState(/* useTransition */ true);
        }
        else
        {
            // Realize the main page content.
            FindName(L"RootPanel");
        }

        windowSizeChangedToken = Window::Current().SizeChanged({ get_weak(), &DetailsPage::OnWindowSizeChanged });
    }

    void DetailsPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        selectedContact = e.Parameter().as<SDKTemplate::Contact>();
        // Register for hardware and software back request from the system
        SystemNavigationManager systemNavigationManager = SystemNavigationManager::GetForCurrentView();
        backRequestedToken = systemNavigationManager.BackRequested({ get_weak(), &DetailsPage::OnBackRequested });
        systemNavigationManager.AppViewBackButtonVisibility(AppViewBackButtonVisibility::Visible);
    }

    void DetailsPage::OnNavigatedFrom(NavigationEventArgs const&)
    {
        SystemNavigationManager systemNavigationManager = SystemNavigationManager::GetForCurrentView();
        systemNavigationManager.BackRequested(backRequestedToken);
        systemNavigationManager.AppViewBackButtonVisibility(AppViewBackButtonVisibility::Collapsed);
    }

    void DetailsPage::OnBackRequested(IInspectable const&, BackRequestedEventArgs const& e)
    {
        // Mark event as handled so we don't leave the app.
        e.Handled(true);
        // Page above us will be our master view.
        // Make sure we are using the "drill out" animation in this transition.
        Frame().Navigate(xaml_typename<SDKTemplate::Scenario3_MasterDetail>(), box_value(L"B" + SelectedContact().Id()), EntranceNavigationTransitionInfo());
    }

    void DetailsPage::NavigateBackForWideState(bool useTransition)
    {
        // Evict this page from the cache as we may not need it again.
        NavigationCacheMode(NavigationCacheMode::Disabled);

        if (useTransition)
        {
            Frame().GoBack(EntranceNavigationTransitionInfo());
        }
        else
        {
            Frame().GoBack(SuppressNavigationTransitionInfo());
        }
    }

    bool DetailsPage::ShouldGoToWideState()
    {
        return Window::Current().Bounds().Width >= 720;
    }

    void DetailsPage::OnUnloaded(IInspectable const&, RoutedEventArgs const&)
    {
        Window::Current().SizeChanged(windowSizeChangedToken);
    }

    void DetailsPage::OnWindowSizeChanged(IInspectable const&, WindowSizeChangedEventArgs const&)
    {
        if (ShouldGoToWideState())
        {
            // Make sure we are no longer listening to window change events.
            Window::Current().SizeChanged(windowSizeChangedToken);

            // We shouldn't see this page since we are in "wide master-detail" mode.
            NavigateBackForWideState(/* useTransition */ false);
        }
    }

    void DetailsPage::DeleteItem(IInspectable const&, RoutedEventArgs const&)
    {
        Frame().Navigate(xaml_typename<SDKTemplate::Scenario3_MasterDetail>(), box_value(L"D" + SelectedContact().Id()), EntranceNavigationTransitionInfo());
    }

}
