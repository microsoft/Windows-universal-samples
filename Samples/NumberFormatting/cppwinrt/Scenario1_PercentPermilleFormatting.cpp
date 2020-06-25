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
#include "Scenario1_PercentPermilleFormatting.h"
#include "Scenario1_PercentPermilleFormatting.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_PercentPermilleFormatting::Scenario1_PercentPermilleFormatting()
    {
        InitializeComponent();
    }

    void Scenario1_PercentPermilleFormatting::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.NumberFormatting.PercentFormatter and
        // the Windows.Globalization.NumberFormatting.PermilleFormatter classes to format numbers
        // as a percent or a permille.

        // Create formatters initialized using the current user's preference settings.
        PercentFormatter percentFormat;
        PermilleFormatter permilleFormat;

        // Make a random number.
        float randomNumber = (float)rand() / RAND_MAX;

        // Format with current user preferences.
        hstring percent = percentFormat.Format(randomNumber);
        hstring permille = permilleFormat.Format(randomNumber);

        // Get a fixed number.
        long long fixedNumber = 500;

        // Format with grouping using default.
        PercentFormatter percentFormat1;
        percentFormat1.IsGrouped(true);
        hstring percent1 = percentFormat1.Format(fixedNumber);

        // Format with grouping using French.
        PercentFormatter percentFormatFR({ L"fr-FR" }, L"ZZ");
        percentFormatFR.IsGrouped(true);
        hstring percentFR1 = percentFormatFR.Format(randomNumber);
        hstring percentFR2 = percentFormatFR.Format(fixedNumber);

        // Format with grouping using Arabic.
        PercentFormatter percentFormatAR({ L"ar" }, L"ZZ");
        percentFormatAR.IsGrouped(true);
        percentFormatAR.FractionDigits(2);
        hstring percentAR = percentFormatAR.Format(fixedNumber);
        PermilleFormatter permilleFormatAR({ L"ar" }, L"ZZ");
        hstring permilleAR = permilleFormatAR.Format(randomNumber);

        // Format with no fractional digits using default.
        PercentFormatter percentFormat2;
        percentFormat2.FractionDigits(0);
        hstring percent2 = percentFormat2.Format(fixedNumber);

        // Format always with a decimal point.
        PercentFormatter percentFormat3;
        percentFormat3.IsDecimalPointAlwaysDisplayed(true);
        percentFormat3.FractionDigits(0);
        hstring percent3 = percentFormat3.Format(fixedNumber);

        // Display the results.
        std::wostringstream results;
        results <<
            L"Random number (" << randomNumber << L")" << std::endl <<
            L"Percent formatted: " << std::wstring_view(percent) << std::endl <<
            L"Permille formatted: " << std::wstring_view(permille) << std::endl <<
            std::endl <<
            L"Language-specific percent formatted: " << std::wstring_view(percentFR1) << std::endl <<
            L"Language-specific permille formatted: " << std::wstring_view(permilleAR) << std::endl <<
            std::endl <<
            L"Fixed number (" << fixedNumber << L")" << std::endl <<
            L"Percent formatted (grouped): " << std::wstring_view(percent1) << std::endl <<
            L"Percent formatted (grouped as fr-FR): " << std::wstring_view(percentFR2) << std::endl <<
            L"Percent formatted (grouped w/digits as ar): " << std::wstring_view(percentAR) << std::endl <<
            L"Percent formatted (no fractional digits): " << std::wstring_view(percent2) << std::endl <<
            L"Percent formatted (always with a decimal point): " << std::wstring_view(percent3);

        OutputTextBlock().Text(results.str());
    }
}
