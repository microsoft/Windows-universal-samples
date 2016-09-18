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

using Windows.Devices.Sensors;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario3_Calibration : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        CalibrationBar calibrationBar = new CalibrationBar();

        public Scenario3_Calibration()
        {
            this.InitializeComponent();

            if (rootPage.SensorReadingType == SensorReadingType.Relative)
            {
                EnabledContent.Visibility = Visibility.Collapsed;
                DisabledContent.Visibility = Visibility.Visible;
            }
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            calibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for high accuracy. Acceptable accuracy met, so
        /// hide the calibration bar.
        /// </summary>
        private void OnHighAccuracy()
        {
            calibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for approximate accuracy. Acceptable accuracy met, so
        /// hide the calibration bar.
        /// </summary>
        private void OnApproximateAccuracy()
        {
            calibrationBar.Hide();
        }

        /// <summary>
        /// This is the click handler for unreliable accuracy. Sensor does not meet accuracy
        /// requirements. Request to show the calibration bar per the calibration guidance.
        /// </summary>
        private void OnUnreliableAccuracy()
        {
            calibrationBar.RequestCalibration(MagnetometerAccuracy.Unreliable);
        }
    }
}
