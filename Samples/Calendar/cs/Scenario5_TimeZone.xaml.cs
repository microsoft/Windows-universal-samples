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

using System.Text;
using Windows.Foundation;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario5_TimeZone : Page
    {
        public Scenario5_TimeZone()
        {
            this.InitializeComponent();
        }

        private string ReportCalendarData(Calendar calendar)
        {
            // Display individual date/time elements.
            return string.Format("In {0} time zone:   {1}   {2} {3}, {4}   {5}:{6}:{7} {8}  {9}",
                                 calendar.GetTimeZone(),
                                 calendar.DayOfWeekAsSoloString(),
                                 calendar.MonthAsSoloString(),
                                 calendar.DayAsPaddedString(2),
                                 calendar.YearAsString(),
                                 calendar.HourAsPaddedString(2),
                                 calendar.MinuteAsPaddedString(2),
                                 calendar.SecondAsPaddedString(2),
                                 calendar.PeriodAsString(),
                                 calendar.TimeZoneAsString(3));
        }

        private void ShowResults()
        {
            // This scenario illustrates time zone support in Windows.Globalization.Calendar class

            // Displayed time zones in addition to the local time zone.
            string[] timeZones = new[] { "UTC", "America/New_York", "Asia/Kolkata" };

            // Store results here.
            StringBuilder results = new StringBuilder();

            // Create default Calendar object
            Calendar calendar = new Calendar();
            string localTimeZone = calendar.GetTimeZone();

            // Show current time in local time zone
            results.AppendLine("Current date and time:");
            results.AppendLine(ReportCalendarData(calendar));

            // Show current time in additional time zones
            foreach (string timeZone in timeZones)
            {
                calendar.ChangeTimeZone(timeZone);
                results.AppendLine(ReportCalendarData(calendar));
            }
            results.AppendLine();

            // Change back to local time zone
            calendar.ChangeTimeZone(localTimeZone);

            // Show a time on 14th day of second month of next year.
            // Note the effect of daylight saving time on the results.
            results.AppendLine("Same time on 14th day of second month of next year:");
            calendar.AddYears(1); calendar.Month = 2; calendar.Day = 14;
            results.AppendLine(ReportCalendarData(calendar));
            foreach (string timeZone in timeZones)
            {
                calendar.ChangeTimeZone(timeZone);
                results.AppendLine(ReportCalendarData(calendar));
            }
            results.AppendLine();

            // Change back to local time zone
            calendar.ChangeTimeZone(localTimeZone);

            // Show a time on 14th day of tenth month of next year.
            // Note the effect of daylight saving time on the results.
            results.AppendLine("Same time on 14th day of tenth month of next year:");
            calendar.AddMonths(8);
            results.AppendLine(ReportCalendarData(calendar));
            foreach (string timeZone in timeZones)
            {
                calendar.ChangeTimeZone(timeZone);
                results.AppendLine(ReportCalendarData(calendar));
            }
            results.AppendLine();

            // Display the results
            OutputTextBlock.Text = results.ToString();
        }
    }
}
