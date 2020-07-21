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
#include "pch.h"
#include "Scenario1_Data.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;

Scenario1_Data::Scenario1_Data()
{
    InitializeComponent();
}

String^ Scenario1_Data::ReportCalendarData(Calendar^ calendar, String^ calendarLabel)
{
    String^ results = calendarLabel + ": " + calendar->GetCalendarSystem() + "\n";
    results += "Name of Month: " + calendar->MonthAsSoloString() + "\n";
    results += "Day of Month: " + calendar->DayAsPaddedString(2) + "\n";
    results += "Day of Week: " + calendar->DayOfWeekAsSoloString() + "\n";
    results += "Year: " + calendar->YearAsString() + "\n";
    results += "\n";
    return results;
}


void Scenario1_Data::ShowResults()
{
    // This scenario uses the Windows.Globalization.Calendar class to display the parts of a date.

    // Create Calendar objects using different constructors.
    Calendar^ calendar = ref new Calendar();
    Calendar^ japaneseCalendar = ref new Calendar(ref new Vector<String^>({ "ja-JP" }), CalendarIdentifiers::Japanese, ClockIdentifiers::TwelveHour);
    Calendar^ hebrewCalendar = ref new Calendar(ref new Vector<String^>({ "he-IL" }), CalendarIdentifiers::Hebrew, ClockIdentifiers::TwentyFourHour);

    // Display the results
    OutputTextBlock->Text =
        ReportCalendarData(calendar, "User's default calendar system") +
        ReportCalendarData(japaneseCalendar, "Calendar system") +
        ReportCalendarData(hebrewCalendar, "Calendar system");
}