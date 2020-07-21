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
#include "Scenario3_CurrencyFormatting.h"
#include "Scenario3_CurrencyFormatting.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::NumberFormatting;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario3_CurrencyFormatting::Scenario3_CurrencyFormatting()
    {
        InitializeComponent();
    }

    void Scenario3_CurrencyFormatting::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.NumberFormatting.CurrencyFormatter class
        // to format a number as a currency.

        // Determine the current user's default currency.
        auto userCurrency = GlobalizationPreferences::Currencies().GetAt(0);

        // Generate numbers used for formatting.
        long long wholeNumber = 12345;
        double fractionalNumber = 12345.67;

        // Create formatter initialized using the current user's preference settings for number formatting.
        CurrencyFormatter userCurrencyFormat(userCurrency);
        hstring currencyDefault = userCurrencyFormat.Format(fractionalNumber);

        // Create a formatter initialized to a specific currency, in this case it's the US Dollar, but with the default number formatting for the current user.
        CurrencyFormatter currencyFormatUSD (CurrencyIdentifiers::USD());  // Specified as an ISO 4217 code.
        hstring currencyUSD = currencyFormatUSD.Format(fractionalNumber);

        // Create a formatter initialized to a specific currency, in this case it's the Euro with the default number formatting for France.  
        CurrencyFormatter currencyFormatEuroFR(CurrencyIdentifiers::EUR(), { L"fr-FR" }, L"FR");
        hstring currencyEuroFR = currencyFormatEuroFR.Format(fractionalNumber);

        // Create a formatter initialized to a specific currency, in this case it's the Euro with the default number formatting for Ireland.  
        CurrencyFormatter currencyFormatEuroIE(CurrencyIdentifiers::EUR(), { L"gd-IE" }, L"IE");
        hstring currencyEuroIE = currencyFormatEuroIE.Format(fractionalNumber);

        // Formatted so that fraction digits are always included.
        CurrencyFormatter currencyFormatUSD1(CurrencyIdentifiers::USD());
        currencyFormatUSD1.FractionDigits(2);
        hstring currencyUSD1 = currencyFormatUSD1.Format(wholeNumber);

        // Formatted so that integer grouping separators are included.
        CurrencyFormatter currencyFormatUSD2(CurrencyIdentifiers::USD());
        currencyFormatUSD2.IsGrouped(true);
        hstring currencyUSD2 = currencyFormatUSD2.Format(fractionalNumber);

        // Formatted using currency code instead of currency symbol
        CurrencyFormatter currencyFormatEuroModeSwitch(CurrencyIdentifiers::EUR());
        currencyFormatEuroModeSwitch.Mode(CurrencyFormatterMode::UseCurrencyCode);
        hstring currencyEuroCode = currencyFormatEuroModeSwitch.Format(fractionalNumber);

        // Return back to currency symbol
        currencyFormatEuroModeSwitch.Mode(CurrencyFormatterMode::UseSymbol);
        hstring currencyEuroSymbol = currencyFormatEuroModeSwitch.Format(fractionalNumber);

        // Display the results.
        std::wostringstream results;
        results <<
            L"Fixed number (" << fractionalNumber << L")" << std::endl <<
            L"With user's default currency: " << std::wstring_view(currencyDefault) << std::endl <<
            L"Formatted US Dollar: " << std::wstring_view(currencyUSD) << std::endl <<
            L"Formatted Euro (fr-FR defaults): " << std::wstring_view(currencyEuroFR) << std::endl <<
            L"Formatted Euro (gd-IE defaults): " << std::wstring_view(currencyEuroIE) << std::endl <<
            L"Formatted US Dollar (with fractional digits): " << std::wstring_view(currencyUSD1) << std::endl <<
            L"Formatted US Dollar (with grouping separators): " << std::wstring_view(currencyUSD2) << std::endl <<
            L"Formatted Euro (as currency code): " << std::wstring_view(currencyEuroCode) << std::endl <<
            L"Formatted Euro (as symbol): " << std::wstring_view(currencyEuroSymbol);

        OutputTextBlock().Text(results.str());
    }
}
