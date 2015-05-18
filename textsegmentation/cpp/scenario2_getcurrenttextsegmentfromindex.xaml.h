// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "Scenario2_GetCurrentTextSegmentFromIndex.g.h"

namespace TextSegmentation
{
    public ref class Scenario2_GetCurrentTextSegmentFromIndex sealed
    {
    public:
        Scenario2_GetCurrentTextSegmentFromIndex();

    private:
        void WordSegmentButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SelectionSegmentButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
