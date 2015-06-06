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
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using SDKTemplate;

namespace ActivitySensorCS
{
    public sealed partial class Scenario1_CurrentActivity : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

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

            try
            {
                // Get the default sensor
                var activitySensor = await ActivitySensor.GetDefaultAsync();

                if (null != activitySensor)
                {
                    // Get the current activity reading
                    var reading = await activitySensor.GetCurrentReadingAsync();
                    ScenarioOutput_Activity.Text = reading.Activity.ToString();
                    ScenarioOutput_Confidence.Text = reading.Confidence.ToString();
                    ScenarioOutput_Timestamp.Text = reading.Timestamp.ToString("u");
                }
                else
                {
                    rootPage.NotifyUser("No activity sensor found", NotifyType.ErrorMessage);
                }
            }
            catch (UnauthorizedAccessException)
            {
                rootPage.NotifyUser("User has denied access to the activity sensor", NotifyType.ErrorMessage);
            }
        }
    }
}
