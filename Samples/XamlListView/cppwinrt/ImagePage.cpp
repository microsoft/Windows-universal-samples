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
#include "ImagePage.h"
#include "ImagePage.g.cpp"

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media::Animation;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    ImagePage::ImagePage()
    {
        InitializeComponent();
    }

    void ImagePage::OnNavigatedTo(NavigationEventArgs const&)
    {
        // Register for hardware and software back request from the system
        SystemNavigationManager systemNavigationManager = SystemNavigationManager::GetForCurrentView();
        backRequestedToken = systemNavigationManager.BackRequested({ get_weak(), &ImagePage::OnBackRequested });
        systemNavigationManager.AppViewBackButtonVisibility(AppViewBackButtonVisibility::Visible);
    }

    void ImagePage::OnNavigatedFrom(NavigationEventArgs const&)
    {
        SystemNavigationManager systemNavigationManager = SystemNavigationManager::GetForCurrentView();
        systemNavigationManager.BackRequested(backRequestedToken);
        systemNavigationManager.AppViewBackButtonVisibility(AppViewBackButtonVisibility::Collapsed);
    }

    void ImagePage::OnBackRequested(IInspectable const&, BackRequestedEventArgs const& e)
    {
        // Mark event as handled so we don't leave the app.
        e.Handled(true);
        // Page above us will be our master view.
        // Make sure we are using the "drill out" animation in this transition.
        Frame().GoBack(DrillInNavigationTransitionInfo());
    }

}
