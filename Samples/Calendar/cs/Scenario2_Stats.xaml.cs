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

using Windows.Foundation;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Stats : Page
    {
        public Scenario2_Stats()
        {
            this.InitializeComponent();
        }

        private string ReportCalendarData(Calendar calendar, string calendarLabel)
        {
            string results = calendarLabel + ": " + calendar.GetCalendarSystem() + "\n";
            results += "Months in this Year: " + calendar.NumberOfMonthsInThisYear + "\n";
            results += "Days in this Month: " + calendar.NumberOfDaysInThisMonth + "\n";
            results += "Hours in this Period: " + calendar.NumberOfHoursInThisPeriod + "\n";
            results += "Era: " + calendar.EraAsString() + "\n";
            results += "\n";
            return results;
        }

        private void ShowResults()
        {
            // This scenario uses the Windows.Globalization.Calendar class to display the calendar
            // system statistics.

            // Create Calendar objects using different constructors.
            Calendar calendar = new Calendar();
            Calendar japaneseCalendar = new Calendar(new[] { "ja-JP" }, CalendarIdentifiers.Japanese, ClockIdentifiers.TwelveHour);
            Calendar hebrewCalendar = new Calendar(new[] { "he-IL" }, CalendarIdentifiers.Hebrew, ClockIdentifiers.TwentyFourHour);

            // Display results
            OutputTextBlock.Text =
                ReportCalendarData(calendar, "User's default calendar system") +
                ReportCalendarData(japaneseCalendar, "Calendar system") +
                ReportCalendarData(hebrewCalendar, "Calendar system");
        }
    }
}
