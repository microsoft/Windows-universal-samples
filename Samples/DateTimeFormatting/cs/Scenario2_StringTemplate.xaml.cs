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
    public sealed partial class Scenario2_StringTemplate : Page
    {
        public Scenario2_StringTemplate()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Display' button.
        /// </summary>
        private void Display()
        {
            // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
            // to format a date/time via a string template.  Note that the order specifed in the string pattern does
            // not determine the order of the parts of the formatted string.  The user's language and region preferences will
            // determine the pattern of the date returned based on the specified parts.

            // Create template-based date/time formatters.
            DateTimeFormatter[] dateFormatters = new[]
            {
                // Formatters for dates.
                new DateTimeFormatter("month day"),
                new DateTimeFormatter("month year"),
                new DateTimeFormatter("month day year"),
                new DateTimeFormatter("month day dayofweek year"),
                new DateTimeFormatter("dayofweek.abbreviated"),
                new DateTimeFormatter("month.abbreviated"),
                new DateTimeFormatter("year.abbreviated")
            };

            // Create template-based date/time formatters.
            DateTimeFormatter[] timeFormatters = new[]
            {
                // Formatters for time.
                new DateTimeFormatter("hour minute"),
                new DateTimeFormatter("hour minute second"),
                new DateTimeFormatter("hour")
            };

            // Create template-based date/time formatters.
            DateTimeFormatter[] timeZoneFormatters = new[]
            {
                // Formatters for time zone.
                new DateTimeFormatter("timezone"),
                new DateTimeFormatter("timezone.full"),
                new DateTimeFormatter("timezone.abbreviated")
            };
                
            // Create template-based date/time formatters.
            DateTimeFormatter[] combinationFormatters = new[]
            {
                // Formatters for combinations.
                new DateTimeFormatter("hour minute second timezone.full"),
                new DateTimeFormatter("day month year hour minute timezone"),
                new DateTimeFormatter("dayofweek day month year hour minute second"),
                new DateTimeFormatter("dayofweek.abbreviated day month hour minute"),
                new DateTimeFormatter("dayofweek day month year hour minute second timezone.abbreviated"),
             };

            // Create date/time to format and display.
            DateTime dateTime = DateTime.Now;

            // We keep results in this variable
            StringBuilder results = new StringBuilder();
            results.AppendLine("Current application context language: " + ApplicationLanguages.Languages[0]);
            results.AppendLine();
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

            results.AppendLine();
            results.AppendLine("Time zones:");

            // Format and display date/time. Calendar always supports Now. Otherwise you may need to verify dateTime is in supported range.
            foreach (DateTimeFormatter formatter in timeZoneFormatters)
            {
                // Format and display date/time.
                results.AppendLine(formatter.Template + ": " + formatter.Format(dateTime));
            }

            results.AppendLine();
            results.AppendLine("Date and time combinations:");

            // Format and display date/time. Calendar always supports Now. Otherwise you may need to verify dateTime is in supported range.
            foreach (DateTimeFormatter formatter in combinationFormatters)
            {
                // Format and display date/time.
                results.AppendLine(formatter.Template + ": " + formatter.Format(dateTime));
            }

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
