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

#include "Scenario3_CustomItems.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3_CustomItems : Scenario3_CustomItemsT<Scenario3_CustomItems>
    {
        Scenario3_CustomItems();

        // These methods are public so they can be called by binding.
        void DisplayNameLocalizedResource_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void DescriptionLocalizedResource_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void GroupNameLocalizedResource_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget AddToJumpList_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3_CustomItems : Scenario3_CustomItemsT<Scenario3_CustomItems, implementation::Scenario3_CustomItems>
    {
    };
}
