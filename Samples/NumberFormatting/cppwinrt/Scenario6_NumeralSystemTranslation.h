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

#include "Scenario6_NumeralSystemTranslation.g.h"

namespace winrt::SDKTemplate::implementation
{
    struct Scenario6_NumeralSystemTranslation : Scenario6_NumeralSystemTranslationT<Scenario6_NumeralSystemTranslation>
    {
        Scenario6_NumeralSystemTranslation();

        void Display_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SDKTemplate::factory_implementation
{
    struct Scenario6_NumeralSystemTranslation : Scenario6_NumeralSystemTranslationT<Scenario6_NumeralSystemTranslation, implementation::Scenario6_NumeralSystemTranslation>
    {
    };
}
