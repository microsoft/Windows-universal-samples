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

#include "pch.h"
#include "Scenario5_RoundingAndPadding.h"
#include "Scenario5_RoundingAndPadding.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::UI::Xaml;

namespace
{
    // Used to display the rounding algorithm in a string format.  Used by the different scenarios when 
    // displaying results for the scenario
    std::wstring DisplayRoundingAlgorithmAsString(RoundingAlgorithm roundingAlgorithm)
    {
        // Convert the enumeration value into a string representation
        switch (roundingAlgorithm) {
        case RoundingAlgorithm::None:
            return L"no";
            break;
        case RoundingAlgorithm::RoundAwayFromZero:
            return L"away from zero";
            break;
        case RoundingAlgorithm::RoundDown:
            return L"down";
            break;
        case RoundingAlgorithm::RoundHalfAwayFromZero:
            return L"half away from zero";
            break;
        case RoundingAlgorithm::RoundHalfDown:
            return L"half down";
            break;
        case RoundingAlgorithm::RoundHalfToEven:
            return L"half to even";
            break;
        case RoundingAlgorithm::RoundHalfToOdd:
            return L"half to odd";
            break;
        case RoundingAlgorithm::RoundHalfTowardsZero:
            return L"half towards zero";
            break;
        case RoundingAlgorithm::RoundHalfUp:
            return L"half up";
            break;
        case RoundingAlgorithm::RoundTowardsZero:
            return L"toward zero";
            break;
        case RoundingAlgorithm::RoundUp:
            return L"up";
            break;
        default:
            return L"unknown";
            break;
        }
    }

    // Executes the padding scenarios.
    /// The formatter parameter can be either a DecimalFormatter, PercentFormatter
    /// PerMilleFormatter or CurrencyFormatter
    void DoPaddingScenarios(std::wostream& results, INumberFormatter const& formatter)
    {
        // Display the properties of the scenario
        results << L"Padding with ";
        results << formatter.as<ISignificantDigitsOption>().SignificantDigits() << L" significant digits, ";
        results << formatter.as<INumberFormatterOptions>().IntegerDigits() << L" integer digits, ";
        results << formatter.as<INumberFormatterOptions>().FractionDigits() << L" fractional digits";
        results << std::endl << std::endl;

        // Iterate through the numbers to pad, format them and add them to the results
        static constexpr double numbersToPad[] = { 0.12, 1.2, 10.2, 102 };
        for (double numberToPad : numbersToPad)
        {
            hstring paddedNumber = formatter.Format(numberToPad);
            results << L"Value: " << numberToPad << L" Padded: " << std::wstring_view(paddedNumber) << std::endl;
        }

        // Add a carriage return at the end of the scenario for readability
        results << std::endl;
    }

    /// Demonstrates how to perform padding and rounding by using the DecimalFormatter class (can be any of the 
    /// formatter classes in the Windows.Globalization.Numberformatting namespace) and the IncrementNumberRounder
    /// to do so.  The SignificantDigitsNumberRounder can also be used instead of the latter.
    void DoPaddingAndRoundingScenarios(std::wostream& results, RoundingAlgorithm roundingAlgorithm, unsigned int significantDigits, int integerDigits, int fractionalDigits)
    {
        // Create the rounder and set the rounding algorithm provided as a parameter
        IncrementNumberRounder rounder;
        rounder.RoundingAlgorithm(roundingAlgorithm);

        // Create the formatter, set the padding properties provided as parameters and associate the rounder
        // we have just created
        DecimalFormatter formatter;
        formatter.SignificantDigits(significantDigits);
        formatter.IntegerDigits(integerDigits);
        formatter.FractionDigits(fractionalDigits);
        formatter.NumberRounder(rounder);

        // Iterate through the increments we have defined in the scenario
        static constexpr double incrementsToRound[] = { 0.001, 0.01, 0.1, 1.0 };
        for (double incrementToRound : incrementsToRound)
        {
            // Set the increment to round to
            rounder.Increment(incrementToRound);

            // Display the properties of the scenario
            results << L"Padding and rounding with ";
            results << formatter.SignificantDigits() << L" significant digits, ";
            results << formatter.IntegerDigits() << L" integer digits, ";
            results << formatter.FractionDigits() << L" fractional digits, ";
            results << rounder.Increment() << L" increment and ";
            results << DisplayRoundingAlgorithmAsString(rounder.RoundingAlgorithm()) << L" rounding algorithm";
            results << std::endl << std::endl;

            // Iterate through the numbers to round and pad, format them and add them to the results
            static constexpr double numbersToFormat[] = { 0.1458, 1.458, 14.58, 145.8 };
            for (double numberToFormat : numbersToFormat)
            {
                hstring formattedNumber = formatter.Format(numberToFormat);
                results << L"Value: " << numberToFormat << L" Formatted: " << std::wstring_view(formattedNumber) << std::endl;
            }

            // Add a carriage return at the end of the scenario for readability
            results << std::endl;
        }
    }

