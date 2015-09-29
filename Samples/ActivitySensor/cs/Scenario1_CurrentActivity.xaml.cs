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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_CurrentActivity : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        // Common class ID for activity sensors
        Guid ActivitySensorClassId = new Guid("9D9E0118-1807-4F2E-96E4-2CE57142E196");

        public Scenario1_CurrentActivity()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Get Current Activity' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ScenarioGetCurrentActivity(object sender, RoutedEventArgs e)
        {
            // Reset fields and status
            ScenarioOutput_Activity.Text = "No data";
            ScenarioOutput_Confidence.Text = "No data";
            ScenarioOutput_Timestamp.Text = "No data";
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Determine if we can access activity sensors
            var deviceAccessInfo = DeviceAccessInformation.CreateFromDeviceClassId(ActivitySensorClassId);
            if (deviceAccessInfo.CurrentStatus == DeviceAccessStatus.Allowed)
            {
                // Get the default sensor
                var activitySensor = await ActivitySensor.GetDefaultAsync();
                if (activitySensor != null)
                {
                    // Get the current activity reading
                    var reading = await activitySensor.GetCurrentReadingAsync();
                    if (reading != null)
                    {
                        ScenarioOutput_Activity.Text = reading.Activity.ToString();
                        ScenarioOutput_Confidence.Text = reading.Confidence.ToString();
                        ScenarioOutput_Timestamp.Text = reading.Timestamp.ToString("u");
                    }
                }
                else
                {
                    rootPage.NotifyUser("No activity sensor found", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("Access denied to activity sensors", NotifyType.ErrorMessage);
            }
        }
    }
}
