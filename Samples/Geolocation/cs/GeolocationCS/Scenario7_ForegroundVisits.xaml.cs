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
    public sealed partial class Scenario7 : Page
    {
        // Proides access to visits
        private GeovisitMonitor _visitMonitor = null;

        // A pointer to the main page
        private MainPage _rootPage = MainPage.Current;

        public Scenario7()
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
            StartMonitoringButton.IsEnabled = true;
            StopMonitoringButton.IsEnabled = false;
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
            _visitMonitor?.Stop();
        }

        /// <summary>
        /// This is the click handler for the 'StartMonitoring' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void StartMonitoring(object sender, RoutedEventArgs e)
        {
            // Request permission to access location
            var accessStatus = await Geolocator.RequestAccessAsync();

            switch (accessStatus)
            {
                case GeolocationAccessStatus.Allowed:

                    _visitMonitor = new GeovisitMonitor();

                    // Add visit state changed event handler.
                    _visitMonitor.VisitStateChanged += OnVisitStateChanged;

                    // Start monitoring with the desired scope.
                    // For higher granularity such as building/venue level changes, choose venue.
                    // For lower granularity more or less in the range of zipcode level changes, choose city. 
                    // Choosing Venue here as an example.
                    _visitMonitor.Start(VisitMonitoringScope.Venue);

                    LocationDisabledMessage.Visibility = Visibility.Collapsed;
                    _rootPage.NotifyUser("Waiting for update...", NotifyType.StatusMessage);

                    StartMonitoringButton.IsEnabled = false;
                    StopMonitoringButton.IsEnabled = true;
                    break;

                case GeolocationAccessStatus.Denied:
                    _rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);
                    LocationDisabledMessage.Visibility = Visibility.Visible;
                    break;

                default:
                case GeolocationAccessStatus.Unspecified:
                    _rootPage.NotifyUser("Unspecificed error!", NotifyType.ErrorMessage);
                    LocationDisabledMessage.Visibility = Visibility.Collapsed;
                    break;
            }
        }

        /// <summary>
        /// This is the click handler for the 'StopMonitoring' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StopMonitoring(object sender, RoutedEventArgs e)
        {
            // Stop visit monitoring.
            _visitMonitor.Stop();
            _visitMonitor = null;

            StartMonitoringButton.IsEnabled = true;
            StopMonitoringButton.IsEnabled = false;

            // Clear status
            _rootPage.NotifyUser("", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Event handler for Visitstatechanged events. It is raised when
        /// a new visit state change is available based on the monitoring scope specified.
        /// </summary>
        /// <param name="sender">Geovisitmonitor instance</param>
        /// <param name="args">visitstatechanged args data</param>
        async private void OnVisitStateChanged(GeovisitMonitor sender, GeovisitStateChangedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                _rootPage.NotifyUser("Visit state changed.", NotifyType.StatusMessage);
                UpdateVisitData(args.Visit);
            });
        }

        /// <summary>
        /// Updates the user interface with the Geovisit data provided
        /// </summary>
        /// <param name="visit">Geovisit to display its details</param>
        private void UpdateVisitData(Geovisit visit)
        {
            // A valid visit is available, extract the state change and Timestamp.
            ScenarioOutput_VisitStateChange.Text = visit.StateChange.ToString();
            ScenarioOutput_Timestamp.Text = visit.Timestamp.ToString();

            // If a valid position is available, extract the position information that caused the state change to happen.
            if (visit.Position == null)
            {
                ScenarioOutput_Latitude.Text   = "No data";
                ScenarioOutput_Longitude.Text  = "No data";
                ScenarioOutput_Accuracy.Text   = "No data";
            }
            else
            {
                ScenarioOutput_Latitude.Text    = visit.Position.Coordinate.Point.Position.Latitude.ToString();
                ScenarioOutput_Longitude.Text   = visit.Position.Coordinate.Point.Position.Longitude.ToString();
                ScenarioOutput_Accuracy.Text    = visit.Position.Coordinate.Accuracy.ToString();
            }
        }
    }
}
