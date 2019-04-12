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

#include "Scenario3.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario3 : Scenario3T<Scenario3>
    {
        Scenario3();
        fire_and_forget GetFilesButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

    private:
        static hstring GetPropertyDisplayValue(Windows::Foundation::IInspectable const& rawValue);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario3 : Scenario3T<Scenario3, implementation::Scenario3>
    {
    };
}
