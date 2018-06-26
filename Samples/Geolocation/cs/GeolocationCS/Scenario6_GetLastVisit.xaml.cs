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
using System.Threading.Tasks;
using Windows.Devices.Geolocation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace GeolocationCS
{
    public sealed partial class Scenario6 : Page
    {
        // A pointer to the main page
        private MainPage _rootPage = MainPage.Current;

        public Scenario6()
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
            GetLastVisitButton.IsEnabled = true;
        }

        /// <summary>
        /// This is the click handler for the 'getLastVisit' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void GetLastVisitButtonClicked(object sender, RoutedEventArgs e)
        {
            GetLastVisitButton.IsEnabled = false;
            LocationDisabledMessage.Visibility = Visibility.Collapsed;

            // Request permission to access location
            var accessStatus = await Geolocator.RequestAccessAsync();

            Geovisit visit = null;
            switch (accessStatus)
            {
                case GeolocationAccessStatus.Allowed:
                    _rootPage.NotifyUser("Waiting for update...", NotifyType.StatusMessage);

                    // Get the last visit report, if any. This should be a quick operation.
                    visit = await GeovisitMonitor.GetLastReportAsync();
                    _rootPage.NotifyUser("Visit info updated.", NotifyType.StatusMessage);
                    break;

                case GeolocationAccessStatus.Denied:
                    _rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);
                    LocationDisabledMessage.Visibility = Visibility.Visible;
                    break;

                default:
                case GeolocationAccessStatus.Unspecified:
                    _rootPage.NotifyUser("Unspecified error.", NotifyType.ErrorMessage);
                    break;
            }

            UpdateLastVisit(visit);

            GetLastVisitButton.IsEnabled = true;
        }

        /// <summary>
        /// Updates the user interface with the Geovisit data provided
        /// </summary>
        /// <param name="visit">Geovisit to display its details</param>
        private void UpdateLastVisit(Geovisit visit)
        {
            if (visit == null)
            {
                ScenarioOutput_Latitude.Text = "No data";
                ScenarioOutput_Longitude.Text = "No data";
                ScenarioOutput_Accuracy.Text = "No data";
                ScenarioOutput_Timestamp.Text = "No data";
                ScenarioOutput_VisitStateChange.Text = "No data";
            }
            else
            {
                // A valid visit is available, extract the state change and Timestamp.
                ScenarioOutput_VisitStateChange.Text = visit.StateChange.ToString();
                ScenarioOutput_Timestamp.Text = visit.Timestamp.ToString();

                // If a valid position is available, extract the position information that caused the state change to happen.
                if(visit.Position == null)
                {
                    ScenarioOutput_Latitude.Text = "No data";
                    ScenarioOutput_Longitude.Text = "No data";
                    ScenarioOutput_Accuracy.Text = "No data";
                }
                else
                {
                    ScenarioOutput_Latitude.Text = visit.Position.Coordinate.Point.Position.Latitude.ToString();
                    ScenarioOutput_Longitude.Text = visit.Position.Coordinate.Point.Position.Longitude.ToString();
                    ScenarioOutput_Accuracy.Text = visit.Position.Coordinate.Accuracy.ToString();
                }
            }
        }
    }
}
