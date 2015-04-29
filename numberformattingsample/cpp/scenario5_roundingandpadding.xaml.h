// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "pch.h"
#include "Scenario5_RoundingAndPadding.g.h"

namespace NumberFormatting
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario5_RoundingAndPadding sealed
    {
    public:
        Scenario5_RoundingAndPadding();

    private:
        // Class methods that implement the scenarios
        Platform::String^ DisplayRoundingAlgorithmAsString(Windows::Globalization::NumberFormatting::RoundingAlgorithm roundingAlgorithm); 
        Platform::String^ DoPaddingScenarios(Windows::Globalization::NumberFormatting::INumberFormatter^ formatter);
        Platform::String^ DoPaddingAndRoundingScenarios(Windows::Globalization::NumberFormatting::RoundingAlgorithm roundingAlgorithm, unsigned int significantDigits, int integerDigits, int fractionalDigits);
        Platform::String^ DoCurrencyRoundingScenarios(Platform::String^ currencyCode, Windows::Globalization::NumberFormatting::RoundingAlgorithm roundingAlgorithm);
        Platform::String^ DoIncrementRoundingScenarios(Windows::Globalization::NumberFormatting::RoundingAlgorithm roundingAlgorithm);
        Platform::String^ DoSignificantDigitRoundingScenarios(Windows::Globalization::NumberFormatting::RoundingAlgorithm roundingAlgorithm);

        void Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
