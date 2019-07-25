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

#include "Scenario4_CompactOverlay.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_CompactOverlay : Scenario4_CompactOverlayT<Scenario4_CompactOverlay>
    {
        Scenario4_CompactOverlay();

        fire_and_forget OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        fire_and_forget ShowWindowBtn_Click(IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::UI::WindowManagement::AppWindow appWindow{ nullptr };
        Windows::UI::Xaml::Controls::Frame appWindowFrame{};

        void OnWindowClosed(Windows::UI::WindowManagement::AppWindow const& sender, IInspectable const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_CompactOverlay : Scenario4_CompactOverlayT<Scenario4_CompactOverlay, implementation::Scenario4_CompactOverlay>
    {
    };
}
