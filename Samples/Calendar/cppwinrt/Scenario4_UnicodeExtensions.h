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

#include "Scenario4_UnicodeExtensions.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario4_UnicodeExtensions : Scenario4_UnicodeExtensionsT<Scenario4_UnicodeExtensions>
    {
        Scenario4_UnicodeExtensions();

        void ShowResults_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario4_UnicodeExtensions : Scenario4_UnicodeExtensionsT<Scenario4_UnicodeExtensions, implementation::Scenario4_UnicodeExtensions>
    {
    };
}
