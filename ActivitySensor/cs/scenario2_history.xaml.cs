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

using System;
using System.Threading.Tasks;
using Windows.Devices.Sensors;
using Windows.Foundation;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using SDKTemplate;

namespace ActivitySensorCS
{
    public sealed partial class Scenario2_History : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Scenario2_History()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Get Activity History' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ScenarioGetActivityHistory(object sender, RoutedEventArgs e)
        {
            // Reset fields and status
            ScenarioOutput_Count.Text = "No data";
            ScenarioOutput_Activity1.Text = "No data";
            ScenarioOutput_Confidence1.Text = "No data";
            ScenarioOutput_Timestamp1.Text = "No data";
            ScenarioOutput_ActivityN.Text = "No data";
            ScenarioOutput_ConfidenceN.Text = "No data";
            ScenarioOutput_TimestampN.Text = "No data";
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            var calendar = new Calendar();
            calendar.SetToNow();
            calendar.AddDays(-1);
            var yesterday = calendar.GetDateTime();

            // Get history from yesterday onwards
            var history = await ActivitySensor.GetSystemHistoryAsync(yesterday);

            ScenarioOutput_Count.Text = history.Count.ToString();
            if (history.Count > 0)
            {
                var reading1 = history[0];
                ScenarioOutput_Activity1.Text = reading1.Activity.ToString();
                ScenarioOutput_Confidence1.Text = reading1.Confidence.ToString();
                ScenarioOutput_Timestamp1.Text = reading1.Timestamp.ToString("u");

                var readingN = history[history.Count - 1];
                ScenarioOutput_ActivityN.Text = readingN.Activity.ToString();
                ScenarioOutput_ConfidenceN.Text = readingN.Confidence.ToString();
                ScenarioOutput_TimestampN.Text = readingN.Timestamp.ToString("u");
            }
        }
    }
}
