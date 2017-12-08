// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "Scenario2_Prediction.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario2_Prediction sealed
    {
    public:
        Scenario2_Prediction();
    private:
        MainPage^ rootPage;

        void CreateGeneratorButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Methods_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
        void Execute_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void DisplayMultipleCandidates(Windows::Foundation::Collections::IVectorView<Platform::String^>^ candidates);

        Windows::Data::Text::TextPredictionGenerator^ generator;
    };
}