    /// This scenario illustrates how to provide a rounding algorithm to a CurrencyFormatter class by means of the 
    /// applyRoundingForCurrency method.  This will associate an IncrementNumberRounder using the same increment as the
    /// fractionDigits appropriate for the currency and language.  You can always choose to provide a different rounding 
    /// algorithm by setting the numberRounder property, as depicted in the doPaddingAndRoundingScenarios function.  
    /// The mechanism provided here is better suited for currencies.
    void DoCurrencyRoundingScenarios(std::wostream& results, hstring const& currencyCode, RoundingAlgorithm roundingAlgorithm)
    {
        // Create the currency formatter and set the rounding algorithm provided as a parameter
        CurrencyFormatter currencyFormatter(currencyCode);
        currencyFormatter.ApplyRoundingForCurrency(roundingAlgorithm);

        // Display the properties of the scenario
        results << L"Currency formatting for ";
        results << std::wstring_view(currencyFormatter.Currency()) << L" currency and using the ";
        results << DisplayRoundingAlgorithmAsString(roundingAlgorithm) << L" rounding algorithm";
        results << std::endl << std::endl;

        // Iterate through the numbers to round and add them to the results
        static constexpr double numbersToFormat[] = { 14.55, 3.345, 16.2, 175.8 };
        for (double numberToFormat : numbersToFormat)
        {
            hstring formattedNumber = currencyFormatter.Format(numberToFormat);
            results << L"Value: " << numberToFormat << L" Formatted: " << std::wstring_view(formattedNumber) << std::endl;
        }

        // Add a carriage return at the end of the scenario for readability
        results << std::endl;
    }

    /// Shows how to do number rounding using the IncrementNumberRounding class.
    void DoIncrementRoundingScenarios(std::wostream& results, RoundingAlgorithm roundingAlgorithm)
    {
        // Create the rounder and set the rounding algorithm provided as a parameter
        IncrementNumberRounder rounder;
        rounder.RoundingAlgorithm(roundingAlgorithm);

        // Formatter to display the rounded value.  It is recommended to use the increment number
        // rounder within a formatter object to avoid precision issues when displaying.
        DecimalFormatter formatter;
        formatter.NumberRounder(rounder);

        // Iterate through the increments we have defined in the scenario
        static constexpr double incrementsToRound[] = { 0.001, 0.01, 0.1, 1.0 };
        for (double incrementToRound : incrementsToRound)
        {
            // Set the significant digits to round to
            rounder.Increment(incrementToRound);

            // Display the properties of the scenario
            results << L"Rounding with ";
            results << rounder.Increment() << L" increment and ";
            results << DisplayRoundingAlgorithmAsString(rounder.RoundingAlgorithm()) << L" rounding algorithm";
            results << std::endl << std::endl;

            // Iterate through the numbers to round and add them to the results
            static constexpr double numbersToRound[] = { 0.1458, 1.458, 14.58, 145.8 };
            for (double numberToRound : numbersToRound)
            {
                results << L"Value: " << numberToRound << L" Rounded: " << std::wstring_view(formatter.Format(numberToRound)) << std::endl;
            }

            // Add a carriage return at the end of the scenario for readability
            results << std::endl;
        }
    }

