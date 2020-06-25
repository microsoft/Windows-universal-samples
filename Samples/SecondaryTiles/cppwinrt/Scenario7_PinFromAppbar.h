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

#include "Scenario7_PinFromAppbar.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario7_PinFromAppbar : Scenario7_PinFromAppbarT<Scenario7_PinFromAppbar>
    {
        Scenario7_PinFromAppbar();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        fire_and_forget PinToAppBar_Click(Windows::Foundation::IInspectable sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        // Don't use NotifyUser on this page because it conflicts with the AppBar.
        event_token windowActivatedEventToken;

    private:
        void Window_Activated(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::WindowActivatedEventArgs const& e);
        void UpdateAppBarButton();
        void BottomAppBar_Opened(Windows::Foundation::IInspectable const& sender, Windows::Foundation::IInspectable const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario7_PinFromAppbar : Scenario7_PinFromAppbarT<Scenario7_PinFromAppbar, implementation::Scenario7_PinFromAppbar>
    {
    };
}
