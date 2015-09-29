//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Text;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Globalization;
using Windows.Globalization.DateTimeFormatting;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_LongAndShortFormats : Page
    {
        public Scenario1_LongAndShortFormats()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Display' button.
        /// </summary>
        private void Display()
        {
            // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
            // in order to display dates and times using basic formatters.

            // We keep results in this variable
            StringBuilder results = new StringBuilder();
            results.AppendLine("Current application context language: " + ApplicationLanguages.Languages[0]);
            results.AppendLine();

            // Create basic date/time formatters.
            DateTimeFormatter[] basicFormatters = new[]
            {
                // Default date formatters
                new DateTimeFormatter("shortdate"),
                new DateTimeFormatter("longdate"),

                // Default time formatters
                new DateTimeFormatter("shorttime"),
                new DateTimeFormatter("longtime"),
             };

            // Create date/time to format and display.
            DateTime dateTime = DateTime.Now;

            // Format and display date/time. Calendar will always support Now. Otherwise you may need to verify dateTime is in supported range.
            foreach (DateTimeFormatter formatter in basicFormatters)
            {
                // Format and display date/time.
                results.AppendLine(formatter.Template + ": " + formatter.Format(dateTime));
            }

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
