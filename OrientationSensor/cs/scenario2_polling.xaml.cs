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

namespace OrientationCS
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

            _sensor = OrientationSensor.GetDefault();
            if (_sensor != null)
            {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                uint minReportInterval = _sensor.MinimumReportInterval;
                _desiredReportInterval = minReportInterval > 16 ? minReportInterval : 16;

                // Set up a DispatchTimer
                _dispatcherTimer = new DispatcherTimer();
                _dispatcherTimer.Tick += DisplayCurrentReading;
                _dispatcherTimer.Interval = new TimeSpan(0, 0, 0, 0, (int)_desiredReportInterval);
            }
            else
            {
                rootPage.NotifyUser("No orientation sensor found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting Cancel.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (ScenarioDisableButton.IsEnabled)
            {
                Window.Current.VisibilityChanged -= new WindowVisibilityChangedEventHandler(VisibilityChanged);

                // Stop the dispatcher
                _dispatcherTimer.Stop();

                // Restore the default report interval to release resources while the sensor is not in use
                _sensor.ReportInterval = 0;
            }

            base.OnNavigatingFrom(e);
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
                // Quaternion values
                SensorQuaternion quaternion = reading.Quaternion;   // get a reference to the object to avoid re-creating it for each access
                ScenarioOutput_X.Text = String.Format("{0,8:0.00000}", quaternion.X);
                ScenarioOutput_Y.Text = String.Format("{0,8:0.00000}", quaternion.Y);
                ScenarioOutput_Z.Text = String.Format("{0,8:0.00000}", quaternion.Z);
                ScenarioOutput_W.Text = String.Format("{0,8:0.00000}", quaternion.W);

                // Rotation Matrix values
                SensorRotationMatrix rotationMatrix = reading.RotationMatrix;
                ScenarioOutput_M11.Text = String.Format("{0,8:0.00000}", rotationMatrix.M11);
                ScenarioOutput_M12.Text = String.Format("{0,8:0.00000}", rotationMatrix.M12);
                ScenarioOutput_M13.Text = String.Format("{0,8:0.00000}", rotationMatrix.M13);
                ScenarioOutput_M21.Text = String.Format("{0,8:0.00000}", rotationMatrix.M21);
                ScenarioOutput_M22.Text = String.Format("{0,8:0.00000}", rotationMatrix.M22);
                ScenarioOutput_M23.Text = String.Format("{0,8:0.00000}", rotationMatrix.M23);
                ScenarioOutput_M31.Text = String.Format("{0,8:0.00000}", rotationMatrix.M31);
                ScenarioOutput_M32.Text = String.Format("{0,8:0.00000}", rotationMatrix.M32);
                ScenarioOutput_M33.Text = String.Format("{0,8:0.00000}", rotationMatrix.M33);

                // Yaw accuracy
                switch (reading.YawAccuracy)
                {
                    case MagnetometerAccuracy.Unknown:
                        ScenarioOutput_YawAccuracy.Text = "Unknown";
                        break;
                    case MagnetometerAccuracy.Unreliable:
                        ScenarioOutput_YawAccuracy.Text = "Unreliable";
                        break;
                    case MagnetometerAccuracy.Approximate:
                        ScenarioOutput_YawAccuracy.Text = "Approximate";
                        break;
                    case MagnetometerAccuracy.High:
                        ScenarioOutput_YawAccuracy.Text = "High";
                        break;
                    default:
                        ScenarioOutput_YawAccuracy.Text = "No data";
                        break;
                }
            }
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_sensor != null)
            {
                // Set the report interval to enable the sensor for polling
                _sensor.ReportInterval = _desiredReportInterval;

                Window.Current.VisibilityChanged += new WindowVisibilityChangedEventHandler(VisibilityChanged);
                _dispatcherTimer.Start();

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No orientation sensor found", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// This is the click handler for the 'Disable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioDisable(object sender, RoutedEventArgs e)
        {
            Window.Current.VisibilityChanged -= new WindowVisibilityChangedEventHandler(VisibilityChanged);

            // Stop the dispatcher
            _dispatcherTimer.Stop();
            
            // Restore the default report interval to release resources while the sensor is not in use
            _sensor.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
