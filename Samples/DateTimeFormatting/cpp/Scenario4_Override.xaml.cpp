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
#include "Scenario4_Override.xaml.h"

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

Scenario4_Override::Scenario4_Override()
{
    InitializeComponent();
}

void Scenario4_Override::Display()
{
    // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
    // to format a date/time by using a formatter that provides specific languages, 
    // geographic region, calendar and clock

    //Get the current default application context language
    String^ currentLanguage = ApplicationLanguages::Languages->GetAt(0);

    // Array languages used by the test
    auto ja = ref new Vector<String^>({ "ja-JP" });
    auto fr = ref new Vector<String^>({ "fr-FR" });
    auto de = ref new Vector<String^>({ "de-DE" });

    // Formatters for dates and times
    Array<DateTimeFormatter^> ^dateFormatters = {
        ref new DateTimeFormatter("longdate", ja, "JP", CalendarIdentifiers::Japanese, ClockIdentifiers::TwelveHour),

        ref new DateTimeFormatter("month day", fr, "FR", CalendarIdentifiers::Gregorian, ClockIdentifiers::TwentyFourHour),
        
        ref new DateTimeFormatter(
            YearFormat::Abbreviated, 
            MonthFormat::Abbreviated, 
            DayFormat::Default, 
            DayOfWeekFormat::None, 
            HourFormat::None, 
            MinuteFormat::None, 
            SecondFormat::None, 
            de, "DE", CalendarIdentifiers::Gregorian, ClockIdentifiers::TwelveHour),

        ref new DateTimeFormatter("longtime", ja, "JP", CalendarIdentifiers::Japanese, ClockIdentifiers::TwelveHour),

        ref new DateTimeFormatter("hour minute", fr, "FR", CalendarIdentifiers::Gregorian, ClockIdentifiers::TwentyFourHour),
        
        ref new DateTimeFormatter(
            YearFormat::None, 
            MonthFormat::None, 
            DayFormat::None, 
            DayOfWeekFormat::None,
            HourFormat::Default, 
            MinuteFormat::Default, 
            SecondFormat::None, 
            de, "DE", CalendarIdentifiers::Gregorian, ClockIdentifiers::TwelveHour)
    };

    // Obtain the date that will be formatted.
    Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
    Windows::Foundation::DateTime dateToFormat = cal->GetDateTime();
    
    // Keep the results here
    String^ results = "";
    results = results + "Current default application context language: " + currentLanguage + "\n\n";

    // Generate the results.
    for (auto formatter : dateFormatters)
    {
        // Perform the actual formatting. 
        results = results + formatter->Template + ": (" + formatter->ResolvedLanguage + ") " + formatter->Format(dateToFormat) + "\n";
    }

    // Display the results.
    OutputTextBlock->Text = results;
}
