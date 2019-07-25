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
        LaunchInFullScreenModeCheckBox().IsChecked(ApplicationView::PreferredLaunchWindowingMode() == ApplicationViewWindowingMode::FullScreen);

    }

    void Scenario2_Launch::LaunchInFullScreenModeCheckBox_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ApplicationView::PreferredLaunchWindowingMode(LaunchInFullScreenModeCheckBox().IsChecked().Value() ? ApplicationViewWindowingMode::FullScreen : ApplicationViewWindowingMode::Auto);
    }
}
