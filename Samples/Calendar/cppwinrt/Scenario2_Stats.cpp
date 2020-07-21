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
#include "Scenario2_Stats.h"
#include "Scenario2_Stats.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;

namespace
{
    void ReportCalendarData(std::wostream& results, Calendar const& calendar, PCWSTR calendarLabel)
    {
        results << calendarLabel << L": " << std::wstring_view(calendar.GetCalendarSystem()) << L"\n" <<
            L"Months in this Year: " << calendar.NumberOfMonthsInThisYear() << L"\n" <<
            L"Days in this Month: " << calendar.NumberOfDaysInThisMonth() << L"\n" <<
            L"Hours in this Period: " << calendar.NumberOfHoursInThisPeriod() << L"\n" <<
            L"Era: " << std::wstring_view(calendar.EraAsString()) << L"\n\n";
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario2_Stats::Scenario2_Stats()
    {
        InitializeComponent();
    }

    void Scenario2_Stats::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.Calendar class to display the parts of a date.

        // Create Calendar objects using different constructors.
        Calendar calendar;
        Calendar japaneseCalendar({ L"ja-JP" }, CalendarIdentifiers::Japanese(), ClockIdentifiers::TwelveHour());
        Calendar hebrewCalendar({ L"he-IL" }, CalendarIdentifiers::Hebrew(), ClockIdentifiers::TwentyFourHour());

        // Generate the results
        std::wostringstream results;
        ReportCalendarData(results, calendar, L"User's default calendar system");
        ReportCalendarData(results, japaneseCalendar, L"Calendar system");
        ReportCalendarData(results, hebrewCalendar, L"Calendar system");
        OutputTextBlock().Text(results.str());
    }
}
