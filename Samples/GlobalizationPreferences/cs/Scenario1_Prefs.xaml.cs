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

using Windows.Foundation;
using Windows.System.UserProfile;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Prefs : Page
    {
        public Scenario1_Prefs()
        {
            this.InitializeComponent();
        }

        private void ShowResults()
        {
            // This scenario uses the Windows.System.UserProfile.GlobalizationPreferences class to
            // obtain the user's globalization preferences.
            OutputTextBlock.Text = "Languages: " + string.Join(", ", GlobalizationPreferences.Languages) + "\n" +
                "Home Region: " + GlobalizationPreferences.HomeGeographicRegion + "\n" +
                "Calendar System: " + string.Join(", ", GlobalizationPreferences.Calendars) + "\n" +
                "Clock: " + string.Join(", ", GlobalizationPreferences.Clocks) + "\n" +
                "First Day of the Week: " + GlobalizationPreferences.WeekStartsOn.ToString();
        }
    }
}
