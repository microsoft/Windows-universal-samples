// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "pch.h"
#include "Scenario2_DecimalFormatting.g.h"

namespace NumberFormatting
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_DecimalFormatting sealed
    {
    public:
        Scenario2_DecimalFormatting();

    private:
        void Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
