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
#include "Scenario6_TimeZone.h"
#include "Scenario6_TimeZone.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;

namespace
{
    void GenerateResult(std::wostream& results, DateTimeFormatter const& formatter, PCWSTR timeZone, DateTime dateToFormat)
    {
        // Perform the actual formatting. 
        results << L"In " << timeZone << L" time zone: " <<
            std::wstring_view(formatter.Format(dateToFormat, timeZone)) << std::endl;
    }
}

namespace winrt::SDKTemplate::implementation
{
    Scenario6_TimeZone::Scenario6_TimeZone()
    {
        InitializeComponent();
    }

    void Scenario6_TimeZone::Display_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario illustrates time zone support in DateTimeFormatter class

        // Displayed time zones (other than local time zone)
        std::array timeZones{ L"UTC", L"America/New_York", L"Asia/Kolkata" };

        // Keep the results here
        std::wostringstream results;

        // Create formatter object using longdate and longtime template
        DateTimeFormatter dateFormatter(L"longdate longtime");

        // Create date/time to format and display.
        Calendar calendar;
        DateTime dateToFormat = calendar.GetDateTime();

        // Show current time.
        results << L"Current date and time" << std::endl <<
            L"In Local time zone: " << std::wstring_view(dateFormatter.Format(dateToFormat)) << std::endl;
        for (auto&& timeZone : timeZones)
        {
            GenerateResult(results, dateFormatter, timeZone, dateToFormat);
        }
        results << std::endl;

        // Show a time on 14th day of second month of next year.
        // Note the effect of daylight saving on the results.
        calendar.AddYears(1); calendar.Month(2); calendar.Day(14);
        dateToFormat = calendar.GetDateTime();
        results << L"Same time on 14th day of second month of next year" << std::endl <<
            L"In Local time zone: " << std::wstring_view(dateFormatter.Format(dateToFormat)) << std::endl;
        for (auto timeZone : timeZones)
        {
            GenerateResult(results, dateFormatter, timeZone, dateToFormat);
        }
        results << std::endl;

        // Show a time on 14th day of tenth month of next year.
        // Note the effect of daylight saving on the results.
        calendar.AddMonths(8);
        dateToFormat = calendar.GetDateTime();
        results << L"Same time on 14th day of tenth month of next year" << std::endl <<
            L"In Local time zone: " << std::wstring_view(dateFormatter.Format(dateToFormat)) << std::endl;
        for (auto timeZone : timeZones)
        {
            GenerateResult(results, dateFormatter, timeZone, dateToFormat);
        }
        results << std::endl;

        // Display the results.
        OutputTextBlock().Text(results.str());
    }
}
