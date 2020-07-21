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
#include "Scenario4_Override.h"
#include "Scenario4_Override.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario4_Override::Scenario4_Override()
    {
        InitializeComponent();
    }

    void Scenario4_Override::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
        // to format a date/time by using a formatter that provides specific languages, 
        // geographic region, calendar and clock

        //Get the current default application context language
        hstring currentLanguage = ApplicationLanguages::Languages().GetAt(0);

        // Formatters for dates and times
        std::array dateFormatters{
            DateTimeFormatter(L"longdate", { L"ja-JP" }, L"JP", CalendarIdentifiers::Japanese(), ClockIdentifiers::TwelveHour()),

            DateTimeFormatter(L"month day", { L"fr-FR" }, L"FR", CalendarIdentifiers::Gregorian(), ClockIdentifiers::TwentyFourHour()),

            DateTimeFormatter(
                YearFormat::Abbreviated,
                MonthFormat::Abbreviated,
                DayFormat::Default,
                DayOfWeekFormat::None,
                HourFormat::None,
                MinuteFormat::None,
                SecondFormat::None,
                { L"de-DE" }, L"DE", CalendarIdentifiers::Gregorian(), ClockIdentifiers::TwelveHour()),

            DateTimeFormatter(L"longtime", { L"ja-JP" }, L"JP", CalendarIdentifiers::Japanese(), ClockIdentifiers::TwelveHour()),

            DateTimeFormatter(L"hour minute", { L"fr-FR" }, L"FR", CalendarIdentifiers::Gregorian(), ClockIdentifiers::TwentyFourHour()),

            DateTimeFormatter(
                YearFormat::None,
                MonthFormat::None,
                DayFormat::None,
                DayOfWeekFormat::None,
                HourFormat::Default,
                MinuteFormat::Default,
                SecondFormat::None,
                { L"de-DE" }, L"DE", CalendarIdentifiers::Gregorian(), ClockIdentifiers::TwelveHour()),
        };

        // Obtain the date that will be formatted.
        DateTime dateToFormat = clock::now();

        // Keep the results here
        std::wostringstream results;
        results << "Current default application context language: " << std::wstring_view(currentLanguage) << std::endl << std::endl;

        // Generate the results.
        for (auto&& formatter : dateFormatters)
        {
            // Perform the actual formatting. 
            results << std::wstring_view(formatter.Template()) << L": (" << std::wstring_view(formatter.ResolvedLanguage()) << L") " << std::wstring_view(formatter.Format(dateToFormat)) << std::endl;
        }

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
