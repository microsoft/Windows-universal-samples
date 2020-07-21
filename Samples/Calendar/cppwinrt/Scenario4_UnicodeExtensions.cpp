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
#include "Scenario4_UnicodeExtensions.h"
#include "Scenario4_UnicodeExtensions.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::UI::Xaml;

namespace
{
    void ReportCalendarData(std::wostream& results, Calendar const& calendar, PCWSTR calendarLabel)
    {
        results <<
            calendarLabel << L": " << std::wstring_view(calendar.GetCalendarSystem()) << "\n" <<
            L"Calendar system: " << std::wstring_view(calendar.GetCalendarSystem()) << L"\n" <<
            L"Numeral System: " << std::wstring_view(calendar.NumeralSystem()) << L"\n" <<
            L"Resolved Language " << std::wstring_view(calendar.ResolvedLanguage()) << L"\n" <<
            L"Name of Month: " << std::wstring_view(calendar.MonthAsSoloString()) << L"\n" <<
            L"Day of Month: " << std::wstring_view(calendar.DayAsPaddedString(2)) << L"\n" <<
            L"Day of Week: " << std::wstring_view(calendar.DayOfWeekAsSoloString()) << L"\n" <<
            L"Year: " << std::wstring_view(calendar.YearAsString()) << L"\n" <<
            L"\n";
    }

}

namespace winrt::SDKTemplate::implementation
{
    Scenario4_UnicodeExtensions::Scenario4_UnicodeExtensions()
    {
        InitializeComponent();
    }

    void Scenario4_UnicodeExtensions::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.Calendar class to display the parts of a date.

        // Create Calendar objects using different Unicode extensions for different languages.
        // NOTE: Calendar (ca) and numeral system (nu) are the only supported extensions with any others being ignored.
        // Note that collation (co) extension is ignored in the last example.
        Calendar cal1;
        Calendar cal2({ L"ar-SA-u-ca-gregory-nu-Latn" });
        Calendar cal3({ L"he-IL-u-nu-arab" });
        Calendar cal4({ L"he-IL-u-ca-hebrew-co-phonebk" });

        // Generate the results
        std::wostringstream results;
        ReportCalendarData(results, cal1, L"User's default calendar system");
        ReportCalendarData(results, cal2, L"Calendar object with Arabic language, Gregorian Calendar and Latin Numeral System (ar-SA-ca-gregory-nu-Latn)");
        ReportCalendarData(results, cal3, L"Calendar object with Hebrew language, Default Calendar for that language and Arab Numeral System (he-IL-u-nu-arab)");
        ReportCalendarData(results, cal4, L"Calendar object with Hebrew language, Hebrew Calendar, Default Numeral System for that language and Phonebook collation (he-IL-u-ca-hebrew-co-phonebk)");
        OutputTextBlock().Text(results.str());
    }
}
