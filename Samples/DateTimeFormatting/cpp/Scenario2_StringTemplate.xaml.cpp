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
#include "Scenario2_StringTemplate.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario2_StringTemplate::Scenario2_StringTemplate()
{
    InitializeComponent();
}

void Scenario2_StringTemplate::Display()
{
    // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
    // to format a date/time via a string template.  Note that the order specifed in the string pattern does 
    // not determine the order of the parts of the formatted string.  The current application language and region value will
    // determine the pattern of the date returned based on the specified parts.

    //Get the current application context language
    String^ currentLanguage = ApplicationLanguages::Languages->GetAt(0);

    // Formatters for dates.
    Array<DateTimeFormatter^>^ dateFormatters = {
        ref new DateTimeFormatter("month day"),
        ref new DateTimeFormatter("month year"),
        ref new DateTimeFormatter("month day year"),
        ref new DateTimeFormatter("month day dayofweek year"),
        ref new DateTimeFormatter("dayofweek.abbreviated"),
        ref new DateTimeFormatter("month.abbreviated"),
        ref new DateTimeFormatter("year.abbreviated")
    };

    // Formatters for time.
    Array<DateTimeFormatter^>^ timeFormatters = {
        ref new DateTimeFormatter("hour minute"),
        ref new DateTimeFormatter("hour minute second"),
        ref new DateTimeFormatter("hour")
    };

    // Formatters for timezone.
    Array<DateTimeFormatter^>^ timeZoneFormatters = {
        ref new DateTimeFormatter("timezone"),
        ref new DateTimeFormatter("timezone.full"),
        ref new DateTimeFormatter("timezone.abbreviated")
    };
                
    // Formatters for combinations.
    Array<DateTimeFormatter^>^ combinationFormatters = {
        ref new DateTimeFormatter("hour minute second timezone.full"),
        ref new DateTimeFormatter("day month year hour minute timezone"),
        ref new DateTimeFormatter("dayofweek day month year hour minute second"),
        ref new DateTimeFormatter("dayofweek.abbreviated day month hour minute"),
        ref new DateTimeFormatter("dayofweek day month year hour minute second timezone.abbreviated")
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

    results = results + "\n";
    results = results + "Time zones:\n";
        
    // Generate the results.
    for (auto formatter : timeZoneFormatters)
    {
        // Perform the actual formatting. 
        results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
    }

    results = results + "\n";
    results = results + "Date and time combinations:\n";
        
    // Generate the results.
    for (auto formatter : combinationFormatters)
    {
        // Perform the actual formatting. 
        results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
    }

    // Display the results.
    OutputTextBlock->Text = results;
}
