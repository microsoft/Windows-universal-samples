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
#include "pch.h"

namespace winrt::SDKTemplate
{
    struct unique_disabled_control
    {
        unique_disabled_control(Windows::UI::Xaml::Controls::Control const& control) : c{ control }
        {
            c.IsEnabled(false);
        }

        unique_disabled_control(const unique_disabled_control&) = delete;
        void operator=(unique_disabled_control const&) = delete;

        ~unique_disabled_control()
        {
            c.IsEnabled(true);
        }
        Windows::UI::Xaml::Controls::Control c;
    };

    bool TryParseFloat(hstring const& str, float& result);
}
