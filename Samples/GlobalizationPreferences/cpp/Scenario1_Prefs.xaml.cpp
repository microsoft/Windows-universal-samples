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
#include "Scenario1_Prefs.xaml.h"
#include "SampleConfiguration.h"

using namespace SDKTemplate;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Globalization;
using namespace Windows::System::UserProfile;

Scenario1_Prefs::Scenario1_Prefs()
{
    InitializeComponent();
}

void Scenario1_Prefs::ShowResults()
{
    // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
    // obtain the user's globalization preferences.
    OutputTextBlock->Text = "Languages: " + StringJoin(", ", GlobalizationPreferences::Languages) + "\n" +
        "Home Region: " + GlobalizationPreferences::HomeGeographicRegion + "\n" +
        "Calendar System: " + StringJoin(", ", GlobalizationPreferences::Calendars) + "\n" +
        "Clock: " + StringJoin(", ", GlobalizationPreferences::Clocks) + "\n" +
        "First Day of the Week: " + GlobalizationPreferences::WeekStartsOn.ToString();
}