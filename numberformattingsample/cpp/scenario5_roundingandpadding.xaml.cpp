// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario5_RoundingAndPadding.xaml.h"

using namespace SDKTemplate;
using namespace NumberFormatting;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::NumberFormatting;

Scenario5_RoundingAndPadding::Scenario5_RoundingAndPadding()
{
    InitializeComponent();
}

/// <summary>
/// Used to display the rounding algorithm in a string format.  Used by the different scenarios when 
/// displaying results for the scenario
/// </summary>
/// <param name="roundingAlgorithm"></param>
String^ Scenario5_RoundingAndPadding::DisplayRoundingAlgorithmAsString(RoundingAlgorithm roundingAlgorithm) 
{
    String^ roundingAsString = "";

    // Convert the enumeration value into a string representation
    switch (roundingAlgorithm) {
        case RoundingAlgorithm::None:
            roundingAsString = "no";
            break;
        case RoundingAlgorithm::RoundAwayFromZero:
            roundingAsString = "away from zero";
            break;
        case RoundingAlgorithm::RoundDown:
            roundingAsString = "down";
            break;
        case RoundingAlgorithm::RoundHalfAwayFromZero:
            roundingAsString = "half away from zero";
            break;
        case RoundingAlgorithm::RoundHalfDown:
            roundingAsString = "half down";
            break;
        case RoundingAlgorithm::RoundHalfToEven:
            roundingAsString = "half to even";
            break;
        case RoundingAlgorithm::RoundHalfToOdd:
            roundingAsString = "half to odd";
            break;
        case RoundingAlgorithm::RoundHalfTowardsZero:
            roundingAsString = "half towards zero";
            break;
        case RoundingAlgorithm::RoundHalfUp:
            roundingAsString = "half up";
            break;
        case RoundingAlgorithm::RoundTowardsZero:
            roundingAsString = "toward zero";
            break;
        case RoundingAlgorithm::RoundUp:
            roundingAsString = "up";
            break;
        default:
            roundingAsString = "unknown";
            break;
    }

    return roundingAsString;
}

/// <summary>
/// Executes the padding scenarios.
/// </summary>
/// <param name="formatter">
/// The formatter parameter can be either a DecimalFormatter, PercentFormatter
/// PerMilleFormatter or CurrencyFormatter
/// </param>
String^ Scenario5_RoundingAndPadding::DoPaddingScenarios(INumberFormatter^ formatter) 
{
    // Display the properties of the scenario
    String^ results = "Padding with ";
    results = results + ((ISignificantDigitsOption^)formatter)->SignificantDigits + " significant digits, ";
    results = results + ((INumberFormatterOptions^)formatter)->IntegerDigits + " integer digits, ";
    results = results + ((INumberFormatterOptions^)formatter)->FractionDigits + " fractional digits\n\n";

    // Iterate through the numbers to pad, format them and add them to the results
    double numbersToPad[] = { 0.12, 1.2, 10.2, 102 };
    for (int index = 0; index < sizeof(numbersToPad)/sizeof(double); index++) 
    {
        double numberToPad = numbersToPad[index];
        String^ paddedNumber = formatter->Format(numberToPad);
        results = results + "Value: " + numberToPad + " Padded: " + paddedNumber + "\n";
    }

    // Add a carriage return at the end of the scenario for readability
    return results + "\n";
}

/// <summary>
/// Demonstrates how to perform padding and rounding by using the DecimalFormatter class (can be any of the 
/// formatter classes in the Windows.Globalization.Numberformatting namespace) and the IncrementNumberRounder
/// to do so.  The SignificantDigitsNumberRounder can also be used instead of the latter.
/// </summary>
/// <param name="roundingAlgorithm"></param>
/// <param name="significantDigits"></param>
/// <param name="integerDigits"></param>
/// <param name="fractionalDigits"></param>
String^ Scenario5_RoundingAndPadding::DoPaddingAndRoundingScenarios(RoundingAlgorithm roundingAlgorithm, unsigned int significantDigits, int integerDigits, int fractionalDigits) 
{
    // Create the rounder and set the rounding algorithm provided as a parameter
    IncrementNumberRounder^ rounder = ref new Windows::Globalization::NumberFormatting::IncrementNumberRounder();
    rounder->RoundingAlgorithm = roundingAlgorithm;

    // Create the formatter, set the padding properties provided as parameters and associate the rounder
    // we have just created
    DecimalFormatter^ formatter = ref new Windows::Globalization::NumberFormatting::DecimalFormatter();
    formatter->SignificantDigits = significantDigits;
    formatter->IntegerDigits = integerDigits;
    formatter->FractionDigits = fractionalDigits;
    formatter->NumberRounder = rounder;

    // Stores the results
    String^ results = "";

    // Iterate through the increments we have defined in the scenario
    double incrementsToRound[] = { 0.001, 0.01, 0.1, 1.0 };
    for (int incrementIndex=0; incrementIndex < sizeof(incrementsToRound)/sizeof(double); incrementIndex++) 
    {
        // Set the increment to round to
        rounder->Increment = incrementsToRound[incrementIndex];

        // Display the properties of the scenario
        results = results + "Padding and rounding with ";
        results = results + formatter->SignificantDigits + " significant digits, ";
        results = results + formatter->IntegerDigits + " integer digits, ";
        results = results + formatter->FractionDigits + " fractional digits, ";
        results = results + rounder->Increment + " increment and ";
        results = results + DisplayRoundingAlgorithmAsString(rounder->RoundingAlgorithm) + " rounding algorithm\n\n";

        // Iterate through the numbers to round and pad, format them and add them to the results
        double numbersToFormat[] = { 0.1458, 1.458, 14.58, 145.8 };
        for (int numberIndex = 0; numberIndex < sizeof(numbersToFormat)/sizeof(double); numberIndex++) 
        {
            double numberToFormat = numbersToFormat[numberIndex];
            String^ formattedNumber = formatter->Format(numberToFormat);
            results = results + "Value: " + numberToFormat + " Formatted: " + formattedNumber + "\n";
        }

        // Add a carriage return at the end of the scenario for readability
        results = results + "\n";
    }

    return results;
}

