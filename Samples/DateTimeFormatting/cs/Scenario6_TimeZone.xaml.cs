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
    public sealed partial class Scenario6_TimeZone : Page
    {
        public Scenario6_TimeZone()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Display' button.
        /// </summary>
        private void Display()
        {
            // This scenario illustrates time zone support in DateTimeFormatter class

            // Displayed time zones (other than local time zone)
            String[] timeZones = new String[] { "UTC", "America/New_York", "Asia/Kolkata" };

            // Store results here.
            StringBuilder results = new StringBuilder();

            // Create formatter object using longdate and longtime template
            DateTimeFormatter formatter = new DateTimeFormatter("longdate longtime");

            // Create date/time to format and display.
            DateTime dateTime = DateTime.Now;

            // Show current time.
            results.AppendLine("Current date and time");
            results.AppendLine("In Local time zone: " + formatter.Format(dateTime));
            foreach (String timeZone in timeZones)
            {
                results.AppendLine("In " + timeZone + " time zone: " + formatter.Format(dateTime, timeZone));
            }
            results.AppendLine();

            // Show a time on 14th day of second month of next year.
            // Note the effect of daylight saving on the results.
            results.AppendLine("Same time on 14th day of second month of next year");
            dateTime = new DateTime(dateTime.Year + 1, 2, 14, dateTime.Hour, dateTime.Minute, dateTime.Second);
            results.AppendLine("In Local time zone: " + formatter.Format(dateTime));
            foreach (String timeZone in timeZones)
            {
                results.AppendLine("In " + timeZone + " time zone: " + formatter.Format(dateTime, timeZone));
            }
            results.AppendLine();

            // Show a time on 14th day of tenth month of next year.
            // Note the effect of daylight saving on the results.
            results.AppendLine("Same time on 14th day of tenth month of next year");
            dateTime = dateTime.AddMonths(8);
            results.AppendLine("In Local time zone: " + formatter.Format(dateTime));
            foreach (String timeZone in timeZones)
            {
                results.AppendLine("In " + timeZone + " time zone: " + formatter.Format(dateTime, timeZone));
            }
            results.AppendLine();

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
