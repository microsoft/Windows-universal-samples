// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario4_NumberParsing.xaml.h"

using namespace SDKTemplate;
using namespace NumberFormatting;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::NumberFormatting;

Scenario4_NumberParsing::Scenario4_NumberParsing()
{
    InitializeComponent();
}

void Scenario4_NumberParsing::Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // This scenario uses the Windows.Globalization.NumberFormatting.DecimalFormatter,
    // Windows.Globalization.NumberFormatting.CurrencyFormatter and 
    // Windows.Globalization.NumberFormatting.PercentFormatter classes to format and parse a number
    // percentage or currency.

    // Define percent formatters.
    PercentFormatter^ percentFormat = ref new PercentFormatter();

    auto ja = ref new Platform::Collections::Vector<String^>();
    ja->Append("ja-JP");
    PercentFormatter^ percentFormatJP = ref new PercentFormatter(ja, "JP");

    auto fr = ref new Platform::Collections::Vector<String^>();
    fr->Append("fr-FR");
    PercentFormatter^ percentFormatFR = ref new PercentFormatter(fr, "FR");

    // Define decimal formatters.
    DecimalFormatter^ decimalFormat = ref new DecimalFormatter();
    decimalFormat->IsGrouped = true;
    DecimalFormatter^ decimalFormatJP = ref new DecimalFormatter(ja, "JP");
    decimalFormatJP->IsGrouped = true;
    DecimalFormatter^ decimalFormatFR = ref new DecimalFormatter(fr, "FR");
    decimalFormatFR->IsGrouped = true;

    // Define currency formatters
    auto userCurrency = Windows::System::UserProfile::GlobalizationPreferences::Currencies->First()->Current;
    CurrencyFormatter^ currencyFormat = ref new CurrencyFormatter(userCurrency);
    CurrencyFormatter^ currencyFormatJP = ref new CurrencyFormatter("JPY", ja, "JP");
    CurrencyFormatter^ currencyFormatFR = ref new CurrencyFormatter("EUR", fr, "FR");

    // Generate numbers for parsing.
    double percentNumber = 0.523;
    double decimalNumber = 12345.67;
    double currencyNumber = 1234.56;

    // Roundtrip the percent numbers by formatting and parsing.
    String^ percent1 = percentFormat->Format(percentNumber);
    double percent1Parsed = percentFormat->ParseDouble(percent1)->Value;

    String^ percent1JP = percentFormatJP->Format(percentNumber);
    double percent1JPParsed = percentFormatJP->ParseDouble(percent1JP)->Value;

    String^ percent1FR = percentFormatFR->Format(percentNumber);
    double percent1FRParsed = percentFormatFR->ParseDouble(percent1FR)->Value;

    // Generate the results for percent parsing.
    String^ results1 = "Percent parsing of " + percentNumber + "\n" + 
    "Formatted for current user: " + percent1   + " Parsed as current user: " + percent1Parsed   + "\n" +
    "Formatted for ja-JP: "        + percent1JP + " Parsed for ja-JP: "       + percent1JPParsed + "\n" + 
    "Formatted for fr-FR: "        + percent1FR + " Parsed for fr-FR: "       + percent1FRParsed;

    // Roundtrip the decimal numbers for formatting and parsing.
    String^ decimal1 = decimalFormat->Format(decimalNumber);
    double decimal1Parsed = decimalFormat->ParseDouble(decimal1)->Value;

    String^ decimal1JP = decimalFormatJP->Format(decimalNumber);
    double decimal1JPParsed = decimalFormatJP->ParseDouble(decimal1JP)->Value;

    String^ decimal1FR = decimalFormatFR->Format(decimalNumber);
    double decimal1FRParsed = decimalFormatFR->ParseDouble(decimal1FR)->Value;

    // Generate the results for decimal parsing.
    String^ results2 = "Decimal parsing of " + decimalNumber + "\n" +
    "Formatted for current user: " + decimal1   + " Parsed as current user: " + decimal1Parsed   + "\n" +
    "Formatted for ja-JP: "        + decimal1JP + " Parsed for ja-JP: "       + decimal1JPParsed + "\n" +
    "Formatted for fr-FR: "        + decimal1FR + " Parsed for fr-FR: "       + decimal1FRParsed;

    // Roundtrip the currency numbers for formatting and parsing.
    String^ currency1 = currencyFormat->Format(currencyNumber);
    double currency1Parsed = currencyFormat->ParseDouble(currency1)->Value;

    String^ currency1JP = currencyFormatJP->Format(currencyNumber);
    double currency1JPParsed = currencyFormatJP->ParseDouble(currency1JP)->Value;

    String^ currency1FR = currencyFormatFR->Format(currencyNumber);
    double currency1FRParsed = currencyFormatFR->ParseDouble(currency1FR)->Value;

    // Generate the results for decimal parsing.
    String^ results3 = "Currency parsing of " + currencyNumber + "\n" + 
    "Formatted for current user: " + currency1   + " Parsed as current user: " + currency1Parsed   + "\n" +
    "Formatted for ja-JP: "        + currency1JP + " Parsed for ja-JP: "       + currency1JPParsed + "\n" +
    "Formatted for fr-FR: "        + currency1FR + " Parsed for fr-FR: "       + currency1FRParsed;

    // Display the results.
    String^ results = results1 + "\n\n" + results2 + "\n\n" + results3;
    OutputTextBlock->Text = results;
}