/// <summary>
/// This scenario illustrates how to provide a rounding algorithm to a CurrencyFormatter class by means of the 
/// applyRoundingForCurrency method.  This will associate an IncrementNumberRounder using the same increment as the
/// fractionDigits appropriate for the currency and language.  You can always choose to provide a different rounding 
/// algorithm by setting the numberRounder property, as depicted in the doPaddingAndRoundingScenarios function.  
/// The mechanism provided here is better suited for currencies.
/// </summary>
/// <param name="currencyCode"></param>
/// <param name="roundingAlgorithm"></param>
String^ Scenario5_RoundingAndPadding::DoCurrencyRoundingScenarios(String^ currencyCode, RoundingAlgorithm roundingAlgorithm) 
{
    // Create the currency formatter and set the rounding algorithm provided as a parameter
    CurrencyFormatter^ currencyFormatter = ref new Windows::Globalization::NumberFormatting::CurrencyFormatter(currencyCode);
    currencyFormatter->ApplyRoundingForCurrency(roundingAlgorithm);

    // Display the properties of the scenario
    String^ results = "Currency formatting for ";
    results = results + currencyFormatter->Currency + " currency and using the ";
    results = results + DisplayRoundingAlgorithmAsString(roundingAlgorithm) + " rounding algorithm\n\n";

    // Iterate through the numbers to round and add them to the results
    double numbersToFormat[] = { 14.55, 3.345, 16.2, 175.8 };
    for (int numberIndex = 0; numberIndex < sizeof(numbersToFormat)/sizeof(double); numberIndex++) 
    {
        double numberToFormat = numbersToFormat[numberIndex];
        String^ formattedNumber = currencyFormatter->Format(numberToFormat);
        results = results + "Value: " + numberToFormat + " Formatted: " + formattedNumber + "\n";
    }

    // Add a carriage return at the end of the scenario for readability
    return results + "\n";
}

/// <summary>
/// Shows how to do number rounding using the IncrementNumberRounding class.
///</summary>
///<param name="roundingAlgorithm"></param>
String^ Scenario5_RoundingAndPadding::DoIncrementRoundingScenarios(RoundingAlgorithm roundingAlgorithm) 
{
    // Create the rounder and set the rounding algorithm provided as a parameter
    IncrementNumberRounder^ rounder = ref new Windows::Globalization::NumberFormatting::IncrementNumberRounder();
    rounder->RoundingAlgorithm = roundingAlgorithm;

    // Formatter to display the rounded value.  It is recommended to use the increment number
    // rounder within a formatter object to avoid precision issues when displaying.
    DecimalFormatter^ formatter = ref new Windows::Globalization::NumberFormatting::DecimalFormatter();
    formatter->NumberRounder = rounder;

    // Stores the results
    String^ results = "";

    // Iterate through the increments we have defined in the scenario
    double incrementsToRound[] = { 0.001, 0.01, 0.1, 1.0 };
    for (int incrementIndex = 0; incrementIndex < sizeof(incrementsToRound)/sizeof(double); incrementIndex++) 
    {
        // Set the significant digits to round to
        rounder->Increment = incrementsToRound[incrementIndex];

        // Display the properties of the scenario
        results = results + "Rounding with ";
        results = results + rounder->Increment + " increment and ";
        results = results + DisplayRoundingAlgorithmAsString(rounder->RoundingAlgorithm) + " rounding algorithm\n\n";

        // Iterate through the numbers to round and add them to the results
        double numbersToRound[] = { 0.1458, 1.458, 14.58, 145.8 };
        for (int numberIndex = 0; numberIndex < sizeof(numbersToRound)/sizeof(double); numberIndex++) 
        {
            double numberToRound = numbersToRound[numberIndex];
            results = results + "Value: " + numberToRound + " Rounded: " + formatter->Format(numberToRound) + "\n";
        }

        // Add a carriage return at the end of the scenario for readability
        results = results + "\n";
    }

    return results;
}

