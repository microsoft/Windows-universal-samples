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
#include "Scenario2_Disable.h"
#include "Scenario2_Disable.g.cpp"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Navigation;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Disable::Scenario2_Disable()
    {
        InitializeComponent();
    }

    void Scenario2_Disable::OnNavigatedTo(NavigationEventArgs const&)
    {
        // Disable screen capture when the user navigates to this page.  
        Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().IsScreenCaptureEnabled(false);
    }

    void Scenario2_Disable::OnNavigatedFrom(NavigationEventArgs const&)
    {
        // Re-enable screen capture when the user navigates away from this page.
        Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().IsScreenCaptureEnabled(true);
    }
}
