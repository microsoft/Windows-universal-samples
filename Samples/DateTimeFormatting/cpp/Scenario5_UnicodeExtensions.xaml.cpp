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
#include "Scenario5_UnicodeExtensions.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario5_UnicodeExtensions::Scenario5_UnicodeExtensions()
{
    InitializeComponent();
}

void Scenario5_UnicodeExtensions::Display()
{
    // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
    // to format a date/time by using a formatter that uses unicode extenstion in the specified
    // language name

    // Get the current application context language
    String^ currentLanguage = ApplicationLanguages::Languages->GetAt(0);

    // Keep the results here
    String^ results = "";
    results += "Current application context language: " + currentLanguage + "\n\n";
    
    // Formatters for dates and times
    Array<DateTimeFormatter^> ^dateFormatters = {
        // Default application context  
        ref new DateTimeFormatter("longdate longtime"),

        // Telugu language, Gregorian Calendar and Latin Numeral System
        ref new DateTimeFormatter("longdate longtime", ref new Vector<String^>({ "te-in-u-ca-gregory-nu-latn", "en-US" })),

        // Hebrew language and Arabic Numeral System - calendar NOT specified in constructor
        ref new DateTimeFormatter(YearFormat::Default, 
            MonthFormat::Default, 
            DayFormat::Default, 
            DayOfWeekFormat::Default,
            HourFormat::Default,
            MinuteFormat::Default,
            SecondFormat::Default,
            ref new Vector<String^>({ "he-IL-u-nu-arab", "en-US" })),
        // Hebrew language and calendar - calendar specified in constructor
        // also, which overrides the one specified in Unicode extension
        ref new DateTimeFormatter(YearFormat::Default, 
            MonthFormat::Default, 
            DayFormat::Default, 
            DayOfWeekFormat::Default,
            HourFormat::Default,
            MinuteFormat::Default,
            SecondFormat::Default,
            ref new Vector<String^>({ "he-IL-u-ca-hebrew-co-phonebk", "en-US" }),
            "US",
            CalendarIdentifiers::Gregorian,
            ClockIdentifiers::TwentyFourHour), 
    };

    // Obtain the date that will be formatted.
    Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
    Windows::Foundation::DateTime dateToFormat = cal->GetDateTime();

    // Format and display date/time along with other relevant properites
    for (auto formatter : dateFormatters)
    {
        // Perform the actual formatting. 
        results += "Result: " + formatter->Format(dateToFormat) + "\n";

        String^ languages = "";
        for (auto language : formatter->Languages)
        {
            if (!languages->IsEmpty())
            {
                languages += ", ";
            }
            languages += language;
        }

        results += "    Language List: " + languages + "\n" +
                   "    Template: " + formatter->Template + "\n" +
                   "    Resolved Language: " + formatter->ResolvedLanguage + "\n" +
                   "    Calendar System: " + formatter->Calendar + "\n" +
                   "    Numeral System: " + formatter->NumeralSystem + "\n\n";
    }

    // Display the results.
    OutputTextBlock->Text = results;
}
