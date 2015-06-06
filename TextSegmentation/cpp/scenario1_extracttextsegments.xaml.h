// Copyright (c) Microsoft. All rights reserved.

#pragma once
#include "Scenario1_ExtractTextSegments.g.h"

namespace TextSegmentation
{
    public ref class Scenario1_ExtractTextSegments sealed
    {
    public:
        Scenario1_ExtractTextSegments();

    private:
        void WordSegmentsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void SelectionSegmentsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
