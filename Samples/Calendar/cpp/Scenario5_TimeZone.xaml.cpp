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
#include "Scenario5_TimeZone.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;

Scenario5_TimeZone::Scenario5_TimeZone()
{
    InitializeComponent();
}

String^ Scenario5_TimeZone::ReportCalendarData(Calendar^ calendar)
{
    // Display individual date/time elements.
    return
        "In " + calendar->GetTimeZone() +
        " time zone:   " + calendar->DayOfWeekAsSoloString() +
        "   " + calendar->MonthAsSoloString() +
        " " + calendar->DayAsPaddedString(2) +
        ", " + calendar->YearAsString() +
        "   " + calendar->HourAsPaddedString(2) +
        ":" + calendar->MinuteAsPaddedString(2) +
        ":" + calendar->SecondAsPaddedString(2) +
        " " + calendar->PeriodAsString() +
        "  " + calendar->TimeZoneAsString(3) +
        "\n";
}

void Scenario5_TimeZone::ShowResults()
{
    // This scenario illustrates time zone support in Windows.Globalization.Calendar class

    // Displayed time zones in addition to the local time zone.
    String^ timeZones[] = { "UTC", "America/New_York", "Asia/Kolkata" };

    // Store results here.
    String^ results = "";

    // Create default Calendar object
    Calendar^ calendar = ref new Calendar();
    String^ localTimeZone = calendar->GetTimeZone();

    // Show current time in local time zone
    results += "Current date and time:\n";
    results += ReportCalendarData(calendar) + "\n";

    // Show current time in additional time zones
    for (String^ timeZone : timeZones)
    {
        calendar->ChangeTimeZone(timeZone);
        results += ReportCalendarData(calendar);
    }
    results += "\n";

    // Change back to local time zone
    calendar->ChangeTimeZone(localTimeZone);

    // Show a time on 14th day of second month of next year.
    // Note the effect of daylight saving time on the results.
    results += "Same time on 14th day of second month of next year:\n";
    calendar->AddYears(1); calendar->Month = 2; calendar->Day = 14;
    results += ReportCalendarData(calendar);
    for (String^ timeZone : timeZones)
    {
        calendar->ChangeTimeZone(timeZone);
        results += ReportCalendarData(calendar);
    }
    results += "\n";

    // Change back to local time zone
    calendar->ChangeTimeZone(localTimeZone);

    // Show a time on 14th day of tenth month of next year.
    // Note the effect of daylight saving time on the results.
    results += "Same time on 14th day of tenth month of next year:\n";
    calendar->AddMonths(8);
    results += ReportCalendarData(calendar);
    for (String^ timeZone : timeZones)
    {
        calendar->ChangeTimeZone(timeZone);
        results += ReportCalendarData(calendar);
    }
    results += "\n";

    // Display the results
    OutputTextBlock->Text = results;
}