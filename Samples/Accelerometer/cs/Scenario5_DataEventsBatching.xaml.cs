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
using Windows.Devices.Sensors;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario5_DataEventsBatching : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Accelerometer accelerometer;
        private uint desiredReportInterval;
        private uint desiredReportLatency;


        public Scenario5_DataEventsBatching()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            accelerometer = Accelerometer.GetDefault(rootPage.AccelerometerReadingType);
            if (accelerometer != null)
            {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                desiredReportInterval = Math.Max(accelerometer.MinimumReportInterval, 16);

                // MaxBatchSize will be 0 if the accelerometer does not support batching.
                uint maxLatency = accelerometer.MaxBatchSize * desiredReportInterval;
                desiredReportLatency = Math.Min(maxLatency, 10000);

                rootPage.NotifyUser(rootPage.AccelerometerReadingType + " accelerometer ready", NotifyType.StatusMessage);
                ScenarioEnableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser(rootPage.AccelerometerReadingType + " accelerometer not found", NotifyType.ErrorMessage);
            }
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                ScenarioDisable();
            }
        }

        /// <summary>
        /// This is the event handler for VisibilityChanged events. You would register for these notifications
        /// if handling sensor data when the app is not visible could cause unintended actions in the app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">
        /// Event data that can be examined for the current visibility state.
        /// </param>
        private void VisibilityChanged(object sender, VisibilityChangedEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                if (e.Visible)
                {
                    // Re-enable sensor input (no need to restore the desired reportInterval... it is restored for us upon app resume)
                    accelerometer.ReadingChanged += ReadingChanged;
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    accelerometer.ReadingChanged -= ReadingChanged;
                }
            }
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ReadingChanged(object sender, AccelerometerReadingChangedEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                MainPage.SetReadingText(ScenarioOutput, e.Reading);
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        private void ScenarioEnable()
        {
            // Establish the report interval
            accelerometer.ReportInterval = desiredReportInterval;

            // Establish the report latency. This is a no-op if the accelerometer does not support batching
            accelerometer.ReportLatency = desiredReportLatency;
            Window.Current.VisibilityChanged += VisibilityChanged;
            accelerometer.ReadingChanged += ReadingChanged;

            ScenarioEnableButton.IsEnabled = false;
            ScenarioDisableButton.IsEnabled = true;
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        private void ScenarioDisable()
        {
            Window.Current.VisibilityChanged -= VisibilityChanged;
            accelerometer.ReadingChanged -= ReadingChanged;

            // Restore the default report interval to release resources while the sensor is not in use
            accelerometer.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
