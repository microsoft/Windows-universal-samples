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
#include "Scenario3_ParameterizedTemplate.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario3_ParameterizedTemplate::Scenario3_ParameterizedTemplate()
{
    InitializeComponent();
}

void Scenario3_ParameterizedTemplate::Display()
{
    // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
    // to format a date/time by specifying a template via parameters.  Note that the current application language 
    // language and region value will determine the pattern of the date returned based on the 
    // specified parts.

    //Get the current application context language
    String^ currentLanguage = ApplicationLanguages::Languages->GetAt(0);

    // Formatters for dates
    Array<DateTimeFormatter^>^ dateFormatters = {
        ref new DateTimeFormatter(
            YearFormat::Full, 
            MonthFormat::Abbreviated, 
            DayFormat::Default, 
            DayOfWeekFormat::Abbreviated),

        ref new DateTimeFormatter(
            YearFormat::Abbreviated, 
            MonthFormat::Abbreviated, 
            DayFormat::Default, 
            DayOfWeekFormat::None),

        ref new DateTimeFormatter(
            YearFormat::Full, 
            MonthFormat::Full, 
            DayFormat::None, 
            DayOfWeekFormat::None),

        ref new DateTimeFormatter(
            YearFormat::None, 
            MonthFormat::Full, 
            DayFormat::Default, 
            DayOfWeekFormat::None),
    };

    // Formatters for times
    Array<DateTimeFormatter^>^ timeFormatters = {
        ref new DateTimeFormatter(
            HourFormat::Default,
            MinuteFormat::Default,
            SecondFormat::Default),

        ref new DateTimeFormatter(
            HourFormat::Default,
            MinuteFormat::Default,
            SecondFormat::None),

        ref new DateTimeFormatter(
            HourFormat::Default,
            MinuteFormat::None,
            SecondFormat::None),
    };
    
    // Obtain the date that will be formatted.
    Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
    Windows::Foundation::DateTime dateToFormat = cal->GetDateTime();
    
    // Keep the results here
    String^ results = "";
    results = results + "Current application context language: " + currentLanguage + "\n\n";
    results = results + "Dates:\n";

    // Generate the results.
    for (auto formatter : dateFormatters)
    {
        // Perform the actual formatting. 
        results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
    }

    results = results + "\n";
    results = results + "Times:\n";

    // Generate the results.
    for (auto formatter : timeFormatters)
    {
        // Perform the actual formatting. 
        results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
    }

    // Display the results.
    OutputTextBlock->Text = results;
}
