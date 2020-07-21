//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using System.Text;
using Windows.System.UserProfile;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Globalization;
using Windows.Globalization.NumberFormatting;

namespace NumberFormatting
{
    public sealed partial class Scenario4_NumberParsing : Page
    {
        public Scenario4_NumberParsing()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Display' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Display_Click(object sender, RoutedEventArgs e)
        {
            // This scenario uses the Windows.Globalization.NumberFormatting.DecimalFormatter,
            // Windows.Globalization.NumberFormatting.CurrencyFormatter and 
            // Windows.Globalization.NumberFormatting.PercentFormatter classes to format and parse a number
            // percentage or currency.

            // Keep results of the scenario in a StringBuilder
            StringBuilder results = new StringBuilder();

            // Define percent formatters.
            PercentFormatter percentFormat = new PercentFormatter();
            PercentFormatter percentFormatJP = new PercentFormatter(new string[] { "ja" }, "ZZ");
            PercentFormatter percentFormatFR = new PercentFormatter(new string[] { "fr-FR" }, "ZZ");

            // Define decimal formatters.
            DecimalFormatter decimalFormat = new DecimalFormatter();
            decimalFormat.IsGrouped = true;
            DecimalFormatter decimalFormatJP = new DecimalFormatter(new string[] { "ja" }, "ZZ");
            decimalFormatJP.IsGrouped = true;
            DecimalFormatter decimalFormatFR = new DecimalFormatter(new string[] { "fr-FR" }, "ZZ");
            decimalFormatFR.IsGrouped = true;

            // Define currency formatters
            string userCurrency = GlobalizationPreferences.Currencies[0];
            CurrencyFormatter currencyFormat = new CurrencyFormatter(userCurrency);
            CurrencyFormatter currencyFormatJP = new CurrencyFormatter("JPY", new string[] { "ja" }, "ZZ");
            CurrencyFormatter currencyFormatFR = new CurrencyFormatter("EUR", new string[] { "fr-FR" }, "ZZ");

            // Generate numbers for parsing.
            double percentNumber = 0.523;
            double decimalNumber = 12345.67;
            double currencyNumber = 1234.56;

            // Roundtrip the percent numbers by formatting and parsing.
            String percent1 = percentFormat.Format(percentNumber);
            double percent1Parsed = percentFormat.ParseDouble(percent1).Value;

            String percent1JP = percentFormatJP.Format(percentNumber);
            double percent1JPParsed = percentFormatJP.ParseDouble(percent1JP).Value;

            String percent1FR = percentFormatFR.Format(percentNumber);
            double percent1FRParsed = percentFormatFR.ParseDouble(percent1FR).Value;

            // Generate the results for percent parsing.
            results.AppendLine("Percent parsing of " + percentNumber); 
            results.AppendLine("Formatted for current user: " + percent1   + " Parsed as current user: " + percent1Parsed);
            results.AppendLine("Formatted for ja-JP: "        + percent1JP + " Parsed for ja-JP: "       + percent1JPParsed);
            results.AppendLine("Formatted for fr-FR: "        + percent1FR + " Parsed for fr-FR: "       + percent1FRParsed);
            results.AppendLine();

            // Roundtrip the decimal numbers for formatting and parsing.
            String decimal1 = decimalFormat.Format(decimalNumber);
            double decimal1Parsed = decimalFormat.ParseDouble(decimal1).Value;

            String decimal1JP = decimalFormatJP.Format(decimalNumber);
            double decimal1JPParsed = decimalFormatJP.ParseDouble(decimal1JP).Value;

            String decimal1FR = decimalFormatFR.Format(decimalNumber);
            double decimal1FRParsed = decimalFormatFR.ParseDouble(decimal1FR).Value;

            // Generate the results for decimal parsing.
            results.AppendLine("Decimal parsing of " + decimalNumber);
            results.AppendLine("Formatted for current user: " + decimal1   + " Parsed as current user: " + decimal1Parsed);
            results.AppendLine("Formatted for ja-JP: "        + decimal1JP + " Parsed for ja-JP: "       + decimal1JPParsed);
            results.AppendLine("Formatted for fr-FR: "        + decimal1FR + " Parsed for fr-FR: "       + decimal1FRParsed);
            results.AppendLine();

            // Roundtrip the currency numbers for formatting and parsing.
            String currency1 = currencyFormat.Format(currencyNumber);
            double currency1Parsed = currencyFormat.ParseDouble(currency1).Value;

            String currency1JP = currencyFormatJP.Format(currencyNumber);
            double currency1JPParsed = currencyFormatJP.ParseDouble(currency1JP).Value;

            String currency1FR = currencyFormatFR.Format(currencyNumber);
            double currency1FRParsed = currencyFormatFR.ParseDouble(currency1FR).Value;

            // Generate the results for decimal parsing.
            results.AppendLine("Currency parsing of " + currencyNumber); 
            results.AppendLine("Formatted for current user: " + currency1   + " Parsed as current user: " + currency1Parsed);
            results.AppendLine("Formatted for ja-JP: "        + currency1JP + " Parsed for ja-JP: "       + currency1JPParsed);
            results.AppendLine("Formatted for fr-FR: "        + currency1FR + " Parsed for fr-FR: "       + currency1FRParsed);
            results.AppendLine();

            // Display the results.
            OutputTextBlock.Text = results.ToString();
        }
    }
}
