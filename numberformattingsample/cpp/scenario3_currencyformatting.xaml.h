// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "pch.h"
#include "Scenario3_CurrencyFormatting.g.h"

namespace NumberFormatting
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_CurrencyFormatting sealed
    {
    public:
        Scenario3_CurrencyFormatting();

    private:
        void Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
