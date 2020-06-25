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
#include "Scenario1_Prefs.h"
#include "Scenario1_Prefs.g.cpp"
#include "SampleConfiguration.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::System::UserProfile;
using namespace Windows::UI::Xaml;

namespace winrt::SDKTemplate::implementation
{
    Scenario1_Prefs::Scenario1_Prefs()
    {
        InitializeComponent();
    }

    void Scenario1_Prefs::ShowResults_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
        // obtain the user's globalization preferences.
        OutputTextBlock().Text(L"Languages: " + StringJoin(L", ", GlobalizationPreferences::Languages()) + L"\n" +
            L"Home Region: " + GlobalizationPreferences::HomeGeographicRegion() + L"\n" +
            L"Calendar System: " + StringJoin(L", ", GlobalizationPreferences::Calendars()) + L"\n" +
            L"Clock: " + StringJoin(L", ", GlobalizationPreferences::Clocks()) + L"\n" +
            L"First Day of the Week: " + to_hstring(GlobalizationPreferences::WeekStartsOn()));
    }
}
