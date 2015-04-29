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
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Globalization;
using Windows.Globalization.NumberFormatting;

namespace NumberFormatting
{
    public sealed partial class Scenario1_PercentPermilleFormatting : Page
    {
        public Scenario1_PercentPermilleFormatting()
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
            // This scenario uses the Windows.Globalization.NumberFormatting.PercentFormatter and
            // the Windows.Globalization.NumberFormatting.PermilleFormatter classes to format numbers
            // as a percent or a permille.

            // Keep results of the scenario in a StringBuilder
            StringBuilder results = new StringBuilder();

            // Create numbers to format.
            double randomNumber = new Random().NextDouble();
            ulong fixedNumber = 500;

            // Create percent formatters.
            PercentFormatter defaultPercentFormatter = new PercentFormatter();
            PercentFormatter frPercentFormatter = new PercentFormatter(new[] { "fr-FR" }, "ZZ");
            PercentFormatter arPercentFormatter = new PercentFormatter(new[] { "ar" }, "ZZ");

            // Create permille formatters.
            PermilleFormatter defaultPermilleFormatter = new PermilleFormatter();
            PermilleFormatter arPermilleFormatter = new PermilleFormatter(new[] { "ar" }, "ZZ");

            // Format random numbers as percent or permille.
            results.AppendLine("Random number (" + randomNumber + ")");
            results.AppendLine("Percent formatted: " + defaultPercentFormatter.Format(randomNumber));
            results.AppendLine("Permille formatted: " + defaultPermilleFormatter.Format(randomNumber));
            results.AppendLine();
            results.AppendLine("Language-specific percent formatted: " + frPercentFormatter.Format(randomNumber));
            results.AppendLine("Language-specific permille formatted: " + arPermilleFormatter.Format(randomNumber));
            results.AppendLine();
            results.AppendLine("Fixed number (" + fixedNumber + ")");

            // Format fixed number with grouping.
            defaultPercentFormatter.IsGrouped = true;
            results.AppendLine("Percent formatted (grouped): " + defaultPercentFormatter.Format(fixedNumber));

            //Format with grouping using French language.
            frPercentFormatter.IsGrouped = true;
            results.AppendLine("Percent formatted (grouped as fr-FR): " + frPercentFormatter.Format(fixedNumber));

            // Format with grouping using Arabic.
            results.AppendLine("Percent formatted (grouped w/digits as ar): " + arPercentFormatter.Format(fixedNumber));

            // Format with no fraction digits.
            defaultPercentFormatter.FractionDigits = 0;
            defaultPercentFormatter.IsGrouped = false;
            results.AppendLine("Percent formatted (no fractional digits): " + defaultPercentFormatter.Format(fixedNumber));

            // Format always with a decimal point.
            defaultPercentFormatter.IsDecimalPointAlwaysDisplayed = true;
            defaultPercentFormatter.IsGrouped = false;
            results.AppendLine("Percent formatted (always with a decimal point): " + defaultPercentFormatter.Format(fixedNumber));

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
