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

#include "Scenario2_Stats.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario2_Stats : Scenario2_StatsT<Scenario2_Stats>
    {
        Scenario2_Stats();

        void ShowResults_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario2_Stats : Scenario2_StatsT<Scenario2_Stats, implementation::Scenario2_Stats>
    {
    };
}
