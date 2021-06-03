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
using System.Threading;
using System.Threading.Tasks;
using Windows.Devices.Geolocation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_GetPosition : Page
    {
        private CancellationTokenSource _cts = null;

        // A pointer to the main page
        private MainPage _rootPage = MainPage.Current;

        public Scenario2_GetPosition()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (_cts != null)
            {
                _cts.Cancel();
                _cts = null;
            }
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
                        uint desiredAccuracyInMetersValue;
                        uint.TryParse(DesiredAccuracyInMeters.Text, out desiredAccuracyInMetersValue);
                        Geolocator geolocator = new Geolocator { DesiredAccuracyInMeters = desiredAccuracyInMetersValue };

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
                ShowSatelliteData(false);
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
                    ShowSatelliteData(true);
                }
                else
                {
                    // Hide labels and satellite data
                    ShowSatelliteData(false);
                }
            }
        }

        private void ShowSatelliteData(bool isVisible)
        {
            var visibility = isVisible ? Visibility.Visible : Visibility.Collapsed;
            ScenarioOutput_PosPrecision.Visibility = visibility;
            ScenarioOutput_HorzPrecision.Visibility = visibility;
            ScenarioOutput_VertPrecision.Visibility = visibility;
        }
    }
}
