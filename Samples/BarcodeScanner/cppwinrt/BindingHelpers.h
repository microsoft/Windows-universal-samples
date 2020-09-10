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

#include "BindingHelpers.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct BindingHelpers
    {
        static bool Not(bool value)
        {
            return !value;
        }

        static winrt::Windows::UI::Xaml::Visibility CollapsedIf(bool value)
        {
            return value ? winrt::Windows::UI::Xaml::Visibility::Collapsed : winrt::Windows::UI::Xaml::Visibility::Visible;
        }
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct BindingHelpers : BindingHelpersT<BindingHelpers, implementation::BindingHelpers>
    {
    };
}
