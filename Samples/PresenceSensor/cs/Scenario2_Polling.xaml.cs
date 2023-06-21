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
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Polling : Page
    {
        MainPage rootPage = MainPage.Current;

        private HumanPresenceSensor sensor;

        public Scenario2_Polling()
        {
            InitializeComponent();
        }

        private async void ScenarioGetSensor_Click(object sender, RoutedEventArgs e)
        {
            // This scenario uses DeviceInformation.FindAllAsync to find all
            // HumanPresenceSensor devices. This allows the app to choose a sensor
            // which best fits its requirements.
            //
            // Scenario 1 shows how to get the default HumanPresenceSensor device.

            // Disable the button so the user can't click it again while we are already working.
            GetSensorButton.IsEnabled = false;

            // Clear any previous message.
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            string selector = HumanPresenceSensor.GetDeviceSelector();
            DeviceInformationCollection deviceInfos = await DeviceInformation.FindAllAsync(selector);
            foreach (DeviceInformation deviceInfo in deviceInfos)
            {
                HumanPresenceSensor foundSensor = null;
                try
                {
                    foundSensor = await HumanPresenceSensor.FromIdAsync(deviceInfo.Id);
                }
                catch (UnauthorizedAccessException)
                {
                    // The user denied this app presence sensor access.
                    foundSensor = null;
                }

                if (foundSensor != null)
                {
                    if (null != foundSensor.MaxDetectableDistanceInMillimeters)
                    {
                        // Check if this is the sensor that matches our ranges.
                        // TODO: Customize these values to your application's needs.
                        const uint distanceInMillimetersLValue = 200; // 0.2m
                        const uint distanceInMillimetersRValue = 1200; // 1.2m
                        if (foundSensor.MinDetectableDistanceInMillimeters <= distanceInMillimetersLValue &&
                            foundSensor.MaxDetectableDistanceInMillimeters >= distanceInMillimetersRValue)
                        {
                            rootPage.NotifyUser("Found a presence sensor that meets the detection range", NotifyType.StatusMessage);
                            sensor = foundSensor;
                        }
                        else
                        {
                            rootPage.NotifyUser("Presence sensor does not meet the detection range, using it anyway", NotifyType.StatusMessage);
                            // We'll use the sensor anyway, to demonstrate how events work.
                            // Your app may decide not to use this sensor and keep looking for another.
                            sensor = foundSensor;
                        }
                    }
                    else
                    {
                        rootPage.NotifyUser("Presence sensor does not report detection ranges, using it anyway", NotifyType.StatusMessage);
                        sensor = foundSensor;
                    }
                }

                if (sensor != null)
                {
                    // We found a sensor we like. We can stop looking now.
                    break;
                }
            }

            GetSensorButton.IsEnabled = true;

            if (sensor != null)
            {
                VisualStateManager.GoToState(this, "Ready", true);
            }
            else
            {
                rootPage.NotifyUser("No presence sensor found", NotifyType.ErrorMessage);
            }
        }

        private void ScenarioGetData_Click(object sender, RoutedEventArgs e)
        {
            HumanPresenceSensorReading reading;
            try
            {
                reading = sensor.GetCurrentReading();
            }
            catch (UnauthorizedAccessException)
            {
                // The user denied this app presence sensor access.
                reading = null;
            }

            if (reading != null)
            {
                if (sensor.IsEngagementSupported)
                {
                    EngagementStateTextBlock.Text = reading.Engagement.ToString();
                }
                else
                {
                    EngagementStateTextBlock.Text = "Not Supported";
                }

                if (sensor.IsPresenceSupported)
                {
                    DetectionStateTextBlock.Text = reading.Presence.ToString();
                }
                else
                {
                    DetectionStateTextBlock.Text = "Not Supported";
                }

                // Show the detection distance, if available
                DetectionDistanceTextBlock.Text = reading.DistanceInMillimeters == null ? "Unavailable" : reading.DistanceInMillimeters.ToString();

                TimestampTextBlock.Text = reading.Timestamp.ToString("u");
            }
        }
    }
}

