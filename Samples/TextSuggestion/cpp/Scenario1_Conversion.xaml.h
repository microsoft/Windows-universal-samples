// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario1_Conversion.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Conversion sealed
    {
    public:
        Scenario1_Conversion();
    private:
        MainPage^ rootPage;

        void CreateGeneratorButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Methods_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void Execute_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void DisplayMultipleCandidates(Windows::Foundation::Collections::IVectorView<Platform::String^>^ candidates);

        Windows::Data::Text::TextConversionGenerator^ generator;
    };
}

