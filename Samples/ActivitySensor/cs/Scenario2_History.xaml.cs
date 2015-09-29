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
using Windows.Devices.Enumeration;
using Windows.Devices.Sensors;
using Windows.Globalization;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario2_History : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        // Common class ID for activity sensors
        Guid ActivitySensorClassId = new Guid("9D9E0118-1807-4F2E-96E4-2CE57142E196");

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

            // Determine if we can access activity sensors
            var deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(ActivitySensorClassId);
            if (deviceAccessInfo.CurrentStatus == DeviceAccessStatus.Allowed)
            {
                // Determine if an activity sensor is present
                // This can also be done using Windows::Devices::Enumeration::DeviceInformation::FindAllAsync
                var activitySensor = await ActivitySensor.GetDefaultAsync();
                if (activitySensor != null)
                {
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
                else
                {
                    rootPage.NotifyUser("No activity sensors found", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access to activity sensors is denied", NotifyType.ErrorMessage);
            }
        }
    }
}
