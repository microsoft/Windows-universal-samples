// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario3_ReverseConversion.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario3_ReverseConversion sealed
    {
    public:
        Scenario3_ReverseConversion();
    private:
        void CreateGeneratorButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Execute_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        Windows::Data::Text::TextReverseConversionGenerator^ generator;
    };
}
