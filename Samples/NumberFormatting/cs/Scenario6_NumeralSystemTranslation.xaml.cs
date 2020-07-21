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
    public sealed partial class Scenario6_NumeralSystemTranslation : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario6_NumeralSystemTranslation()
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
            // Text for which translation of the numeral system will be performed.  Please note that translation only happens between
            // Latin and any other supported numeral system.  Translation between numeral systems is not a supported scenario.
            String stringToTranslate = "These are the 10 digits of a numeral system: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9";

            // Variable where we keep the results of the scenario
            StringBuilder results = new StringBuilder();
            results.Append("Original string: " + stringToTranslate + "\n\n");

            // The numeral system translator is initialized based on the current application language.
            NumeralSystemTranslator numeralTranslator = new Windows.Globalization.NumberFormatting.NumeralSystemTranslator();

            // Do translation
            results.Append("Using application settings (" + numeralTranslator.NumeralSystem + "): ");
            results.Append(numeralTranslator.TranslateNumerals(stringToTranslate) + "\n");

            // Switch to a different numeral system
            numeralTranslator.NumeralSystem = "hanidec";

            // Do translation
            results.Append("Using numeral system via property (" + numeralTranslator.NumeralSystem + " ): ");
            results.Append(numeralTranslator.TranslateNumerals(stringToTranslate) + "\n");

            // Create a converter using a language list to initialize the numeral system to an appropriate default
            numeralTranslator = new Windows.Globalization.NumberFormatting.NumeralSystemTranslator(new string[] {"ar-SA", "en-US"}); 

            // Do translation
            results.Append("Using numeral system via language list (" + numeralTranslator.NumeralSystem + "): ");
            results.Append(numeralTranslator.TranslateNumerals(stringToTranslate) + "\n");

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
