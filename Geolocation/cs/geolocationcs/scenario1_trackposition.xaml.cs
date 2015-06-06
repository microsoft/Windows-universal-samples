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
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace GeolocationCS
{
    public sealed partial class Scenario1 : Page
    {
        // Proides access to location data
        private Geolocator _geolocator = null;

        // A pointer to the main page
        private MainPage _rootPage = MainPage.Current;

        public Scenario1()
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
            StartTrackingButton.IsEnabled = true;
            StopTrackingButton.IsEnabled = false;
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
            if (StopTrackingButton.IsEnabled)
            {
                _geolocator.PositionChanged -= OnPositionChanged;
                _geolocator.StatusChanged -= OnStatusChanged;
            }

            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// This is the click handler for the 'StartTracking' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void StartTracking(object sender, RoutedEventArgs e)
        {
            // Request permission to access location
            var accessStatus = await Geolocator.RequestAccessAsync();

            switch (accessStatus)
            {
                case GeolocationAccessStatus.Allowed:
                    // You should set MovementThreshold for distance-based tracking
                    // or ReportInterval for periodic-based tracking before adding event
                    // handlers. If none is set, a ReportInterval of 1 second is used
                    // as a default and a position will be returned every 1 second.
                    //
                    // Value of 2000 milliseconds (2 seconds) 
                    // isn't a requirement, it is just an example.
                    _geolocator = new Geolocator { ReportInterval = 2000 };

                    // Subscribe to PositionChanged event to get updated tracking positions
                    _geolocator.PositionChanged += OnPositionChanged;

                    // Subscribe to StatusChanged event to get updates of location status changes
                    _geolocator.StatusChanged += OnStatusChanged;
                    
                    _rootPage.NotifyUser("Waiting for update...", NotifyType.StatusMessage);
                    LocationDisabledMessage.Visibility = Visibility.Collapsed;
                    StartTrackingButton.IsEnabled = false;
                    StopTrackingButton.IsEnabled = true;
                    break;

                case GeolocationAccessStatus.Denied:
                    _rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);
                    LocationDisabledMessage.Visibility = Visibility.Visible;
                    break;

                case GeolocationAccessStatus.Unspecified:
                    _rootPage.NotifyUser("Unspecificed error!", NotifyType.ErrorMessage);
                    LocationDisabledMessage.Visibility = Visibility.Collapsed;
                    break;
            }
        }

        /// <summary>
        /// This is the click handler for the 'StopTracking' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StopTracking(object sender, RoutedEventArgs e)
        {
            _geolocator.PositionChanged -= OnPositionChanged;
            _geolocator.StatusChanged -= OnStatusChanged;
            _geolocator = null;

            StartTrackingButton.IsEnabled = true;
            StopTrackingButton.IsEnabled = false;

            // Clear status
            _rootPage.NotifyUser("", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event handler for PositionChanged events. It is raised when
        /// a location is available for the tracking session specified.
        /// </summary>
        /// <param name="sender">Geolocator instance</param>
        /// <param name="e">Position data</param>
        async private void OnPositionChanged(Geolocator sender, PositionChangedEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                _rootPage.NotifyUser("Location updated.", NotifyType.StatusMessage);
                UpdateLocationData(e.Position);
            });
        }

        /// <summary>
        /// Event handler for StatusChanged events. It is raised when the 
        /// location status in the system changes.
        /// </summary>
        /// <param name="sender">Geolocator instance</param>
        /// <param name="e">Statu data</param>
        async private void OnStatusChanged(Geolocator sender, StatusChangedEventArgs e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                // Show the location setting message only if status is disabled.
                LocationDisabledMessage.Visibility = Visibility.Collapsed;

                switch (e.Status)
                {
                    case PositionStatus.Ready:
                        // Location platform is providing valid data.
                        ScenarioOutput_Status.Text = "Ready";
                        _rootPage.NotifyUser("Location platform is ready.", NotifyType.StatusMessage);
                        break;

                    case PositionStatus.Initializing:
                        // Location platform is attempting to acquire a fix. 
                        ScenarioOutput_Status.Text = "Initializing";
                        _rootPage.NotifyUser("Location platform is attempting to obtain a position.", NotifyType.StatusMessage);
                        break;

                    case PositionStatus.NoData:
                        // Location platform could not obtain location data.
                        ScenarioOutput_Status.Text = "No data";
                        _rootPage.NotifyUser("Not able to determine the location.", NotifyType.ErrorMessage);
                        break;

                    case PositionStatus.Disabled:
                        // The permission to access location data is denied by the user or other policies.
                        ScenarioOutput_Status.Text = "Disabled";
                        _rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);

                        // Show message to the user to go to location settings
                        LocationDisabledMessage.Visibility = Visibility.Visible;

                        // Clear cached location data if any
                        UpdateLocationData(null);
                        break;

                    case PositionStatus.NotInitialized:
                        // The location platform is not initialized. This indicates that the application 
                        // has not made a request for location data.
                        ScenarioOutput_Status.Text = "Not initialized";
                        _rootPage.NotifyUser("No request for location is made yet.", NotifyType.StatusMessage);
                        break;

                    case PositionStatus.NotAvailable:
                        // The location platform is not available on this version of the OS.
                        ScenarioOutput_Status.Text = "Not available";
                        _rootPage.NotifyUser("Location is not available on this version of the OS.", NotifyType.ErrorMessage);
                        break;

                    default:
                        ScenarioOutput_Status.Text = "Unknown";
                        _rootPage.NotifyUser(string.Empty, NotifyType.StatusMessage);
                        break;
                }
            });
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
            }
            else
            {
                ScenarioOutput_Latitude.Text = position.Coordinate.Point.Position.Latitude.ToString();
                ScenarioOutput_Longitude.Text = position.Coordinate.Point.Position.Longitude.ToString();
                ScenarioOutput_Accuracy.Text = position.Coordinate.Accuracy.ToString();
            }
        }
    }
}
