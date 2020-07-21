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
    public sealed partial class Scenario3_CurrencyFormatting : Page
    {
        public Scenario3_CurrencyFormatting()
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
            // This scenario uses the Windows.Globalization.NumberFormatting.CurrencyFormatter class
            // to format a number as a currency.

            // Keep results of the scenario in a StringBuilder
            StringBuilder results = new StringBuilder();

            // Determine the current user's default currency.
            string currency = GlobalizationPreferences.Currencies[0];

            // Generate numbers used for formatting.
            ulong wholeNumber = 12345;
            double fractionalNumber = 12345.67;

            // Create currency formatter initialized with current number formatting preferences.
            CurrencyFormatter defaultCurrencyFormatter = new CurrencyFormatter(currency);

            CurrencyFormatter usdCurrencyFormatter = new CurrencyFormatter(CurrencyIdentifiers.USD);
            CurrencyFormatter eurFRCurrencyFormatter = new CurrencyFormatter(CurrencyIdentifiers.EUR, new[] { "fr-FR" }, "FR");
            CurrencyFormatter eurIECurrencyFormatter = new CurrencyFormatter(CurrencyIdentifiers.EUR, new[] { "gd-IE" }, "IE");
            CurrencyFormatter currencyFormatEuroModeSwitch = new CurrencyFormatter(CurrencyIdentifiers.EUR);

            // Format numbers as currency.
            results.AppendLine("Fixed number (" + fractionalNumber + ")");
            results.AppendLine("With user's default currency: " + defaultCurrencyFormatter.Format(fractionalNumber));
            results.AppendLine("Formatted US Dollar: " + usdCurrencyFormatter.Format(fractionalNumber));
            results.AppendLine("Formatted Euro (fr-FR defaults): " + eurFRCurrencyFormatter.Format(fractionalNumber));
            results.AppendLine("Formatted Euro (gd-IE defaults): " + eurIECurrencyFormatter.Format(fractionalNumber));

            // Format currency with fraction digits always included.
            usdCurrencyFormatter.FractionDigits = 2;
            results.AppendLine("Formatted US Dollar (with fractional digits): " + usdCurrencyFormatter.Format(wholeNumber));

            // Format currenccy with grouping.
            usdCurrencyFormatter.IsGrouped = true;
            results.AppendLine("Formatted US Dollar (with grouping separators): " + usdCurrencyFormatter.Format(fractionalNumber));

            // Format using currency code instead of currency symbol
            currencyFormatEuroModeSwitch.Mode = CurrencyFormatterMode.UseCurrencyCode;
            results.AppendLine("Formatted Euro (as currency code): " + currencyFormatEuroModeSwitch.Format(fractionalNumber));

            // Switch so we can now format using currency symbol
            currencyFormatEuroModeSwitch.Mode = CurrencyFormatterMode.UseSymbol;
            results.AppendLine("Formatted Euro (as symbol): " + currencyFormatEuroModeSwitch.Format(fractionalNumber));
            
            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
