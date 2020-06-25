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

#include "Scenario1_PercentPermilleFormatting.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1_PercentPermilleFormatting : Scenario1_PercentPermilleFormattingT<Scenario1_PercentPermilleFormatting>
    {
        Scenario1_PercentPermilleFormatting();

        void Display_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1_PercentPermilleFormatting : Scenario1_PercentPermilleFormattingT<Scenario1_PercentPermilleFormatting, implementation::Scenario1_PercentPermilleFormatting>
    {
    };
}
