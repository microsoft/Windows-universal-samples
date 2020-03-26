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
#include "DetailPage.h"
#include "DetailPage.g.cpp"

using namespace winrt;
using namespace MasterDetailApp::Data;
using namespace MasterDetailApp::ViewModels;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::MasterDetailApp::implementation
{
    Windows::UI::Xaml::DependencyProperty DetailPage::s_itemProperty{ nullptr };

    void DetailPage::RegisterDependencyProperties()
    {
        s_itemProperty = DependencyProperty::Register(L"Item", xaml_typename<ItemViewModel>(),
            xaml_typename<MasterDetailApp::DetailPage>(), nullptr);
    }

    DetailPage::DetailPage()
    {
        InitializeComponent();
    }

    void DetailPage::OnNavigatedTo(NavigationEventArgs const& e)
    {
        // Parameter is item ID
        auto id = unbox_value<int>(e.Parameter());
        Item(ItemViewModel::FromItem(ItemsDataSource::GetItemById(id)));

        ReplaceLastBackStackEntryParameter(e.Parameter());

        // Register for hardware and software back request from the system
        auto systemNavigationManager = SystemNavigationManager::GetForCurrentView();
        m_backRequestedEventRegistrationToken =
            systemNavigationManager.BackRequested({ this, &DetailPage::DetailPage_BackRequested });
        systemNavigationManager.AppViewBackButtonVisibility(AppViewBackButtonVisibility::Visible);
    }

    void DetailPage::OnNavigatedFrom(NavigationEventArgs const&)
    {
        auto systemNavigationManager = SystemNavigationManager::GetForCurrentView();
        systemNavigationManager.BackRequested(m_backRequestedEventRegistrationToken);
        systemNavigationManager.AppViewBackButtonVisibility(AppViewBackButtonVisibility::Collapsed);
    }

    void DetailPage::OnBackRequested()
    {
        // Page above us will be our master view.
        // Make sure we are using the "drill out" animation in this transition.
        ReplaceLastBackStackEntryParameter(nullptr);
        Frame().GoBack(DrillInNavigationTransitionInfo());
    }

    // Doctor the navigation parameter for the master page so it
    // will show the correct item in the side-by-side view.
    void DetailPage::ReplaceLastBackStackEntryParameter(IInspectable const& parameter)
    {
        auto backStack = Frame().BackStack();
        auto backStackCount = backStack.Size();

        if (backStackCount > 0)
        {
            auto masterPageEntry = backStack.GetAt(backStackCount - 1);

            auto modifiedEntry = PageStackEntry(masterPageEntry.SourcePageType(), parameter, masterPageEntry.NavigationTransitionInfo());

            backStack.SetAt(backStackCount - 1, modifiedEntry);
        }
    }

    void DetailPage::NavigateBackForWideState(bool useTransition)
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

    bool DetailPage::ShouldGoToWideState()
    {
        return Window::Current().Bounds().Width >= 720.0;
    }

    void DetailPage::PageRoot_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        if (ShouldGoToWideState())
        {
            // We shouldn't see this page since we are in "wide master-detail" mode.
            // Play a transition as we are likely navigating back from a separate page.
            NavigateBackForWideState(/* useTransition */ true);
        }
        else
        {
            // Realize the main page content.
            [[maybe_unused]] auto rootPanel = FindName(L"RootPanel");
        }

        m_sizeChangedEventRegistrationToken = Window::Current().SizeChanged({ this, &DetailPage::Window_SizeChanged });
    }


    void DetailPage::PageRoot_Unloaded(IInspectable const&, RoutedEventArgs const&)
    {
        Window::Current().SizeChanged(m_sizeChangedEventRegistrationToken);
        m_sizeChangedEventRegistrationToken = {};
    }

    void DetailPage::Window_SizeChanged(IInspectable const&, WindowSizeChangedEventArgs const&)
    {
        if (ShouldGoToWideState())
        {
            // Make sure we are no longer listening to window change events.
            Window::Current().SizeChanged(m_sizeChangedEventRegistrationToken);
            m_sizeChangedEventRegistrationToken = {};

            // We shouldn't see this page since we are in "wide master-detail" mode.
            NavigateBackForWideState(/* useTransition */ false);
        }
    }

    void DetailPage::DetailPage_BackRequested(IInspectable const&, BackRequestedEventArgs const& args)
    {
        // Mark event as handled so we don't get bounced out of the app.
        args.Handled(true);

        OnBackRequested();
    }
}
