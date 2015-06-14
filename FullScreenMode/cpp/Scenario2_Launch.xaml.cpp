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
#include "Scenario2_Launch.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

Scenario2_Launch::Scenario2_Launch()
{
    InitializeComponent();
    LaunchInFullScreenModeCheckBox->IsChecked = ApplicationView::PreferredLaunchWindowingMode == ApplicationViewWindowingMode::FullScreen;
}


void Scenario2_Launch::LaunchInFullScreenModeCheckBox_Click(Platform::Object^ sender, RoutedEventArgs^ e)
{
    ApplicationView::PreferredLaunchWindowingMode = LaunchInFullScreenModeCheckBox->IsChecked->Value ? ApplicationViewWindowingMode::FullScreen : ApplicationViewWindowingMode::Auto;
}
