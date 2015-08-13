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
    public sealed partial class Scenario3_ParameterizedTemplate : Page
    {
        public Scenario3_ParameterizedTemplate()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Display' button.
        /// </summary>
        private void Display()
        {
            // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
            // to format a date/time by specifying a template via parameters.  Note that the user's language
            // and region preferences will determine the pattern of the date returned based on the
            // specified parts.

            // We keep results in this variable
            StringBuilder results = new StringBuilder();
            results.AppendLine("Current application context language: " + ApplicationLanguages.Languages[0]);
            results.AppendLine();

            // Create formatters with individual format specifiers for date/time elements.
            DateTimeFormatter[] dateFormatters = new[]
            {
                // Example formatters for dates.
                new DateTimeFormatter(
                    YearFormat.Full, 
                    MonthFormat.Abbreviated, 
                    DayFormat.Default, 
                    DayOfWeekFormat.Abbreviated),
                new DateTimeFormatter(
                    YearFormat.Abbreviated, 
                    MonthFormat.Abbreviated, 
                    DayFormat.Default, 
                    DayOfWeekFormat.None),
                new DateTimeFormatter(
                    YearFormat.Full, 
                    MonthFormat.Full, 
                    DayFormat.None, 
                    DayOfWeekFormat.None),
                new DateTimeFormatter(
                    YearFormat.None, 
                    MonthFormat.Full, 
                    DayFormat.Default, 
                    DayOfWeekFormat.None)
            };

            // Create formatters with individual format specifiers for time elements.
            DateTimeFormatter[] timeFormatters = new[]
            {
                // Example formatters for times.
                new DateTimeFormatter(
                    HourFormat.Default, 
                    MinuteFormat.Default, 
                    SecondFormat.Default),
                new DateTimeFormatter(
                    HourFormat.Default, 
                    MinuteFormat.Default, 
                    SecondFormat.None),
                new DateTimeFormatter(
                    HourFormat.Default, 
                    MinuteFormat.None, 
                    SecondFormat.None),
             };

            // Create date/time to format and display.
            DateTime dateTime = DateTime.Now;

            results.AppendLine("Dates:");

            // Format and display date/time. Calendar always supports Now. Otherwise you may need to verify dateTime is in supported range.
            foreach (DateTimeFormatter formatter in dateFormatters)
            {
                // Format and display date/time.
                results.AppendLine(formatter.Template + ": " + formatter.Format(dateTime));
            }

            results.AppendLine();
            results.AppendLine("Times:");

            // Format and display date/time. Calendar always supports Now. Otherwise you may need to verify dateTime is in supported range.
            foreach (DateTimeFormatter formatter in timeFormatters)
            {
                // Format and display date/time.
                results.AppendLine(formatter.Template + ": " + formatter.Format(dateTime));
            }

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
