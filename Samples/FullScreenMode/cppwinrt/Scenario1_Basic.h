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

#pragma once

#include "Scenario1_Basic.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_Basic : Scenario1_BasicT<Scenario1_Basic>
    {
        Scenario1_Basic();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        void ToggleFullScreenModeButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void ShowStandardSystemOverlaysButton_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void UseMinimalOverlaysCheckBox_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        SDKTemplate::MainPage rootPage{ MainPage::Current() };

        // What is the program's last known full-screen state?
        // We use this to detect when the system forced us out of full-screen mode.
        bool isLastKnownFullScreen = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().IsFullScreenMode();

        event_token windowResizeToken;
        event_token keyDownToken;

        void UpdateContent();
        void RootPage_OnKeyDown(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_Basic : Scenario1_BasicT<Scenario1_Basic, implementation::Scenario1_Basic>
    {
    };
}
