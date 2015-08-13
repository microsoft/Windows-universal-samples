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
#include "Scenario1_LongAndShortFormats.xaml.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

Scenario1_LongAndShortFormats::Scenario1_LongAndShortFormats()
{
    InitializeComponent();
}

void Scenario1_LongAndShortFormats::Display()
{
    // This scenario uses the Windows.Globalization.DateTimeFormatting.DateTimeFormatter class
    // in order to display dates and times using basic formatters.

    //Get the current application context language
    String^ currentLanguage = ApplicationLanguages::Languages->GetAt(0);

    // Formatters for dates and times
    Array<DateTimeFormatter^> ^dateFormatters = {
        ref new DateTimeFormatter("shortdate"),
        ref new DateTimeFormatter("longdate"),
        ref new DateTimeFormatter("shorttime"),
        ref new DateTimeFormatter("longtime")
    };

    // Obtain the date that will be formatted.
    Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
    Windows::Foundation::DateTime dateToFormat = cal->GetDateTime();
    
    // Keep the results here
    String^ results = "";
    results = results + "Current application context language: " + currentLanguage + "\n\n";

    // Generate the results.
    for (auto formatter : dateFormatters)
    {
        // Perform the actual formatting. 
        results = results + formatter->Template + ": " + formatter->Format(dateToFormat) + "\n";
    }

    // Display the results.
    OutputTextBlock->Text = results;
}
