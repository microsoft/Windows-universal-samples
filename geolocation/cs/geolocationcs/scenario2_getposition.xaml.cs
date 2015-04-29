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

using SDKTemplate;
using System;
using System.Threading;
using System.Threading.Tasks;
using Windows.Devices.Geolocation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace GeolocationCS
{
    public sealed partial class Scenario2 : Page
    {
        // Captures the value entered by user.
        private uint _desireAccuracyInMetersValue = 0;
        private CancellationTokenSource _cts = null;

        // A pointer to the main page
        private MainPage _rootPage = MainPage.Current;

        public Scenario2()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            GetGeolocationButton.IsEnabled = true;
            CancelGetGeolocationButton.IsEnabled = false;
            DesiredAccuracyInMeters.IsEnabled = true;
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting e.Cancel to true.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            if (_cts != null)
            {
                _cts.Cancel();
                _cts = null;
            }

            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the click handler for the 'getGeolocation' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        async private void GetGeolocationButtonClicked(object sender, RoutedEventArgs e)
        {
            GetGeolocationButton.IsEnabled = false;
            CancelGetGeolocationButton.IsEnabled = true;
            LocationDisabledMessage.Visibility = Visibility.Collapsed;

            try
            {
                // Request permission to access location
                var accessStatus = await Geolocator.RequestAccessAsync();

                switch (accessStatus)
                {
                    case GeolocationAccessStatus.Allowed:

                        // Get cancellation token
                        _cts = new CancellationTokenSource();
                        CancellationToken token = _cts.Token;

                        _rootPage.NotifyUser("Waiting for update...", NotifyType.StatusMessage);

                        // If DesiredAccuracy or DesiredAccuracyInMeters are not set (or value is 0), DesiredAccuracy.Default is used.
                        Geolocator geolocator = new Geolocator { DesiredAccuracyInMeters = _desireAccuracyInMetersValue };
                        
                        // Carry out the operation
                        Geoposition pos = await geolocator.GetGeopositionAsync().AsTask(token);

                        UpdateLocationData(pos);
                        _rootPage.NotifyUser("Location updated.", NotifyType.StatusMessage);
                        break;

                    case GeolocationAccessStatus.Denied:
                        _rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);
                        LocationDisabledMessage.Visibility = Visibility.Visible;
                        UpdateLocationData(null);
                        break;

                    case GeolocationAccessStatus.Unspecified:
                        _rootPage.NotifyUser("Unspecified error.", NotifyType.ErrorMessage);
                        UpdateLocationData(null);
                        break;
                }
            }
            catch (TaskCanceledException)
            {
                _rootPage.NotifyUser("Canceled.", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                _rootPage.NotifyUser(ex.ToString(), NotifyType.ErrorMessage);
            }
            finally
            {
                _cts = null;
            }

            GetGeolocationButton.IsEnabled = true;
            CancelGetGeolocationButton.IsEnabled = false;
        }

        /// <summary>
        /// This is the click handler for the 'CancelGetGeolocation' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CancelGetGeolocationButtonClicked(object sender, RoutedEventArgs e)
        {
            if (_cts != null)
            {
                _cts.Cancel();
                _cts = null;
            }

            GetGeolocationButton.IsEnabled = true;
            CancelGetGeolocationButton.IsEnabled = false;
        }

        void DesiredAccuracyInMeters_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                // Update with the value entered by user
                _desireAccuracyInMetersValue = uint.Parse(DesiredAccuracyInMeters.Text);

                // Enable the button and clear out status message
                GetGeolocationButton.IsEnabled = true;
                _rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
            }
            catch (ArgumentNullException)
            {
                GetGeolocationButton.IsEnabled = false;
            }
            catch (FormatException)
            {
                _rootPage.NotifyUser("Desired Accuracy must be a number", NotifyType.ErrorMessage);
                GetGeolocationButton.IsEnabled = false;
            }
            catch (OverflowException)
            {
                _rootPage.NotifyUser("Desired Accuracy is out of bounds", NotifyType.ErrorMessage);
                GetGeolocationButton.IsEnabled = false;
            }
        }

        /// <summary>
        /// Updates the user interface with the Geoposition data provided
        /// </summary>
        /// <param name="position">Geoposition to display its details</param>
        private void UpdateLocationData(Geoposition position)
        {
            if (position == null)
            {
                ScenarioOutput_Latitude.Text = "No data";
                ScenarioOutput_Longitude.Text = "No data";
                ScenarioOutput_Accuracy.Text = "No data";
                ScenarioOutput_Source.Text = "No data";
                ShowSatalliteData(false);
            }
            else
            {
                ScenarioOutput_Latitude.Text = position.Coordinate.Point.Position.Latitude.ToString();
                ScenarioOutput_Longitude.Text = position.Coordinate.Point.Position.Longitude.ToString();
                ScenarioOutput_Accuracy.Text = position.Coordinate.Accuracy.ToString();
                ScenarioOutput_Source.Text = position.Coordinate.PositionSource.ToString();
                
                if (position.Coordinate.PositionSource == PositionSource.Satellite)
                {
                    // Show labels and satellite data when available
                    ScenarioOutput_PosPrecision.Text = position.Coordinate.SatelliteData.PositionDilutionOfPrecision.ToString();
                    ScenarioOutput_HorzPrecision.Text = position.Coordinate.SatelliteData.HorizontalDilutionOfPrecision.ToString();
                    ScenarioOutput_VertPrecision.Text = position.Coordinate.SatelliteData.VerticalDilutionOfPrecision.ToString();
                    ShowSatalliteData(true);
                }
                else
                {
                    // Hide labels and satellite data
                    ShowSatalliteData(false);
                }
            }
        }

        private void ShowSatalliteData(bool isVisible)
        {
            var visibility = isVisible ? Visibility.Visible : Visibility.Collapsed;
            ScenarioOutput_PosPrecision.Visibility = visibility;
            ScenarioOutput_HorzPrecision.Visibility = visibility;
            ScenarioOutput_VertPrecision.Visibility = visibility;
        }
    }
}
