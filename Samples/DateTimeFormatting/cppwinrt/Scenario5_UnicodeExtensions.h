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

#include "Scenario5_UnicodeExtensions.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario5_UnicodeExtensions : Scenario5_UnicodeExtensionsT<Scenario5_UnicodeExtensions>
    {
        Scenario5_UnicodeExtensions();

        void Display_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario5_UnicodeExtensions : Scenario5_UnicodeExtensionsT<Scenario5_UnicodeExtensions, implementation::Scenario5_UnicodeExtensions>
    {
    };
}
