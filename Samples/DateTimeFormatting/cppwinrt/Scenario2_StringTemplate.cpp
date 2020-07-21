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
#include "Scenario2_StringTemplate.h"
#include "Scenario2_StringTemplate.g.cpp"

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
    Scenario2_StringTemplate::Scenario2_StringTemplate()
    {
        InitializeComponent();
    }

    void Scenario2_StringTemplate::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
        // to format a date/time via a string template.  Note that the order specifed in the string pattern does 
        // not determine the order of the parts of the formatted string.  The current application language and region value will
        // determine the pattern of the date returned based on the specified parts.

        //Get the current application context language
        hstring currentLanguage = ApplicationLanguages::Languages().GetAt(0);

        // Formatters for dates.
        std::array dateFormatters{
            DateTimeFormatter(L"month day"),
            DateTimeFormatter(L"month year"),
            DateTimeFormatter(L"month day year"),
            DateTimeFormatter(L"month day dayofweek year"),
            DateTimeFormatter(L"dayofweek.abbreviated"),
            DateTimeFormatter(L"month.abbreviated"),
            DateTimeFormatter(L"year.abbreviated"),
        };

        // Formatters for time.
        std::array timeFormatters{
            DateTimeFormatter(L"hour minute"),
            DateTimeFormatter(L"hour minute second"),
            DateTimeFormatter(L"hour"),
        };

        // Formatters for timezone.
        std::array timeZoneFormatters{
            DateTimeFormatter(L"timezone"),
            DateTimeFormatter(L"timezone.full"),
            DateTimeFormatter(L"timezone.abbreviated"),
        };

        // Formatters for combinations.
        std::array combinationFormatters{
            DateTimeFormatter(L"hour minute second timezone.full"),
            DateTimeFormatter(L"day month year hour minute timezone"),
            DateTimeFormatter(L"dayofweek day month year hour minute second"),
            DateTimeFormatter(L"dayofweek.abbreviated day month hour minute"),
            DateTimeFormatter(L"dayofweek day month year hour minute second timezone.abbreviated"),
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

        results << std::endl;
        results << L"Time zones:" << std::endl;

        // Generate the results.
        for (auto&& formatter : timeZoneFormatters)
        {
            GenerateResult(results, formatter, dateToFormat);
        }

        results << std::endl;
        results << L"Date and time combinations:" << std::endl;

        // Generate the results.
        for (auto&& formatter : combinationFormatters)
        {
            GenerateResult(results, formatter, dateToFormat);
        }

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
