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
    public sealed partial class Scenario1_DataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Accelerometer _accelerometer;

        public Scenario1_DataEvents()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _accelerometer = Accelerometer.GetDefault(rootPage.AccelerometerReadingType);
            if (_accelerometer != null)
            {
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
                    _accelerometer.ReadingChanged += ReadingChanged;
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _accelerometer.ReadingChanged -= ReadingChanged;
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
            // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
            _accelerometer.ReportInterval = Math.Max(_accelerometer.MinimumReportInterval, 16);

            Window.Current.VisibilityChanged += VisibilityChanged;
            _accelerometer.ReadingChanged += ReadingChanged;

            ScenarioEnableButton.IsEnabled = false;
            ScenarioDisableButton.IsEnabled = true;
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        private void ScenarioDisable()
        {
            Window.Current.VisibilityChanged -= VisibilityChanged;
            _accelerometer.ReadingChanged -= ReadingChanged;

            // Restore the default report interval to release resources while the sensor is not in use
            _accelerometer.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