/// <summary>
/// Shows how to do number rounding using the SignificantDigitsNumberRounder class.
/// </summary>
/// <param name="roundingAlgorithm"></param>
String^ Scenario5_RoundingAndPadding::DoSignificantDigitRoundingScenarios(RoundingAlgorithm roundingAlgorithm) 
{
    // Create the rounder and set the rounding algorithm provided as a parameter.
    SignificantDigitsNumberRounder^ rounder = ref new Windows::Globalization::NumberFormatting::SignificantDigitsNumberRounder();
    rounder->RoundingAlgorithm = roundingAlgorithm;

    // Stores the results
    String^ results = "";

    // Iterate through the significant digits we have defined in the scenario
    unsigned int digitsToRound[] = { 3, 2, 1 };
    for (int digitIndex = 0; digitIndex < sizeof(digitsToRound)/sizeof(unsigned int); digitIndex++) 
    {
        // Set the significant digits to round to
        rounder->SignificantDigits = digitsToRound[digitIndex];

        // Display the properties of the scenario
        results = results + "Rounding with ";
        results = results + rounder->SignificantDigits + " significant digits and ";
        results = results + DisplayRoundingAlgorithmAsString(rounder->RoundingAlgorithm) + " rounding algorithm\n\n";

        // Iterate through the numbers to round and add them to the results
        double numbersToRound[] = { 0.1458, 1.458, 14.58, 145.8 };
        for (int numberIndex = 0; numberIndex < sizeof(numbersToRound)/sizeof(double); numberIndex++)
        {
            double numberToRound = numbersToRound[numberIndex];
            double roundedValue = rounder->RoundDouble(numberToRound);
            results = results + "Value: " + numberToRound + " Rounded: " + roundedValue + "\n";
        }

        // Add a carriage return at the end of the scenario for readability
        results = results + "\n";
    }

    return results;
}

/// <summary>
/// This is the click handler for the 'Display' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario5_RoundingAndPadding::Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // Variable where we keep results to display
    String^ results = "";

    // Create a decimal formatter used to do padding with only 4 significant digits.  We zero the
    // integerDigits and fractionDigits to only show the behavior of the significantDigits property
    // when formatting.
    DecimalFormatter^ decimalFormatter = ref new DecimalFormatter();
    decimalFormatter->SignificantDigits = 4;
    decimalFormatter->IntegerDigits = 0;
    decimalFormatter->FractionDigits = 0;

    // Run through the scenarios with padding
    results = results + DoPaddingScenarios(decimalFormatter);

    // Create a percent formatter used to do padding with 4 significant digits, 2 minimum integer 
    // digits, 2 minimum factional digits.  This will show how the behavior of the significantDigits
    // property in conjunction with the integerDigits and fractionDigits properties.
    PercentFormatter^ percentFormatter = ref new PercentFormatter();
    percentFormatter->SignificantDigits = 4;
    percentFormatter->IntegerDigits = 2;
    percentFormatter->FractionDigits = 2;

    // Run through the scenarios
    results = results + DoPaddingScenarios(percentFormatter);

    // Do the significant rounding scenarios with round up algorithm 
    results = results + DoSignificantDigitRoundingScenarios(RoundingAlgorithm::RoundUp);

    // Do the significant rounding scenarios with round down algorithm
    results = results + DoSignificantDigitRoundingScenarios(RoundingAlgorithm::RoundDown);

    // Do the increment rounding scenarios with round half up algorithm
    results = results + DoIncrementRoundingScenarios(RoundingAlgorithm::RoundHalfUp);

    // Do the increment rounding scenarios with round half down algorithm
    results = results + DoIncrementRoundingScenarios(RoundingAlgorithm::RoundHalfDown);

    // Do currency formatting with rounding for Japanese Yen with round half to odd algorithm
    results = results + DoCurrencyRoundingScenarios("JPY", RoundingAlgorithm::RoundHalfToOdd);

    // Do currency formatting with rounding for Euro with round half to even algorithm
    results = results + DoCurrencyRoundingScenarios("EUR", RoundingAlgorithm::RoundHalfToEven);

    // Do padding and rounding scenarios using round half up algorithm
    results = results + DoPaddingAndRoundingScenarios(RoundingAlgorithm::RoundHalfUp, 4, 3, 2);

    // Do padding and rounding scenarios using round half down algorithm
    results = results + DoPaddingAndRoundingScenarios(RoundingAlgorithm::RoundHalfDown, 4, 3, 2);

    OutputTextBlock->Text = results;
}
