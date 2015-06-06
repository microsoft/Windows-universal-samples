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
    public sealed partial class Scenario5_RoundingAndPadding : Page
    {
        public Scenario5_RoundingAndPadding()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Used to display the rounding algorithm in a string format.  Used by the different scenarios when 
        /// displaying results for the scenario
        /// </summary>
        /// <param name="roundingAlgorithm"></param>
        private string DisplayRoundingAlgorithmAsString(Windows.Globalization.NumberFormatting.RoundingAlgorithm roundingAlgorithm) 
        {
            String roundingAsString = "";

            // Convert the enumeration value into a string representation
            switch (roundingAlgorithm)
            {
                case RoundingAlgorithm.None:
                    roundingAsString = "no";
                    break;
                case RoundingAlgorithm.RoundAwayFromZero:
                    roundingAsString = "away from zero";
                    break;
                case RoundingAlgorithm.RoundDown:
                    roundingAsString = "down";
                    break;
                case RoundingAlgorithm.RoundHalfAwayFromZero:
                    roundingAsString = "half away from zero";
                    break;
                case RoundingAlgorithm.RoundHalfDown:
                    roundingAsString = "half down";
                    break;
                case RoundingAlgorithm.RoundHalfToEven:
                    roundingAsString = "half to even";
                    break;
                case RoundingAlgorithm.RoundHalfToOdd:
                    roundingAsString = "half to odd";
                    break;
                case RoundingAlgorithm.RoundHalfTowardsZero:
                    roundingAsString = "half towards zero";
                    break;
                case RoundingAlgorithm.RoundHalfUp:
                    roundingAsString = "half up";
                    break;
                case RoundingAlgorithm.RoundTowardsZero:
                    roundingAsString = "toward zero";
                    break;
                case RoundingAlgorithm.RoundUp:
                    roundingAsString = "up";
                    break;
                default:
                    roundingAsString = "unknown";
                    break;
            }
            return roundingAsString;
        }

        /// <summary>
        /// Executes the padding scenarios.
        /// </summary>
        /// <param name="formatter">
        /// The formatter parameter can be either a DecimalFormatter, PercentFormatter
        /// PerMilleFormatter or CurrencyFormatter
        /// </param>
        private string DoPaddingScenarios(INumberFormatter formatter) 
        {
            // Display the properties of the scenario
            StringBuilder results = new StringBuilder();
            results.Append("Padding with ");
            results.Append(((ISignificantDigitsOption) formatter).SignificantDigits + " significant digits, ");
            results.Append(((INumberFormatterOptions) formatter).IntegerDigits + " integer digits, ");
            results.Append(((INumberFormatterOptions) formatter).FractionDigits + " fractional digits\n\n");

            // Iterate through the numbers to pad, format them and add them to the results
            double[] numbersToPad = new double[] { 0.12, 1.2, 10.2, 102 };
            foreach (double numberToPad in numbersToPad) 
            {
                string paddedNumber = formatter.Format(numberToPad);
                results.Append("Value: " + numberToPad + " Padded: " + paddedNumber + "\n");
            }

            // Add a carriage return at the end of the scenario for readability
            results.AppendLine();
            return results.ToString();
        }

        /// <summary>
        /// Demonstrates how to perform padding and rounding by using the DecimalFormatter class (can be any of the 
        /// formatter classes in the Windows.Globalization.Numberformatting namespace) and the IncrementNumberRounder
        /// to do so.  The SignificantDigitsNumberRounder can also be used instead of the latter.
        /// </summary>
        /// <param name="roundingAlgorithm"></param>
        /// <param name="significantDigits"></param>
        /// <param name="integerDigits"></param>
        /// <param name="fractionalDigits"></param>
        string DoPaddingAndRoundingScenarios(RoundingAlgorithm roundingAlgorithm, int significantDigits, int integerDigits, int fractionalDigits) 
        {
            // Create the rounder and set the rounding algorithm provided as a parameter
            IncrementNumberRounder rounder = new Windows.Globalization.NumberFormatting.IncrementNumberRounder();
            rounder.RoundingAlgorithm = roundingAlgorithm;

            // Create the formatter, set the padding properties provided as parameters and associate the rounder
            // we have just created
            DecimalFormatter formatter = new Windows.Globalization.NumberFormatting.DecimalFormatter();
            formatter.SignificantDigits = significantDigits;
            formatter.IntegerDigits = integerDigits;
            formatter.FractionDigits = fractionalDigits;
            formatter.NumberRounder = rounder;

            // Stores the results
            StringBuilder results = new StringBuilder();

            // Iterate through the increments we have defined in the scenario
            double[] incrementsToRound = new double[] { 0.001, 0.01, 0.1, 1.0 };
            foreach (double incrementToRound in incrementsToRound) 
            {
                // Set the increment to round to
                rounder.Increment = incrementToRound;

                // Display the properties of the scenario
                results.Append("Padding and rounding with ");
                results.Append(formatter.SignificantDigits + " significant digits, ");
                results.Append(formatter.IntegerDigits + " integer digits, ");
                results.Append(formatter.FractionDigits + " fractional digits, ");
                results.Append(rounder.Increment + " increment and ");
                results.Append(DisplayRoundingAlgorithmAsString(rounder.RoundingAlgorithm) + " rounding algorithm\n\n");

                // Iterate through the numbers to round and pad, format them and add them to the results
                double[] numbersToFormat = new double[]  {0.1458, 1.458, 14.58, 145.8 };
                foreach (double numberToFormat in numbersToFormat) 
                {
                    string formattedNumber = formatter.Format(numberToFormat);
                    results.Append("Value: " + numberToFormat + " Formatted: " + formattedNumber + "\n");
                }

                // Add a carriage return at the end of the scenario for readability
                results.AppendLine();
            }

            return results.ToString();
        }

        /// <summary>
        /// This scenario illustrates how to provide a rounding algorithm to a CurrencyFormatter class by means of the 
        /// applyRoundingForCurrency method.  This will associate an IncrementNumberRounder using the same increment as the
        /// fractionDigits appropriate for the currency and language.  You can always choose to provide a different rounding 
        /// algorithm by setting the numberRounder property, as depicted in the doPaddingAndRoundingScenarios function.  
        /// The mechanism provided here is better suited for currencies.
        /// </summary>
        /// <param name="currencyCode"></param>
        /// <param name="roundingAlgorithm"></param>
        private string DoCurrencyRoundingScenarios(string currencyCode, RoundingAlgorithm roundingAlgorithm) 
        {
            // Create the currency formatter and set the rounding algorithm provided as a parameter
            CurrencyFormatter currencyFormatter = new Windows.Globalization.NumberFormatting.CurrencyFormatter(currencyCode);
            currencyFormatter.ApplyRoundingForCurrency(roundingAlgorithm);

            // Display the properties of the scenario
            StringBuilder results = new StringBuilder();
            results.Append("Currency formatting for ");
            results.Append(currencyFormatter.Currency + " currency and using the ");
            results.Append(DisplayRoundingAlgorithmAsString(roundingAlgorithm) + " rounding algorithm\n\n");

            // Iterate through the numbers to round and add them to the results
            double[] numbersToFormat = new double[] { 14.55, 3.345, 16.2, 175.8 };
            foreach (double numberToFormat in numbersToFormat)
            {
                string formattedNumber = currencyFormatter.Format(numberToFormat);
                results.Append("Value: " + numberToFormat + " Formatted: " + formattedNumber + "\n");
            }

            // Add a carriage return at the end of the scenario for readability
            results.AppendLine();
            return results.ToString();
        }

        /// <summary>
        /// Shows how to do number rounding using the IncrementNumberRounding class.
        ///</summary>
        ///<param name="roundingAlgorithm"></param>
        private string DoIncrementRoundingScenarios(RoundingAlgorithm roundingAlgorithm) 
        {
            // Create the rounder and set the rounding algorithm provided as a parameter
            IncrementNumberRounder rounder = new Windows.Globalization.NumberFormatting.IncrementNumberRounder();
            rounder.RoundingAlgorithm = roundingAlgorithm;

            // Formatter to display the rounded value.  It is recommended to use the increment number
            // rounder within a formatter object to avoid precision issues when displaying.
            DecimalFormatter formatter = new Windows.Globalization.NumberFormatting.DecimalFormatter();
            formatter.NumberRounder = rounder;

            // Stores the results
            StringBuilder results = new StringBuilder();

            // Iterate through the increments we have defined in the scenario
            double[] incrementsToRound = new double[] { 0.001, 0.01, 0.1, 1.0 };
            foreach (double incrementToRound in incrementsToRound)
            {
                // Set the significant digits to round to
                rounder.Increment = incrementToRound;

                // Display the properties of the scenario
                results.Append("Rounding with ");
                results.Append(rounder.Increment + " increment and ");
                results.Append(DisplayRoundingAlgorithmAsString(rounder.RoundingAlgorithm) + " rounding algorithm\n\n");

                // Iterate through the numbers to round and add them to the results
                double[] numbersToRound = new double[] { 0.1458, 1.458, 14.58, 145.8 };
                foreach (double numberToRound in numbersToRound) 
                {
                    results.Append("Value: " + numberToRound + " Rounded: " + formatter.Format(numberToRound) + "\n");
                }

                // Add a carriage return at the end of the scenario for readability
                results.AppendLine();
            }

            return results.ToString();
        }

        /// <summary>
        /// Shows how to do number rounding using the SignificantDigitsNumberRounder class.
        /// </summary>
        /// <param name="roundingAlgorithm"></param>
        private string DoSignificantDigitRoundingScenarios(RoundingAlgorithm roundingAlgorithm) 
        {
            // Create the rounder and set the rounding algorithm provided as a parameter.
            SignificantDigitsNumberRounder rounder = new Windows.Globalization.NumberFormatting.SignificantDigitsNumberRounder();
            rounder.RoundingAlgorithm = roundingAlgorithm;

            // Stores the results
            StringBuilder results = new StringBuilder();

            // Iterate through the significant digits we have defined in the scenario
            uint[] digitsToRound = new uint[] { 3, 2, 1 };
            foreach (uint digitToRound in digitsToRound)
            {
                // Set the significant digits to round to
                rounder.SignificantDigits = digitToRound;

                // Display the properties of the scenario
                results.Append("Rounding with ");
                results.Append(rounder.SignificantDigits + " significant digits and ");
                results.Append(DisplayRoundingAlgorithmAsString(rounder.RoundingAlgorithm) + " rounding algorithm\n\n");

                // Iterate through the numbers to round and add them to the results
                double[] numbersToRound = new double[] { 0.1458, 1.458, 14.58, 145.8 };
                foreach (double numberToRound in numbersToRound) 
                {
                    double roundedValue = rounder.RoundDouble(numberToRound);
                    results.Append("Value: " + numberToRound + " Rounded: " + roundedValue + "\n");
                }

                // Add a carriage return at the end of the scenario for readability
                results.AppendLine();
            }

            return results.ToString();
        }

        /// <summary>
        /// This is the click handler for the 'Display' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Display_Click(object sender, RoutedEventArgs e)
        {
            // Variable where we keep results to display
            StringBuilder results = new StringBuilder();

            // Create a decimal formatter used to do padding with only 4 significant digits.  We zero the
            // integerDigits and fractionDigits to only show the behavior of the significantDigits property
            // when formatting.
            DecimalFormatter decimalFormatter = new Windows.Globalization.NumberFormatting.DecimalFormatter();
            decimalFormatter.SignificantDigits = 4;
            decimalFormatter.IntegerDigits = 0;
            decimalFormatter.FractionDigits = 0;

            // Run through the scenarios with padding
            results.Append(DoPaddingScenarios(decimalFormatter));

            // Create a percent formatter used to do padding with 4 significant digits, 2 minimum integer 
            // digits, 2 minimum factional digits.  This will show how the behavior of the significantDigits
            // property in conjunction with the integerDigits and fractionDigits properties.
            PercentFormatter percentFormatter = new Windows.Globalization.NumberFormatting.PercentFormatter();
            percentFormatter.SignificantDigits = 4;
            percentFormatter.IntegerDigits = 2;
            percentFormatter.FractionDigits = 2;

            // Run through the scenarios
            results.Append(DoPaddingScenarios(percentFormatter));

            // Do the significant rounding scenarios with round up algorithm 
            results.Append(DoSignificantDigitRoundingScenarios(RoundingAlgorithm.RoundUp));

            // Do the significant rounding scenarios with round down algorithm
            results.Append(DoSignificantDigitRoundingScenarios(RoundingAlgorithm.RoundDown));

            // Do the increment rounding scenarios with round half up algorithm
            results.Append(DoIncrementRoundingScenarios(RoundingAlgorithm.RoundHalfUp));

            // Do the increment rounding scenarios with round half down algorithm
            results.Append(DoIncrementRoundingScenarios(RoundingAlgorithm.RoundHalfDown));
  
            // Do currency formatting with rounding for Japanese Yen with round half to odd algorithm
            results.Append(DoCurrencyRoundingScenarios("JPY", RoundingAlgorithm.RoundHalfToOdd));

            // Do currency formatting with rounding for Euro with round half to even algorithm
            results.Append(DoCurrencyRoundingScenarios("EUR", RoundingAlgorithm.RoundHalfToEven));

            // Do padding and rounding scenarios using round half up algorithm
            results.Append(DoPaddingAndRoundingScenarios(RoundingAlgorithm.RoundHalfUp, 4, 3, 2));

            // Do padding and rounding scenarios using round half down algorithm
            results.Append(DoPaddingAndRoundingScenarios(RoundingAlgorithm.RoundHalfDown, 4, 3, 2));

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
