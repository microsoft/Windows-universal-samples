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

#include "Scenario4_Override.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_Override : Scenario4_OverrideT<Scenario4_Override>
    {
        Scenario4_Override();

        void Display_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_Override : Scenario4_OverrideT<Scenario4_Override, implementation::Scenario4_Override>
    {
    };
}
