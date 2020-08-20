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

#include "Scenario2_ShowHideEvents.g.h"
#include "MainPage.h"

namespace winrt::SDKTemplate::implementation
{
    /// <summary>
    /// Sample page to subscribe show/hide event of Touch Keyboard.
    /// </summary>
    struct Scenario2_ShowHideEvents : Scenario2_ShowHideEventsT<Scenario2_ShowHideEvents>
    {
        Scenario2_ShowHideEvents();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        event_token showingEventToken{};
        event_token hidingEventToken{};

        void OnShowing(Windows::UI::ViewManagement::InputPane const& sender, Windows::UI::ViewManagement::InputPaneVisibilityEventArgs const& e);
        void OnHiding(Windows::UI::ViewManagement::InputPane const& sender, Windows::UI::ViewManagement::InputPaneVisibilityEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_ShowHideEvents : Scenario2_ShowHideEventsT<Scenario2_ShowHideEvents, implementation::Scenario2_ShowHideEvents>
    {
    };
}
