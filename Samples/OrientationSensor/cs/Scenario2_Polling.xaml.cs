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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Devices.Sensors;
using Windows.Foundation;
using System.Threading.Tasks;
using Windows.UI.Core;

namespace SDKTemplate
{
    public sealed partial class Scenario2_Polling : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private OrientationSensor _sensor;
        private uint _desiredReportInterval;
        private DispatcherTimer _dispatcherTimer;

        public Scenario2_Polling()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            _sensor = OrientationSensor.GetDefault(rootPage.SensorReadingType, rootPage.SensorOptimizationGoal);
            if (_sensor != null)
            {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                _desiredReportInterval = Math.Max(_sensor.MinimumReportInterval, 16);

                // Set up a DispatchTimer
                _dispatcherTimer = new DispatcherTimer();
                _dispatcherTimer.Tick += DisplayCurrentReading;
                _dispatcherTimer.Interval = TimeSpan.FromMilliseconds(_desiredReportInterval);

                rootPage.NotifyUser(rootPage.SensorDescription + " is ready", NotifyType.StatusMessage);
                ScenarioEnableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser(rootPage.SensorDescription + " not found", NotifyType.ErrorMessage);
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
                    _dispatcherTimer.Start();
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _dispatcherTimer.Stop();
                }
            }
        }

        /// <summary>
        /// This is the dispatcher callback.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void DisplayCurrentReading(object sender, object args)
        {
            OrientationSensorReading reading = _sensor.GetCurrentReading();
            if (reading != null)
            {
                MainPage.SetReadingText(ScenarioOutput, reading);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        private void ScenarioEnable()
        {
            // Set the report interval to enable the sensor for polling
            _sensor.ReportInterval = _desiredReportInterval;

            Window.Current.VisibilityChanged += VisibilityChanged;
            _dispatcherTimer.Start();

            ScenarioEnableButton.IsEnabled = false;
            ScenarioDisableButton.IsEnabled = true;
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        private void ScenarioDisable()
        {
            Window.Current.VisibilityChanged -= VisibilityChanged;

            // Stop the dispatcher
            _dispatcherTimer.Stop();
            
            // Restore the default report interval to release resources while the sensor is not in use
            _sensor.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
