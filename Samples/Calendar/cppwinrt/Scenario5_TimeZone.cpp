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
#include "Scenario5_TimeZone.h"
#include "Scenario5_TimeZone.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;

namespace
{
    void ReportCalendarData(std::wostream& results, Calendar const& calendar)
    {
        // Display individual date/time elements.
        results <<
            L"In " << std::wstring_view(calendar.GetTimeZone()) <<
            L" time zone:   " << std::wstring_view(calendar.DayOfWeekAsSoloString()) <<
            L"   " << std::wstring_view(calendar.MonthAsSoloString()) <<
            L" " << std::wstring_view(calendar.DayAsPaddedString(2)) <<
            L", " << std::wstring_view(calendar.YearAsString()) <<
            L"   " << std::wstring_view(calendar.HourAsPaddedString(2)) <<
            L":" << std::wstring_view(calendar.MinuteAsPaddedString(2)) <<
            L":" << std::wstring_view(calendar.SecondAsPaddedString(2)) <<
            L" " << std::wstring_view(calendar.PeriodAsString()) <<
            L"  " << std::wstring_view(calendar.TimeZoneAsString(3)) <<
            L"\n";
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario5_TimeZone::Scenario5_TimeZone()
    {
        InitializeComponent();
    }

    void Scenario5_TimeZone::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario illustrates time zone support in Windows.Globalization.Calendar class

        // Displayed time zones in addition to the local time zone.
        std::array timeZones{ L"UTC", L"America/New_York", L"Asia/Kolkata" };

        // Store results here.
        std::wostringstream results;

        // Create default Calendar object
        Calendar calendar;
        hstring localTimeZone = calendar.GetTimeZone();

        // Show current time in local time zone
        results << L"Current date and time:\n";
        ReportCalendarData(results, calendar);
        results << L"\n";

        // Show current time in additional time zones
        for (auto&& timeZone : timeZones)
        {
            calendar.ChangeTimeZone(timeZone);
            ReportCalendarData(results, calendar);
        }
        results << L"\n";

        // Change back to local time zone
        calendar.ChangeTimeZone(localTimeZone);

        // Show a time on 14th day of second month of next year.
        // Note the effect of daylight saving time on the results.
        results << L"Same time on 14th day of second month of next year:\n";
        calendar.AddYears(1); calendar.Month(2); calendar.Day(14);
        ReportCalendarData(results, calendar);
        for (auto&& timeZone : timeZones)
        {
            calendar.ChangeTimeZone(timeZone);
            ReportCalendarData(results, calendar);
        }
        results << L"\n";

        // Change back to local time zone
        calendar.ChangeTimeZone(localTimeZone);

        // Show a time on 14th day of tenth month of next year.
        // Note the effect of daylight saving time on the results.
        results << L"Same time on 14th day of tenth month of next year:\n";
        calendar.AddMonths(8);
        ReportCalendarData(results, calendar);
        for (auto&& timeZone : timeZones)
        {
            calendar.ChangeTimeZone(timeZone);
            ReportCalendarData(results, calendar);
        }
        results << L"\n";

        // Display the results
        OutputTextBlock().Text(results.str());
    }
}
