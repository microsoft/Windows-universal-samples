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

#include "Scenario1.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario1 : Scenario1T<Scenario1>
    {
        Scenario1();

        void
        DoWin81Mode(
            Windows::Foundation::IInspectable const& sender,
            Windows::UI::Xaml::RoutedEventArgs const& args);

        void
        DoWin10Mode(
            Windows::Foundation::IInspectable const& sender,
            Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario1 : Scenario1T<Scenario1, implementation::Scenario1>
    {
    };
}
