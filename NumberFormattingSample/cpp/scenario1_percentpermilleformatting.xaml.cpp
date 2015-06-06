// Copyright (c) Microsoft. All rights reserved.

#include "pch.h"
#include "Scenario1_PercentPermilleFormatting.xaml.h"

using namespace SDKTemplate;
using namespace NumberFormatting;

using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization::NumberFormatting;

Scenario1_PercentPermilleFormatting::Scenario1_PercentPermilleFormatting()
{
    InitializeComponent();
}

void Scenario1_PercentPermilleFormatting::Display_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    // This scenario uses the Windows.Globalization.NumberFormatting.PercentFormatter and
    // the Windows.Globalization.NumberFormatting.PermilleFormatter classes to format numbers
    // as a percent or a permille.

    // Create formatters initialized using the current user's preference settings.
    PercentFormatter^ percentFormat = ref new PercentFormatter();
    PermilleFormatter^ permilleFormat = ref new PermilleFormatter();

    // Make a random number.
    float randomNumber = (float)rand() / RAND_MAX;

    // Format with current user preferences.
    String^ percent = percentFormat->Format(randomNumber);
    String^ permille = permilleFormat->Format(randomNumber);

    // Get a fixed number.
    long long fixedNumber = 500;

    // Format with grouping using default.
    PercentFormatter^ percentFormat1 = ref new PercentFormatter();
    percentFormat1->IsGrouped = true;
    String^ percent1 = percentFormat1->Format(fixedNumber);

    // Format with grouping using French.
    auto fr = ref new Platform::Collections::Vector<String^>();
    fr->Append("fr-FR");
    PercentFormatter^ percentFormatFR = ref new PercentFormatter(fr, "ZZ");
    percentFormatFR->IsGrouped = true;
    String^ percentFR1 = percentFormatFR->Format(randomNumber);
    String^ percentFR2 = percentFormatFR->Format(fixedNumber);

    // Format with grouping using Arabic.
    auto ar = ref new Platform::Collections::Vector<String^>();
    ar->Append("ar");
    PercentFormatter^ percentFormatAR = ref new PercentFormatter(ar, "ZZ");
    percentFormatAR->IsGrouped = true;
    percentFormatAR->FractionDigits = 2;
    String^ percentAR = percentFormatAR->Format(fixedNumber);
    PermilleFormatter^ permilleFormatAR = ref new PermilleFormatter(ar, "ZZ");
    String^ permilleAR = permilleFormatAR->Format(randomNumber);

    // Format with no fractional digits using default.
    PercentFormatter^ percentFormat2 = ref new PercentFormatter();
    percentFormat2->FractionDigits = 0;
    String^ percent2 = percentFormat2->Format(fixedNumber);

    // Format always with a decimal point.
    PercentFormatter^ percentFormat3 = ref new PercentFormatter();
    percentFormat3->IsDecimalPointAlwaysDisplayed = true;
    percentFormat3->FractionDigits = 0;
    String^ percent3 = percentFormat3->Format(fixedNumber);

    // Display the results.
    String^ results = 
        "Random number ("                                   + randomNumber + ")\n" +
        "Percent formatted: "                               + percent  + "\n" +
        "Permille formatted: "                              + permille + "\n\n" +
        "Language-specific percent formatted: "             + percentFR1 + "\n" +
        "Language-specific permille formatted: "            + permilleAR + "\n\n" +
        "Fixed number ("                                    + fixedNumber + ")\n" +    
        "Percent formatted (grouped): "                     + percent1 + "\n" +
        "Percent formatted (grouped as fr-FR): "            + percentFR2 + "\n" +
        "Percent formatted (grouped w/digits as ar): "      + percentAR + "\n" +
        "Percent formatted (no fractional digits): "        + percent2  + "\n" +
        "Percent formatted (always with a decimal point): " + percent3;

    OutputTextBlock->Text = results;
}
