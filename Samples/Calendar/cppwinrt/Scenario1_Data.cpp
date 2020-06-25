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
#include "Scenario1_Data.h"
#include "Scenario1_Data.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;

namespace
{
    void ReportCalendarData(std::wostream& results, Calendar const& calendar, PCWSTR calendarLabel)
    {
        results << calendarLabel << L": " << std::wstring_view(calendar.GetCalendarSystem()) << L"\n" <<
        L"Name of Month: " << std::wstring_view(calendar.MonthAsSoloString()) << L"\n" <<
        L"Day of Month: " << std::wstring_view(calendar.DayAsPaddedString(2)) << L"\n" <<
        L"Day of Week: " << std::wstring_view(calendar.DayOfWeekAsSoloString()) << L"\n" <<
        L"Year: " << std::wstring_view(calendar.YearAsString()) << L"\n\n";
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Data::Scenario1_Data()
    {
        InitializeComponent();
    }

    void Scenario1_Data::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
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
