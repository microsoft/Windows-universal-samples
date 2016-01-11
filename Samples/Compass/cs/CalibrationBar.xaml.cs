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
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using Windows.Devices.Sensors;

namespace SDKTemplate
{
    public sealed partial class CalibrationBar : Page
    {
        // Constants specifying when the calibration bar is shown:
        //   * suppress duration determines how long (in minutes) to wait before 
        //     showing the calibration bar again 
        //   * auto dismiss time determines when to auto dismiss the bar
        //     if it's shown too long
        private const double SUPPRESS_DURATION_APPROXIMATE_MIN = 10.0;
        private const double SUPPRESS_DURATION_UNRELIABLE_MIN = 5.0;
        private TimeSpan CALIBRATION_POPUP_AUTO_DIMSMISS_TIME = new TimeSpan(0, 0, 30); // 30 seconds

        private const double LAYOUT_WIDTH_768 = 768.0;

        private DateTime _lastCalibrationTime = new DateTime(0);
        private bool _barDismissed = false;
        private DispatcherTimer _calibrationTimer;

        public CalibrationBar()
        {
            this.InitializeComponent();
            _calibrationTimer = new DispatcherTimer();
            _calibrationTimer.Tick += OnAutoDismissTimerExpired;
            _calibrationTimer.Interval = CALIBRATION_POPUP_AUTO_DIMSMISS_TIME;
        }

        /// <summary>
        /// Called when the application would like to show the calibration bar to the user 
        /// to improve sensor accuracy.
        /// </summary>
        /// <param name="accuracy">Current accuracy of the sensor.</param>
        public void RequestCalibration(MagnetometerAccuracy accuracy)
        {
            // Only show the calibration bar if it is hidden and sufficient
            // time has passed since it was last hidden.
            if (!CalibrationPopup.IsOpen && CanShowCalibrationBar(accuracy))
            {
                InvalidateLayout();
                Window.Current.SizeChanged += OnWindowSizeChanged;
                HideGuidance(); // Calibration image hidden by default
                CalibrationPopup.IsOpen = true;
                _calibrationTimer.Start();
            }
        }

        /// <summary>
        /// Called when sensor accuracy is sufficient and the application would like to 
        /// hide the calibration bar.
        /// </summary>
        public void Hide()
        {
            if (CalibrationPopup.IsOpen)
            {
                // Hide the calibration bar and save the current time.
                _lastCalibrationTime = DateTime.Now;
                HideGuidance();
                CalibrationPopup.IsOpen = false;
                Window.Current.SizeChanged -= OnWindowSizeChanged;
                _calibrationTimer.Stop();
            }
        }

        /// <summary>
        /// Invoked when the window size is updated.
        /// </summary>
        /// <param name="sender">Instance that triggered the event.</param>
        /// <param name="e">Event data describing the conditions that led to the event.</param>
        void OnWindowSizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            InvalidateLayout();
        }

        /// <summary>
        /// Updates the calibration bar layout based on the current window width.
        /// </summary>
        private void InvalidateLayout()
        {
            CalibrationGrid.Width = Window.Current.Bounds.Width;

            if (CalibrationGrid.Width < LAYOUT_WIDTH_768)
            {
                Grid.SetRow(DismissPopup, 1);
                Grid.SetColumn(DismissPopup, 1);

                CalibrationGridTop.Margin = new Thickness(0, 24, 20, 15);
                DismissPopup.Margin = new Thickness(66, 15, 0, 0);
            }
            else
            {
                Grid.SetRow(DismissPopup, 0);
                Grid.SetColumn(DismissPopup, 2);

                CalibrationGridTop.Margin = new Thickness(0, 19, 66, 19);
                DismissPopup.Margin = new Thickness(66, 0, 0, 0);
            }
        }

        /// <summary>
        /// Called to determine if sufficient time has passed since the calibration bar
        /// was hidden to show the calibration bar again. This suppression time is
        /// dependent on the current sensor accuracy.
        /// </summary>
        /// <param name="accuracy">Current accuracy of the sensor.</param>
        private bool CanShowCalibrationBar(MagnetometerAccuracy accuracy)
        {
            bool showBar = false;
            if (!_barDismissed)
            {
                if ((accuracy == MagnetometerAccuracy.Approximate) &&
                    (DateTime.Now.CompareTo(_lastCalibrationTime.AddMinutes(SUPPRESS_DURATION_APPROXIMATE_MIN)) >= 0))
                {
                    showBar = true;
                }
                else if ((accuracy == MagnetometerAccuracy.Unreliable) &&
                    (DateTime.Now.CompareTo(_lastCalibrationTime.AddMinutes(SUPPRESS_DURATION_UNRELIABLE_MIN)) >= 0))
                {
                    showBar = true;
                }
            }
            return showBar;
        }

        /// <summary>
        /// This is the click handler for the show guidance button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ShowGuidanceButton_Click(object sender, RoutedEventArgs e)
        {
            if (Guidance.Visibility == Windows.UI.Xaml.Visibility.Collapsed)
            {
                ShowGuidance();
            }
        }

        /// <summary>
        /// This is the click handler for the hide guidance button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void HideGuidanceButton_Click(object sender, RoutedEventArgs e)
        {
            if (Guidance.Visibility == Windows.UI.Xaml.Visibility.Visible)
            {
                HideGuidance();
            }
        }

        /// <summary>
        /// This is the click handler for the dismiss popup button. It hides the 
        /// calibration bar and prevents it from being shown again for the lifetime
        /// of the app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DismissPopup_Click(object sender, RoutedEventArgs e)
        {
            // Don't show the calibration bar again.
            _barDismissed = true;
            Hide();
        }

        /// <summary>
        /// Expands the calibration bar to show the calibration image to the user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ShowGuidance()
        {
            ShowGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            HideGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Visible;
            Guidance.Visibility = Windows.UI.Xaml.Visibility.Visible;
        }

        /// <summary>
        /// Collapses the calibration bar to hide the calibration image from the user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void HideGuidance()
        {
            HideGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            ShowGuidanceButton.Visibility = Windows.UI.Xaml.Visibility.Visible;
            Guidance.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
        }

        /// <summary>
        /// This is the handler for the auto dismiss timer. After the calibraiton bar
        /// has been shown for the maximum time, it is automatically hidden.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnAutoDismissTimerExpired(object sender, object e)
        {
            Hide();
        }
    }
}
