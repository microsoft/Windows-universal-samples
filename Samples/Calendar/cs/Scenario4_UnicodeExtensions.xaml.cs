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
    public sealed partial class Scenario4_UnicodeExtensions : Page
    {
        public Scenario4_UnicodeExtensions()
        {
            this.InitializeComponent();
        }

        private string ReportCalendarData(Calendar calendar, string calendarLabel)
        {
            string results = calendarLabel + ": " + calendar.GetCalendarSystem() + "\n";
            results += "Calendar system: " + calendar.GetCalendarSystem() + "\n";
            results += "Numeral System: " + calendar.NumeralSystem + "\n";
            results += "Resolved Language " + calendar.ResolvedLanguage + "\n";
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

            // Create Calendar objects using different Unicode extensions for different languages.
            // NOTE: Calendar (ca) and numeral system (nu) are the only supported extensions with any others being ignored.
            // Note that collation (co) extension is ignored in the last example.
            Calendar cal1 = new Calendar();
            Calendar cal2 = new Calendar(new[] { "ar-SA-u-ca-gregory-nu-Latn" });
            Calendar cal3 = new Calendar(new[] { "he-IL-u-nu-arab" });
            Calendar cal4 = new Calendar(new[] { "he-IL-u-ca-hebrew-co-phonebk" });

            // Display individual date/time elements.
            OutputTextBlock.Text =
                ReportCalendarData(cal1, "User's default Calendar object") +
                ReportCalendarData(cal2, "Calendar object with Arabic language, Gregorian Calendar and Latin Numeral System (ar-SA-ca-gregory-nu-Latn)") +
                ReportCalendarData(cal3, "Calendar object with Hebrew language, Default Calendar for that language and Arab Numeral System (he-IL-u-nu-arab)") +
                ReportCalendarData(cal4, "Calendar object with Hebrew language, Hebrew Calendar, Default Numeral System for that language and Phonebook collation (he-IL-u-ca-hebrew-co-phonebk)");
        }
    }
}
