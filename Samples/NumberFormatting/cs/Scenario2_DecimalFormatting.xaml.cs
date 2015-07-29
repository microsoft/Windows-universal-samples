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
    public sealed partial class Scenario2_DecimalFormatting : Page
    {
        public Scenario2_DecimalFormatting()
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
            // This scenario uses the Windows.Globalization.NumberFormatting.DecimalFormatter class
            // to format a number.
            
            // Keep results of the scenario in a StringBuilder
            StringBuilder results = new StringBuilder();

            // Create formatter initialized using the current user's preference settings.
            DecimalFormatter decimalFormat = new Windows.Globalization.NumberFormatting.DecimalFormatter();

            // Make a random number.
            double randomNumber = (new Random().NextDouble() * 100000);

            // Format with the user's default preferences.
            String decimalCurrent = decimalFormat.Format(randomNumber);

            // Format with grouping.
            DecimalFormatter decimalFormat1 = new Windows.Globalization.NumberFormatting.DecimalFormatter();
            decimalFormat1.IsGrouped = true;
            String decimal1 = decimalFormat1.Format(randomNumber);

            // Format with grouping using French.
            DecimalFormatter decimalFormatFR = new Windows.Globalization.NumberFormatting.DecimalFormatter(new string[] { "fr-FR" }, "ZZ");
            decimalFormatFR.IsGrouped = true;
            String decimalFR = decimalFormatFR.Format(randomNumber);

            // Illustrate how automatic digit substitution works
            DecimalFormatter decimalFormatAR = new Windows.Globalization.NumberFormatting.DecimalFormatter(new string[] { "ar" }, "ZZ");
            decimalFormatAR.IsGrouped = true;
            String decimalAR = decimalFormatAR.Format(randomNumber);

            // Get a fixed number.
            ulong fixedNumber = 500;

            // Format with the user's default preferences.
            String decimal2 = decimalFormat.Format(fixedNumber);

            // Format always with a decimal point.
            DecimalFormatter decimalFormat3 = new Windows.Globalization.NumberFormatting.DecimalFormatter();
            decimalFormat3.IsDecimalPointAlwaysDisplayed = true;
            decimalFormat3.FractionDigits = 0;
            String decimal3 = decimalFormat3.Format(fixedNumber);

            // Format with no fractional digits or decimal point.
            DecimalFormatter decimalFormat4 = new Windows.Globalization.NumberFormatting.DecimalFormatter();
            decimalFormat4.FractionDigits = 0;
            String decimal4 = decimalFormat4.Format(fixedNumber);

            // Display the results.
            results.AppendLine("Random number (" + randomNumber + ")");
            results.AppendLine("With current user preferences: " + decimalCurrent);
            results.AppendLine("With grouping separators: " + decimal1);
            results.AppendLine("With grouping separators (fr-FR): " + decimalFR);
            results.AppendLine("With digit substitution (ar): " + decimalAR);
            results.AppendLine();
            results.AppendLine("Fixed number (" + fixedNumber + ")");
            results.AppendLine("With current user preferences: " + decimal2);
            results.AppendLine("Always with a decimal point: " + decimal3);
            results.AppendLine("With no fraction digits or decimal points: " + decimal4);

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
