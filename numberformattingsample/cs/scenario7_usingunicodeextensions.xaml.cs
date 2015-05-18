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
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario7_UsingUnicodeExtensions: Page
    {
        public Scenario7_UsingUnicodeExtensions()
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
            // This scenario uses language tags with unicode extensions to construct and use the various 
            // formatters and NumeralSystemTranslator in Windows.Globalization.NumberFormatting to format numbers 

            // Keep results of the scenario in a StringBuilder
            StringBuilder results = new StringBuilder();

            // Create number to format.
            double randomNumber = (new Random().NextDouble() * 100000);
            results.AppendLine("Random number used by formatters: " + randomNumber);
            // Create a string to translate
            String stringToTranslate = "These are the 10 digits of a numeral system: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9";
            results.AppendLine("String used by NumeralSystemTranslater: " + stringToTranslate);
            // Create the rounder and set its increment to 0.01
            IncrementNumberRounder rounder = new Windows.Globalization.NumberFormatting.IncrementNumberRounder();
            rounder.Increment = 0.001;
            results.AppendLine("CurrencyFormatter will be using Euro symbol and all formatters rounding to 0.001 increments");
            results.AppendLine();

            // The list of language tags with unicode extensions we want to test
            String[] languages = new[] { "de-DE-u-nu-telu-ca-buddist-co-phonebk-cu-usd", "ja-jp-u-nu-arab"};
            
            // Create the various formatters, now using the language list with unicode extensions            
            results.AppendLine("Creating formatters using following languages in the language list:");
            foreach(String language in languages)
            {
                results.AppendLine("\t" + language);
            }
            results.AppendLine();

            // Create the various formatters.
            DecimalFormatter decimalFormatter = new DecimalFormatter(languages, "ZZ");
            decimalFormatter.NumberRounder = rounder; decimalFormatter.FractionDigits = 2;
            CurrencyFormatter currencyFormatter = new CurrencyFormatter(CurrencyIdentifiers.EUR, languages, "ZZ");
            currencyFormatter.NumberRounder = rounder; currencyFormatter.FractionDigits = 2;
            PercentFormatter percentFormatter = new PercentFormatter(languages, "ZZ");
            percentFormatter.NumberRounder = rounder; percentFormatter.FractionDigits = 2;
            PermilleFormatter permilleFormatter = new PermilleFormatter(languages, "ZZ");
            permilleFormatter.NumberRounder = rounder; permilleFormatter.FractionDigits = 2;
            NumeralSystemTranslator numeralTranslator = new NumeralSystemTranslator(languages);

            results.AppendLine("Using resolved language  " + decimalFormatter.ResolvedLanguage + "  : (note that all extension tags other than nu are ignored)");
            // Format using formatters and translate using NumeralSystemTranslator.
            results.AppendLine("Decimal Formatted:   " + decimalFormatter.Format(randomNumber));
            results.AppendLine("Currency formatted:   " + currencyFormatter.Format(randomNumber));
            results.AppendLine("Percent formatted:   " + percentFormatter.Format(randomNumber));
            results.AppendLine("Permille formatted:   " + permilleFormatter.Format(randomNumber));
            results.AppendLine("NumeralTranslator translated:   " + numeralTranslator.TranslateNumerals(stringToTranslate));
            results.AppendLine();

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
