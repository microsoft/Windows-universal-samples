// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario3_CurrencyFormatting.xaml.h"

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

Scenario3_CurrencyFormatting::Scenario3_CurrencyFormatting()
{
    InitializeComponent();
}

void Scenario3_CurrencyFormatting::Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // This scenario uses the Windows.Globalization.NumberFormatting.CurrencyFormatter class
    // to format a number as a currency.

    // Determine the current user's default currency.
    auto userCurrency = Windows::System::UserProfile::GlobalizationPreferences::Currencies->First()->Current;

    // Generate numbers used for formatting.
    long long wholeNumber      = 12345;
    double fractionalNumber = 12345.67;

    // Create formatter initialized using the current user's preference settings for number formatting.
    CurrencyFormatter^ userCurrencyFormat = ref new CurrencyFormatter(userCurrency);
    String^ currencyDefault = userCurrencyFormat->Format(fractionalNumber);

    // Create a formatter initialized to a specific currency, in this case it's the US Dollar, but with the default number formatting for the current user.
    CurrencyFormatter^ currencyFormatUSD = ref new CurrencyFormatter(CurrencyIdentifiers::USD);  // Specified as an ISO 4217 code.
    String^ currencyUSD = currencyFormatUSD->Format(fractionalNumber);

    // Create a formatter initialized to a specific currency, in this case it's the Euro with the default number formatting for France.  
    auto fr = ref new Platform::Collections::Vector<String^>();
    fr->Append("fr-FR");
    CurrencyFormatter^ currencyFormatEuroFR = ref new CurrencyFormatter(CurrencyIdentifiers::EUR, fr, "FR");
    String^ currencyEuroFR = currencyFormatEuroFR->Format(fractionalNumber);

    // Create a formatter initialized to a specific currency, in this case it's the Euro with the default number formatting for Ireland.  
    auto gd = ref new Platform::Collections::Vector<String^>();
    gd->Append("gd-IE");
    CurrencyFormatter^ currencyFormatEuroIE = ref new CurrencyFormatter(CurrencyIdentifiers::EUR, gd, "IE");
    String^ currencyEuroIE = currencyFormatEuroIE->Format(fractionalNumber);

    // Formatted so that fraction digits are always included.
    CurrencyFormatter^ currencyFormatUSD1 = ref new CurrencyFormatter(CurrencyIdentifiers::USD);
    currencyFormatUSD1->FractionDigits = 2;
    String^ currencyUSD1 = currencyFormatUSD1->Format(wholeNumber);

    // Formatted so that integer grouping separators are included.
    CurrencyFormatter^ currencyFormatUSD2 = ref new CurrencyFormatter(CurrencyIdentifiers::USD);
    currencyFormatUSD2->IsGrouped = 1;
    String^ currencyUSD2 = currencyFormatUSD2->Format(fractionalNumber);

    // Formatted using currency code instead of currency symbol
    CurrencyFormatter^ currencyFormatEuroModeSwitch = ref new CurrencyFormatter(CurrencyIdentifiers::EUR);
    currencyFormatEuroModeSwitch->Mode = CurrencyFormatterMode::UseCurrencyCode;
    String^ currencyEuroCode = currencyFormatEuroModeSwitch->Format(fractionalNumber);

    // Return back to currency symbol
    currencyFormatEuroModeSwitch->Mode = CurrencyFormatterMode::UseSymbol;
    String^ currencyEuroSymbol = currencyFormatEuroModeSwitch->Format(fractionalNumber);

    // Display the results.
    String^ results = 
        "Fixed number ("                                   + fractionalNumber + ")\n" +
        "With user's default currency: "                   + currencyDefault  + "\n" +
        "Formatted US Dollar: "                            + currencyUSD      + "\n" +
        "Formatted Euro (fr-FR defaults): "                + currencyEuroFR   + "\n" +
        "Formatted Euro (gd-IE defaults): "                + currencyEuroIE   + "\n" +
        "Formatted US Dollar (with fractional digits): "   + currencyUSD1     + "\n" +
        "Formatted US Dollar (with grouping separators): " + currencyUSD2     + "\n" +
        "Formatted Euro (as currency code): "              + currencyEuroCode + "\n" +
        "Formatted Euro (as symbol): "                     + currencyEuroSymbol;

    OutputTextBlock->Text = results;
}
