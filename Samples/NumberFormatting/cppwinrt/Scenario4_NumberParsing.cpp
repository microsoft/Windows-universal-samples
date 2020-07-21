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
#include "Scenario4_NumberParsing.h"
#include "Scenario4_NumberParsing.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_NumberParsing::Scenario4_NumberParsing()
    {
        InitializeComponent();
    }

    void Scenario4_NumberParsing::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.NumberFormatting.DecimalFormatter,
        // Windows.Globalization.NumberFormatting.CurrencyFormatter and 
        // Windows.Globalization.NumberFormatting.PercentFormatter classes to format and parse a number
        // percentage or currency.

        // Define percent formatters.
        PercentFormatter percentFormat;
        PercentFormatter percentFormatJP({ L"ja-JP" }, L"JP");
        PercentFormatter percentFormatFR({ L"fr-FR" }, L"FR");

        // Define decimal formatters.
        DecimalFormatter decimalFormat = DecimalFormatter();
        decimalFormat.IsGrouped(true);
        DecimalFormatter decimalFormatJP({ L"ja-JP" }, L"JP");
        decimalFormatJP.IsGrouped(true);
        DecimalFormatter decimalFormatFR({ L"fr-FR" }, L"FR");
        decimalFormatFR.IsGrouped(true);

        // Define currency formatters
        auto userCurrency = GlobalizationPreferences::Currencies().GetAt(0);
        CurrencyFormatter currencyFormat(userCurrency);
        CurrencyFormatter currencyFormatJP(L"JPY", { L"ja-JP" }, L"JP");
        CurrencyFormatter currencyFormatFR(L"EUR", { L"fr-FR" }, L"FR");

        // Generate numbers for parsing.
        double percentNumber = 0.523;
        double decimalNumber = 12345.67;
        double currencyNumber = 1234.56;

        // Roundtrip the percent numbers by formatting and parsing.
        hstring percent1 = percentFormat.Format(percentNumber);
        double percent1Parsed = percentFormat.ParseDouble(percent1).Value();

        hstring percent1JP = percentFormatJP.Format(percentNumber);
        double percent1JPParsed = percentFormatJP.ParseDouble(percent1JP).Value();

        hstring percent1FR = percentFormatFR.Format(percentNumber);
        double percent1FRParsed = percentFormatFR.ParseDouble(percent1FR).Value();

        // Generate the results for percent parsing.
        std::wostringstream results;
        results << L"Percent parsing of " << percentNumber << std::endl <<
            L"Formatted for current user: " << std::wstring_view(percent1) << L" Parsed as current user: " << percent1Parsed << std::endl <<
            L"Formatted for ja-JP: " << std::wstring_view(percent1JP) << L" Parsed for ja-JP: " << percent1JPParsed << std::endl <<
            L"Formatted for fr-FR: " << std::wstring_view(percent1FR) << L" Parsed for fr-FR: " << percent1FRParsed << std::endl <<
            std::endl;

        // Roundtrip the decimal numbers for formatting and parsing.
        hstring decimal1 = decimalFormat.Format(decimalNumber);
        double decimal1Parsed = decimalFormat.ParseDouble(decimal1).Value();

        hstring decimal1JP = decimalFormatJP.Format(decimalNumber);
        double decimal1JPParsed = decimalFormatJP.ParseDouble(decimal1JP).Value();

        hstring decimal1FR = decimalFormatFR.Format(decimalNumber);
        double decimal1FRParsed = decimalFormatFR.ParseDouble(decimal1FR).Value();

        // Generate the results for decimal parsing.
        results <<  L"Decimal parsing of " << decimalNumber << std::endl <<
            L"Formatted for current user: " << std::wstring_view(decimal1) << L" Parsed as current user: " << decimal1Parsed << std::endl <<
            L"Formatted for ja-JP: " << std::wstring_view(decimal1JP) << L" Parsed for ja-JP: " << decimal1JPParsed << std::endl <<
            L"Formatted for fr-FR: " << std::wstring_view(decimal1FR) << L" Parsed for fr-FR: " << decimal1FRParsed << std::endl <<
            std::endl;

        // Roundtrip the currency numbers for formatting and parsing.
        hstring currency1 = currencyFormat.Format(currencyNumber);
        double currency1Parsed = currencyFormat.ParseDouble(currency1).Value();

        hstring currency1JP = currencyFormatJP.Format(currencyNumber);
        double currency1JPParsed = currencyFormatJP.ParseDouble(currency1JP).Value();

        hstring currency1FR = currencyFormatFR.Format(currencyNumber);
        double currency1FRParsed = currencyFormatFR.ParseDouble(currency1FR).Value();

        // Generate the results for decimal parsing.
        results << L"Currency parsing of " << currencyNumber << L"\n" <<
            L"Formatted for current user: " << std::wstring_view(currency1) << L" Parsed as current user: " << currency1Parsed << std::endl <<
            L"Formatted for ja-JP: " << std::wstring_view(currency1JP) << L" Parsed for ja-JP: " << currency1JPParsed << std::endl <<
            L"Formatted for fr-FR: " << std::wstring_view(currency1FR) << L" Parsed for fr-FR: " << currency1FRParsed << std::endl <<
            std::endl;

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
