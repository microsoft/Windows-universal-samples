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

#include "Scenario4_ChangeSystemGroup.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_ChangeSystemGroup : Scenario4_ChangeSystemGroupT<Scenario4_ChangeSystemGroup>
    {
        Scenario4_ChangeSystemGroup();

        fire_and_forget OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        // These methods are public so they can be called by binding.
        fire_and_forget SystemGroup_Recent_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget SystemGroup_Frequent_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget SystemGroup_None_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        fire_and_forget PrepareSampleFiles_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_ChangeSystemGroup : Scenario4_ChangeSystemGroupT<Scenario4_ChangeSystemGroup, implementation::Scenario4_ChangeSystemGroup>
    {
    };
}