    /// Shows how to do number rounding using the SignificantDigitsNumberRounder class.
    void DoSignificantDigitRoundingScenarios(std::wostream& results, RoundingAlgorithm roundingAlgorithm)
    {
        // Create the rounder and set the rounding algorithm provided as a parameter.
        SignificantDigitsNumberRounder rounder;
        rounder.RoundingAlgorithm(roundingAlgorithm);

        // Iterate through the significant digits we have defined in the scenario
        static constexpr unsigned int digitsToRound[] = { 3, 2, 1 };
        for (unsigned int digitToRound : digitsToRound)
        {
            // Set the significant digits to round to
            rounder.SignificantDigits(digitToRound);

            // Display the properties of the scenario
            results << L"Rounding with ";
            results << rounder.SignificantDigits() << L" significant digits and ";
            results << DisplayRoundingAlgorithmAsString(rounder.RoundingAlgorithm()) << L" rounding algorithm";
            results << std::endl << std::endl;

            // Iterate through the numbers to round and add them to the results
            static constexpr double numbersToRound[] = { 0.1458, 1.458, 14.58, 145.8 };
            for (double numberToRound : numbersToRound)
            {
                double roundedValue = rounder.RoundDouble(numberToRound);
                results << L"Value: " << numberToRound << L" Rounded: " << roundedValue << std::endl;
            }

            // Add a carriage return at the end of the scenario for readability
            results << std::endl;
        }
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario5_RoundingAndPadding::Scenario5_RoundingAndPadding()
    {
        InitializeComponent();
    }

    void Scenario5_RoundingAndPadding::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Variable where we keep results to display
        std::wostringstream results;

        // Create a decimal formatter used to do padding with only 4 significant digits.  We zero the
        // integerDigits and fractionDigits to only show the behavior of the significantDigits property
        // when formatting.
        DecimalFormatter decimalFormatter;
        decimalFormatter.SignificantDigits(4);
        decimalFormatter.IntegerDigits(0);
        decimalFormatter.FractionDigits(0);

        // Run through the scenarios with padding
        DoPaddingScenarios(results, decimalFormatter);

        // Create a percent formatter used to do padding with 4 significant digits, 2 minimum integer 
        // digits, 2 minimum factional digits.  This will show how the behavior of the significantDigits
        // property in conjunction with the integerDigits and fractionDigits properties.
        PercentFormatter percentFormatter;
        percentFormatter.SignificantDigits(4);
        percentFormatter.IntegerDigits(2);
        percentFormatter.FractionDigits(2);

        // Run through the scenarios
        DoPaddingScenarios(results, percentFormatter);

        // Do the significant rounding scenarios with round up algorithm 
        DoSignificantDigitRoundingScenarios(results, RoundingAlgorithm::RoundUp);

        // Do the significant rounding scenarios with round down algorithm
        DoSignificantDigitRoundingScenarios(results, RoundingAlgorithm::RoundDown);

        // Do the increment rounding scenarios with round half up algorithm
        DoIncrementRoundingScenarios(results, RoundingAlgorithm::RoundHalfUp);

        // Do the increment rounding scenarios with round half down algorithm
        DoIncrementRoundingScenarios(results, RoundingAlgorithm::RoundHalfDown);

        // Do currency formatting with rounding for Japanese Yen with round half to odd algorithm
        DoCurrencyRoundingScenarios(results, L"JPY", RoundingAlgorithm::RoundHalfToOdd);

        // Do currency formatting with rounding for Euro with round half to even algorithm
        DoCurrencyRoundingScenarios(results, L"EUR", RoundingAlgorithm::RoundHalfToEven);

        // Do padding and rounding scenarios using round half up algorithm
        DoPaddingAndRoundingScenarios(results, RoundingAlgorithm::RoundHalfUp, 4, 3, 2);

        // Do padding and rounding scenarios using round half down algorithm
        DoPaddingAndRoundingScenarios(results, RoundingAlgorithm::RoundHalfDown, 4, 3, 2);

        OutputTextBlock().Text(results.str());
    }
}
