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
#include "Scenario2_Launch.h"
#include "Scenario2_Launch.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Launch::Scenario2_Launch()
    {
        InitializeComponent();
        LaunchAtSize().IsChecked(ApplicationView::PreferredLaunchWindowingMode() != ApplicationViewWindowingMode::Auto);
    }

    // If you set the PreferredLaunchViewSize and PreferredLaunchWindowingMode
    // before calling Window.Current.Activate(), then it will take effect
    // on the current launch. Otherwise, it takes effect at the next launch.
    void Scenario2_Launch::LaunchAtSize_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (LaunchAtSize().IsChecked().Value())
        {
            // For best results, set the PreferredLaunchViewSize before setting
            // the PreferredLaunchWindowingMode.
            ApplicationView::PreferredLaunchViewSize({ 600, 500 });
            ApplicationView::PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);
        }
        else
        {
            ApplicationView::PreferredLaunchWindowingMode(ApplicationViewWindowingMode::Auto);
        }
    }
}
