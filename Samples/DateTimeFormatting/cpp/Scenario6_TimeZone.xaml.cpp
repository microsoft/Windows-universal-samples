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
#include "Scenario6_TimeZone.xaml.h"

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

Scenario6_TimeZone::Scenario6_TimeZone()
{
    InitializeComponent();
}

void Scenario6_TimeZone::Display()
{
    // This scenario illustrates time zone support in DateTimeFormatter class
    
    // Displayed time zones (other than local time zone)
    auto timeZones = ref new Vector<String^>({ "UTC", "America/New_York", "Asia/Kolkata" });
    
    // Keep the results here
    String^ results = "";
    
    // Create formatter object using longdate and longtime template
    DateTimeFormatter^ dateFormatter = ref new DateTimeFormatter("longdate longtime");
    
    // Create date/time to format and display.
    Windows::Globalization::Calendar^ calendar = ref new Windows::Globalization::Calendar();
    Windows::Foundation::DateTime dateToFormat = calendar->GetDateTime();    
    
    // Show current time.
    results += "Current date and time\n" +
        "In Local time zone: " + dateFormatter->Format(dateToFormat) + "\n";
    for (auto timeZone : timeZones)
    {
        results += "In " + timeZone + " time zone: " + dateFormatter->Format(dateToFormat, timeZone) + "\n";
    }
    results += "\n";
    
    // Show a time on 14th day of second month of next year.
    // Note the effect of daylight saving on the results.
    calendar->AddYears(1); calendar->Month = 2; calendar->Day = 14;
    dateToFormat = calendar->GetDateTime();
    results += "Same time on 14th day of second month of next year\n" +
        "In Local time zone: " + dateFormatter->Format(dateToFormat) + "\n";
    for (auto timeZone : timeZones)
    {
        results += "In " + timeZone + " time zone: " + dateFormatter->Format(dateToFormat, timeZone) + "\n";
    }
    results += "\n";
    
    // Show a time on 14th day of tenth month of next year.
    // Note the effect of daylight saving on the results.
    calendar->AddMonths(8);
    dateToFormat = calendar->GetDateTime();
    results += "Same time on 14th day of tenth month of next year\n" +
        "In Local time zone: " + dateFormatter->Format(dateToFormat) + "\n";
    for (auto timeZone : timeZones)
    {
        results += "In " + timeZone + " time zone: " + dateFormatter->Format(dateToFormat, timeZone) + "\n";
    }
    results += "\n";

    // Display the results.
    OutputTextBlock->Text = results;
}
