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

namespace SDKTemplate
{
    public sealed partial class Scenario1_DataEvents : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        private Compass _compass;
        private uint _desiredReportInterval;

        public Scenario1_DataEvents()
        {
            this.InitializeComponent();

            _compass = Compass.GetDefault();
            if (_compass != null)
            {
                // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
                // This value will be used later to activate the sensor.
                uint minReportInterval = _compass.MinimumReportInterval;
                _desiredReportInterval = minReportInterval > 16 ? minReportInterval : 16;
            }
            else
            {
                rootPage.NotifyUser("No compass found", NotifyType.ErrorMessage);
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
                _compass.ReadingChanged -= new TypedEventHandler<Compass, CompassReadingChangedEventArgs>(ReadingChanged);

                // Restore the default report interval to release resources while the sensor is not in use
                _compass.ReportInterval = 0;
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
                    _compass.ReadingChanged += new TypedEventHandler<Compass, CompassReadingChangedEventArgs>(ReadingChanged);
                }
                else
                {
                    // Disable sensor input (no need to restore the default reportInterval... resources will be released upon app suspension)
                    _compass.ReadingChanged -= new TypedEventHandler<Compass, CompassReadingChangedEventArgs>(ReadingChanged);
                }
            }
        }

        /// <summary>
        /// This is the event handler for ReadingChanged events.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void ReadingChanged(object sender, CompassReadingChangedEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                CompassReading reading = e.Reading;
                ScenarioOutput_MagneticNorth.Text = String.Format("{0,5:0.00}", reading.HeadingMagneticNorth);
                if (reading.HeadingTrueNorth != null)
                {
                    ScenarioOutput_TrueNorth.Text = String.Format("{0,5:0.00}", reading.HeadingTrueNorth);
                }
                else
                {
                    ScenarioOutput_TrueNorth.Text = "No data";
                }
                switch (reading.HeadingAccuracy)
                {
                    case MagnetometerAccuracy.Unknown:
                        ScenarioOutput_HeadingAccuracy.Text = "Unknown";
                        break;
                    case MagnetometerAccuracy.Unreliable:
                        ScenarioOutput_HeadingAccuracy.Text = "Unreliable";
                        break;
                    case MagnetometerAccuracy.Approximate:
                        ScenarioOutput_HeadingAccuracy.Text = "Approximate";
                        break;
                    case MagnetometerAccuracy.High:
                        ScenarioOutput_HeadingAccuracy.Text = "High";
                        break;
                    default:
                        ScenarioOutput_HeadingAccuracy.Text = "No data";
                        break;
                }
            });
        }

        /// <summary>
        /// This is the click handler for the 'Enable' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ScenarioEnable(object sender, RoutedEventArgs e)
        {
            if (_compass != null)
            {
                // Establish the report interval
                _compass.ReportInterval = _desiredReportInterval;

                Window.Current.VisibilityChanged += new WindowVisibilityChangedEventHandler(VisibilityChanged);
                _compass.ReadingChanged += new TypedEventHandler<Compass, CompassReadingChangedEventArgs>(ReadingChanged);

                ScenarioEnableButton.IsEnabled = false;
                ScenarioDisableButton.IsEnabled = true;
            }
            else
            {
                rootPage.NotifyUser("No compass found", NotifyType.ErrorMessage);
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
            _compass.ReadingChanged -= new TypedEventHandler<Compass, CompassReadingChangedEventArgs>(ReadingChanged);

            // Restore the default report interval to release resources while the sensor is not in use
            _compass.ReportInterval = 0;

            ScenarioEnableButton.IsEnabled = true;
            ScenarioDisableButton.IsEnabled = false;
        }
    }
}
