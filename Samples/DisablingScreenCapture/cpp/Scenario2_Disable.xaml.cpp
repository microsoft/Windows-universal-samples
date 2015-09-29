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
#include "Scenario2_Disable.xaml.h"

using namespace SDKTemplate;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::ViewManagement;

Scenario2_Disable::Scenario2_Disable()
{
    this->InitializeComponent();
}

void Scenario2_Disable::OnNavigatedTo(NavigationEventArgs^ e)
{
    // Disable screen capture when the user navigates to this page.
    ApplicationView::GetForCurrentView()->IsScreenCaptureEnabled = false;
}

void Scenario2_Disable::OnNavigatedFrom(NavigationEventArgs^ e)
{
    // Re-enable screen capture when the user navigates away from this page.
    ApplicationView::GetForCurrentView()->IsScreenCaptureEnabled = true;
}
