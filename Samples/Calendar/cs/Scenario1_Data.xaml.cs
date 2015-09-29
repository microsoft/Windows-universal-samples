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
    public sealed partial class Scenario1_Data : Page
    {
        public Scenario1_Data()
        {
            this.InitializeComponent();
        }

        private string ReportCalendarData(Calendar calendar, string calendarLabel)
        {
            string results = calendarLabel + ": " + calendar.GetCalendarSystem() + "\n";
            results += "Name of Month: " + calendar.MonthAsSoloString() + "\n";
            results += "Day of Month: " + calendar.DayAsPaddedString(2) + "\n";
            results += "Day of Week: " + calendar.DayOfWeekAsSoloString() + "\n";
            results += "Year: " + calendar.YearAsString() + "\n";
            results += "\n";
            return results;
        }

        private void ShowResults()
        {
            // This scenario uses the Windows.Globalization.Calendar class to display the parts of a date.

            // Create Calendar objects using different constructors.
            Calendar calendar = new Calendar();
            Calendar japaneseCalendar = new Calendar(new[] { "ja-JP" }, CalendarIdentifiers.Japanese, ClockIdentifiers.TwelveHour);
            Calendar hebrewCalendar = new Calendar(new[] { "he-IL" }, CalendarIdentifiers.Hebrew, ClockIdentifiers.TwentyFourHour);

            // Display the results
            OutputTextBlock.Text =
                ReportCalendarData(calendar, "User's default calendar system") +
                ReportCalendarData(japaneseCalendar, "Calendar system") +
                ReportCalendarData(hebrewCalendar, "Calendar system");
        }
    }
}
