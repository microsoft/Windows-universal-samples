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
#include "Scenario5_UnicodeExtensions.h"
#include "Scenario5_UnicodeExtensions.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario5_UnicodeExtensions::Scenario5_UnicodeExtensions()
    {
        InitializeComponent();
    }

    void Scenario5_UnicodeExtensions::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
        // to format a date/time by using a formatter that uses unicode extensions in the specified
        // language name

        // Get the current application context language
        hstring currentLanguage = ApplicationLanguages::Languages().GetAt(0);

        // Keep the results here
        std::wostringstream results;
        results << L"Current application context language: " << std::wstring_view(currentLanguage) << std::endl << std::endl;

        // Formatters for dates and times
        std::array dateFormatters{
            // Default application context  
            DateTimeFormatter(L"longdate longtime"),

            // Telugu language, Gregorian Calendar and Latin Numeral System
            DateTimeFormatter(L"longdate longtime", { L"te-in-u-ca-gregory-nu-latn", L"en-US" }),

            // Hebrew language and Arabic Numeral System - calendar NOT specified in constructor
            DateTimeFormatter(YearFormat::Default,
                MonthFormat::Default,
                DayFormat::Default,
                DayOfWeekFormat::Default,
                HourFormat::Default,
                MinuteFormat::Default,
                SecondFormat::Default,
                { L"he-IL-u-nu-arab", L"en-US" }),

                // Hebrew language and calendar - calendar specified in constructor
                // also, which overrides the one specified in Unicode extension
                DateTimeFormatter(YearFormat::Default,
                    MonthFormat::Default,
                    DayFormat::Default,
                    DayOfWeekFormat::Default,
                    HourFormat::Default,
                    MinuteFormat::Default,
                    SecondFormat::Default,
                    { L"he-IL-u-ca-hebrew-co-phonebk", L"en-US" },
                    L"US",
                    CalendarIdentifiers::Gregorian(),
                    ClockIdentifiers::TwentyFourHour()),
        };

        // Obtain the date that will be formatted.
        DateTime dateToFormat = clock::now();

        // Format and display date/time along with other relevant properites
        for (auto&& formatter : dateFormatters)
        {
            // Perform the actual formatting. 
            results << L"Result: " << std::wstring_view(formatter.Format(dateToFormat)) << std::endl;
            results << L"    Language List: " << std::wstring_view(StringJoin(L", ", formatter.Languages())) << std::endl;
            results << L"    Template: " << std::wstring_view(formatter.Template()) << std::endl;
            results << L"    Resolved Language: " << std::wstring_view(formatter.ResolvedLanguage()) << std::endl;
            results << L"    Calendar System: " << std::wstring_view(formatter.Calendar()) << std::endl;
            results << L"    Numeral System: " << std::wstring_view(formatter.NumeralSystem()) << std::endl << std::endl;
        }

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
