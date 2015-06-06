// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario7_UsingUnicodeExtensions.xaml.h"

using namespace SDKTemplate;
using namespace NumberFormatting;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::NumberFormatting;

Scenario7_UsingUnicodeExtensions::Scenario7_UsingUnicodeExtensions()
{
    InitializeComponent();
}

/// <summary>
/// This is the click handler for the 'Display' button.
/// </summary>
/// <param name="sender"></param>
/// <param name="e"></param>
void Scenario7_UsingUnicodeExtensions::Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // This scenario uses language tags with unicode extensions to construct and use the various 
    // formatters and NumeralSystemTranslator in Windows.Globalization.NumberFormatting to format numbers 

    // Variable where we keep results to display
    String^ results = "";

    // Create number to format.
    float randomNumber = (float)rand() / RAND_MAX * 100000.0f;
    DecimalFormatter^ randomNumberFormatter = ref new DecimalFormatter();
    String^ randomNumberFormatted = randomNumberFormatter->Format(randomNumber);

    results = results + "Random number used by formatters: " + randomNumberFormatted + "\n";

    // Create a string to translate
    String^ stringToTranslate = "These are the 10 digits of a numeral system: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9";
    results = results + "String used by NumeralSystemTranslater: " + stringToTranslate + "\n";
    // Create the rounder and set its increment to 0.01
    IncrementNumberRounder^ rounder = ref new IncrementNumberRounder();
    rounder->Increment = 0.001;
    results = results + "CurrencyFormatter will be using Euro symbol and all formatters rounding to 0.001 increments\n\n";
    
    // The list of language tags with unicode extensions we want to test
    auto languages = ref new Platform::Collections::Vector<String^>();
    languages->Append("de-DE-u-nu-telu-ca-buddist-co-phonebk-cu-usd");
    languages->Append("ja-jp-u-nu-arab");
        
    // Create the various formatters, now using the language list with unicode extensions
    results = results + "Creating formatters using following languages in the language list: \n";
    for(unsigned int i =0; i<languages->Size; i++)
    {
        results = results + "\t" + languages->GetAt(i) + "\n";
    }
    results = results + "\n";

    // Create the various formatters.
    DecimalFormatter^ decimalFormatter = ref new DecimalFormatter(languages, "ZZ");
    decimalFormatter->NumberRounder = rounder; decimalFormatter->FractionDigits = 2;
    CurrencyFormatter^ currencyFormatter = ref new CurrencyFormatter(CurrencyIdentifiers::EUR, languages, "ZZ");
    currencyFormatter->NumberRounder = rounder; currencyFormatter->FractionDigits = 2;
    PercentFormatter^ percentFormatter = ref new PercentFormatter(languages, "ZZ");
    percentFormatter->NumberRounder = rounder; percentFormatter->FractionDigits = 2;
    PermilleFormatter^ permilleFormatter = ref new PermilleFormatter(languages, "ZZ");
    permilleFormatter->NumberRounder = rounder; permilleFormatter->FractionDigits = 2;
    NumeralSystemTranslator^ numeralTranslator = ref new NumeralSystemTranslator(languages);
    
    results = results + "Using resolved language  " + decimalFormatter->ResolvedLanguage + "  : (note that all extension tags other than nu are ignored)\n";
    // Format using formatters and translate using NumeralSystemTranslator.
    results = results + "Decimal Formatter:   " + decimalFormatter->Format(randomNumber) + "\n";
    results = results + "Currency formatted:   " + currencyFormatter->Format(randomNumber) + "\n";
    results = results + "Percent formatted:   " + percentFormatter->Format(randomNumber) + "\n";
    results = results + "Permille formatted:   " + permilleFormatter->Format(randomNumber) + "\n";
    results = results + "NumeralTranslator translated:   " + numeralTranslator->TranslateNumerals(stringToTranslate) + "\n";
        
    // Display the results.
    OutputTextBlock->Text = results;
}
