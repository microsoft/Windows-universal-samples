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
#include "Scenario2_DecimalFormatting.h"
#include "Scenario2_DecimalFormatting.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario2_DecimalFormatting::Scenario2_DecimalFormatting()
    {
        InitializeComponent();
    }

    void Scenario2_DecimalFormatting::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.NumberFormatting.DecimalFormatter class
        // to format a number.

        // Create formatter initialized using the current user's preference settings.
        DecimalFormatter decimalFormat;

        // Make a random number.
        float randomNumber = (float)rand() / RAND_MAX * 100000.0f;

        // Format with the user's default preferences.
        hstring decimal = decimalFormat.Format(randomNumber);

        // Format with grouping.
        DecimalFormatter decimalFormat1;
        decimalFormat1.IsGrouped(true);
        hstring decimal1 = decimalFormat1.Format(randomNumber);

        // Format with grouping using French.
        DecimalFormatter decimalFormatFR({ L"fr-FR" }, L"ZZ");
        decimalFormatFR.IsGrouped(true);
        hstring decimalFR = decimalFormatFR.Format(randomNumber);

        // Illustrate how automatic digit substitution works
        DecimalFormatter decimalFormatAR({ L"ar" }, L"ZZ");
        hstring decimalAR = decimalFormatAR.Format(randomNumber);

        // Get a fixed number.
        long long fixedNumber = 500;

        // Format with the user's default preferences.
        hstring decimal2 = decimalFormat.Format(fixedNumber);

        // Format always with a decimal point.
        DecimalFormatter decimalFormat3;
        decimalFormat3.IsDecimalPointAlwaysDisplayed(true);
        decimalFormat3.FractionDigits(0);
        hstring decimal3 = decimalFormat3.Format(fixedNumber);

        // Format with no fractional digits or decimal point.
        DecimalFormatter decimalFormat4;
        decimalFormat4.FractionDigits(0);
        hstring decimal4 = decimalFormat4.Format(fixedNumber);

        // Display the results.
        std::wostringstream results;
        results <<
            L"Random number (" << randomNumber << L")" << std::endl <<
            L"With current user preferences: " << std::wstring_view(decimal) << std::endl <<
            L"With grouping separators: " << std::wstring_view(decimal1) << std::endl <<
            L"With grouping separators (fr-FR): " << std::wstring_view(decimalFR) << std::endl <<
            L"With digit substitution (ar): " << std::wstring_view(decimalAR) << std::endl <<
            std::endl <<
            L"Fixed number (" << fixedNumber << L")" << std::endl <<
            L"With current user preferences: " << std::wstring_view(decimal2) << std::endl <<
            L"Always with a decimal point: " << std::wstring_view(decimal3) << std::endl <<
            L"With no fraction digits or decimal points: " << std::wstring_view(decimal4);

        OutputTextBlock().Text(results.str());
    }
}
