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
#include "Scenario6_NumeralSystemTranslation.h"
#include "Scenario6_NumeralSystemTranslation.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario6_NumeralSystemTranslation::Scenario6_NumeralSystemTranslation()
    {
        InitializeComponent();
    }

    void Scenario6_NumeralSystemTranslation::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Text for which translation of the numeral system will be performed.  Please note that translation only happens between
        // Latin and any other supported numeral system.  Translation between numeral systems is not a supported scenario.
        std::wstring stringToTranslate = L"These are the 10 digits of a numeral system: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9";

        // Variable where we keep the results of the scenario
        std::wostringstream results;
        results << L"Original string: " << stringToTranslate << std::endl << std::endl;

        // The numeral system translator is initialized based on the current application language.
        NumeralSystemTranslator numeralTranslator;

        // Do translation
        results << L"Using application settings (" << std::wstring_view(numeralTranslator.NumeralSystem()) << L"): ";
        results << std::wstring_view(numeralTranslator.TranslateNumerals(stringToTranslate)) << std::endl;

        // Switch to a different numeral system
        numeralTranslator.NumeralSystem(L"hanidec");

        // Do translation
        results << "Using numeral system via property (" << std::wstring_view(numeralTranslator.NumeralSystem()) << L" ): ";
        results << std::wstring_view(numeralTranslator.TranslateNumerals(stringToTranslate)) << std::endl;

        // Create a converter using a language list to initialize the numeral system to an appropriate default
        numeralTranslator = NumeralSystemTranslator({ L"ar-SA", L"en-US" });

        // Do translation
        results << L"Using numeral system via language list (" << std::wstring_view(numeralTranslator.NumeralSystem()) << L"): ";
        results << std::wstring_view(numeralTranslator.TranslateNumerals(stringToTranslate)) << std::endl;

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
