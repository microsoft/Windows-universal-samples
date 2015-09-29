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
    public sealed partial class Scenario5_UnicodeExtensions : Page
    {
        public Scenario5_UnicodeExtensions()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Display' button.
        /// </summary>
        private void Display()
        {
            // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
            // to format a date/time by using a formatter that uses Unicode extenstion in the specified
            // language name

            // We keep results in this variable
            StringBuilder results = new StringBuilder();
            results.AppendLine("Current default application context language: " + ApplicationLanguages.Languages[0]);
            results.AppendLine();

            // Create formatters using various types of constructors specifying Language list with unicode extension in language names
            DateTimeFormatter[] unicodeExtensionFormatters = new[]
            {
                // Default application context  
                new DateTimeFormatter("longdate longtime"),
                // Telugu language, Gregorian Calendar and Latin Numeral System
                new DateTimeFormatter("longdate longtime", new[] { "te-in-u-ca-gregory-nu-latn", "en-US" }),
                // Hebrew language and Arabic Numeral System - calendar NOT specified in constructor
                new DateTimeFormatter(YearFormat.Default, 
                    MonthFormat.Default, 
                    DayFormat.Default, 
                    DayOfWeekFormat.Default,
                    HourFormat.Default,
                    MinuteFormat.Default,
                    SecondFormat.Default,
                    new[] { "he-IL-u-nu-arab", "en-US" }),             
                // Hebrew language and calendar - calendar specified in constructor
                // also, which overrides the one specified in Unicode extension
                new DateTimeFormatter(YearFormat.Default, 
                    MonthFormat.Default, 
                    DayFormat.Default, 
                    DayOfWeekFormat.Default,
                    HourFormat.Default,
                    MinuteFormat.Default,
                    SecondFormat.Default,
                    new[] { "he-IL-u-ca-hebrew-co-phonebk", "en-US" },
                    "US",
                    CalendarIdentifiers.Gregorian,
                    ClockIdentifiers.TwentyFourHour), 
             };

            // Create date/time to format and display.
            DateTime dateTime = DateTime.Now;

            // Format and display date/time along with other relevant properites
            foreach (DateTimeFormatter formatter in unicodeExtensionFormatters)
            {
                // Perform the actual formatting. 
                results.AppendLine("Result: " + formatter.Format(dateTime));
                results.AppendLine("    Language List: " + string.Join(", ", formatter.Languages));
                results.AppendLine("    Template: " + formatter.Template);
                results.AppendLine("    Resolved Language: " + formatter.ResolvedLanguage);
                results.AppendLine("    Calendar System: " + formatter.Calendar);
                results.AppendLine("    Numeral System: " + formatter.NumeralSystem);
                results.AppendLine();
            }

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
