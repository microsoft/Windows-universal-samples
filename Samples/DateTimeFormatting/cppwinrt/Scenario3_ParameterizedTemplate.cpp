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
#include "Scenario3_ParameterizedTemplate.h"
#include "Scenario3_ParameterizedTemplate.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;

namespace
{
    void GenerateResult(std::wostream& results, DateTimeFormatter const& formatter, DateTime dateToFormat)
    {
        // Perform the actual formatting. 
        results << std::wstring_view(formatter.Template()) << L": " << std::wstring_view(formatter.Format(dateToFormat)) << std::endl;
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario3_ParameterizedTemplate::Scenario3_ParameterizedTemplate()
    {
        InitializeComponent();
    }

    void Scenario3_ParameterizedTemplate::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
        // to format a date/time by specifying a template via parameters.  Note that the current application language 
        // language and region value will determine the pattern of the date returned based on the 
        // specified parts.

        //Get the current application context language
        hstring currentLanguage = ApplicationLanguages::Languages().GetAt(0);

        // Formatters for dates
        std::array dateFormatters{
            DateTimeFormatter(
                YearFormat::Full,
                MonthFormat::Abbreviated,
                DayFormat::Default,
                DayOfWeekFormat::Abbreviated),

            DateTimeFormatter(
                YearFormat::Abbreviated,
                MonthFormat::Abbreviated,
                DayFormat::Default,
                DayOfWeekFormat::None),

            DateTimeFormatter(
                YearFormat::Full,
                MonthFormat::Full,
                DayFormat::None,
                DayOfWeekFormat::None),

            DateTimeFormatter(
                YearFormat::None,
                MonthFormat::Full,
                DayFormat::Default,
                DayOfWeekFormat::None),
        };

        // Formatters for times
        std::array timeFormatters{
            DateTimeFormatter(
                HourFormat::Default,
                MinuteFormat::Default,
                SecondFormat::Default),

            DateTimeFormatter(
                HourFormat::Default,
                MinuteFormat::Default,
                SecondFormat::None),

            DateTimeFormatter(
                HourFormat::Default,
                MinuteFormat::None,
                SecondFormat::None),
        };

        // Obtain the date that will be formatted.
        DateTime dateToFormat = clock::now();

        // Keep the results here
        std::wostringstream results;
        results << L"Current application context language: " << std::wstring_view(currentLanguage) << std::endl << std::endl;
        results << L"Dates:" << std::endl;

        // Generate the results.
        for (auto&& formatter : dateFormatters)
        {
            GenerateResult(results, formatter, dateToFormat);
        }

        results << std::endl;
        results << L"Times:" << std::endl;

        // Generate the results.
        for (auto&& formatter : timeFormatters)
        {
            GenerateResult(results, formatter, dateToFormat);
        }

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
