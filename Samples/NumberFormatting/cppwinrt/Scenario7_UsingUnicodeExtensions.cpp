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
#include "Scenario7_UsingUnicodeExtensions.h"
#include "Scenario7_UsingUnicodeExtensions.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario7_UsingUnicodeExtensions::Scenario7_UsingUnicodeExtensions()
    {
        InitializeComponent();
    }

    void Scenario7_UsingUnicodeExtensions::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses language tags with unicode extensions to construct and use the various 
        // formatters and NumeralSystemTranslator in Windows.Globalization.NumberFormatting to format numbers 

        // Variable where we keep results to display
        std::wostringstream results;

        // Create number to format.
        float randomNumber = (float)rand() / RAND_MAX * 100000.0f;
        DecimalFormatter randomNumberFormatter;
        hstring randomNumberFormatted = randomNumberFormatter.Format(randomNumber);

        results << L"Random number used by formatters: " << std::wstring_view(randomNumberFormatted) << std::endl;

        // Create a string to translate
        std::wstring stringToTranslate = L"These are the 10 digits of a numeral system: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9";
        results << L"String used by NumeralSystemTranslater: " << stringToTranslate << std::endl;
        // Create the rounder and set its increment to 0.01
        IncrementNumberRounder rounder;
        rounder.Increment(0.001);
        results << L"CurrencyFormatter will be using Euro symbol and all formatters rounding to 0.001 increments" << std::endl << std::endl;

        // The list of language tags with unicode extensions we want to test
        std::vector<hstring> languages{ L"de-DE-u-nu-telu-ca-buddist-co-phonebk-cu-usd", L"ja-jp-u-nu-arab" };

        // Create the various formatters, now using the language list with unicode extensions
        results << L"Creating formatters using following languages in the language list:" << std::endl;
        for (auto&& language : languages)
        {
            results << "\t" << std::wstring_view(language) << std::endl;
        }
        results << std::endl;

        // Create the various formatters.
        DecimalFormatter decimalFormatter(languages, L"ZZ");
        decimalFormatter.NumberRounder(rounder); decimalFormatter.FractionDigits(2);
        CurrencyFormatter currencyFormatter(CurrencyIdentifiers::EUR(), languages, L"ZZ");
        currencyFormatter.NumberRounder(rounder); currencyFormatter.FractionDigits(2);
        PercentFormatter percentFormatter(languages, L"ZZ");
        percentFormatter.NumberRounder(rounder); percentFormatter.FractionDigits(2);
        PermilleFormatter permilleFormatter(languages, L"ZZ");
        permilleFormatter.NumberRounder(rounder); permilleFormatter.FractionDigits(2);
        NumeralSystemTranslator numeralTranslator(languages);

        results << L"Using resolved language  " << std::wstring_view(decimalFormatter.ResolvedLanguage()) <<
            L"  : (note that all extension tags other than nu are ignored)" << std::endl;

        // Format using formatters and translate using NumeralSystemTranslator.
        results << L"Decimal Formatter:   " << std::wstring_view(decimalFormatter.Format(randomNumber)) << std::endl;
        results << L"Currency formatted:   " << std::wstring_view(currencyFormatter.Format(randomNumber)) << std::endl;
        results << L"Percent formatted:   " << std::wstring_view(percentFormatter.Format(randomNumber)) << std::endl;
        results << L"Permille formatted:   " << std::wstring_view(permilleFormatter.Format(randomNumber)) << std::endl;
        results << L"NumeralTranslator translated:   " << std::wstring_view(numeralTranslator.TranslateNumerals(stringToTranslate)) << std::endl;

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
