
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
using Windows.ApplicationModel.ExtendedExecution;
using Windows.Devices.Geolocation;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Page containing the Extended Execution LocationTracking Reason scenario functions.
    /// </summary>
    public sealed partial class LocationTrackingReason : Page
    {
        private MainPage rootPage = MainPage.Current;

        ExtendedExecutionSession session = null;
        private Timer periodicTimer = null;

        public LocationTrackingReason()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // End the Extended Execution Session.
            // Only one extended execution session can be held by an application at one time.
            ClearExtendedExecution();
        }

        void ClearExtendedExecution()
        {
            if (session != null)
            {
                session.Revoked -= SessionRevoked;
                session.Dispose();
                session = null;
            }

            if (periodicTimer != null)
            {
                periodicTimer.Dispose();
                periodicTimer = null;
            }
        }

        private void UpdateUI()
        {
            if (session == null)
            {
                Status.Text = "Not requested";
                RequestButton.IsEnabled = true;
                CloseButton.IsEnabled = false;
            }
            else
            {
                Status.Text = "Requested";
                RequestButton.IsEnabled = false;
                CloseButton.IsEnabled = true;
            }
        }

        private async Task<Geolocator> StartLocationTrackingAsync()
        {
            Geolocator geolocator = null;

            // Request permission to access location
            var accessStatus = await Geolocator.RequestAccessAsync();

            switch (accessStatus)
            {
                case GeolocationAccessStatus.Allowed:
                    // See the Geolocation sample for more information on using the Geolocator class.
                    geolocator = new Geolocator { ReportInterval = 2000 };
                    break;

                case GeolocationAccessStatus.Denied:
                    rootPage.NotifyUser("Access to location is denied.", NotifyType.ErrorMessage);
                    break;

                default:
                case GeolocationAccessStatus.Unspecified:
                    rootPage.NotifyUser("Couldn't access the geolocator.", NotifyType.ErrorMessage);
                    break;
            }

            return geolocator;
        }

        private async void BeginExtendedExecution()
        {
            // The previous Extended Execution must be closed before a new one can be requested.
            // This code is redundant here because the sample doesn't allow a new extended
            // execution to begin until the previous one ends, but we leave it here for illustration.
            ClearExtendedExecution();

            var newSession = new ExtendedExecutionSession();
            newSession.Reason = ExtendedExecutionReason.LocationTracking;
            newSession.Description = "Tracking your location";
            newSession.Revoked += SessionRevoked;
            ExtendedExecutionResult result = await newSession.RequestExtensionAsync();

            switch (result)
            {
                case ExtendedExecutionResult.Allowed:
                    rootPage.NotifyUser("Extended execution allowed.", NotifyType.StatusMessage);
                    session = newSession;
                    Geolocator geolocator = await StartLocationTrackingAsync();
                    periodicTimer = new Timer(OnTimer, geolocator, TimeSpan.FromSeconds(2), TimeSpan.FromSeconds(10));
                    break;

                default:
                case ExtendedExecutionResult.Denied:
                    rootPage.NotifyUser("Extended execution denied.", NotifyType.ErrorMessage);
                    newSession.Dispose();
                    break;
            }
            UpdateUI();
        }

        private async void OnTimer(object state)
        {
            var geolocator = (Geolocator)state;
            string message;
            if (geolocator == null)
            {
                message = "No geolocator";
            }
            else
            {
                Geoposition geoposition = await geolocator.GetGeopositionAsync();
                if (geoposition == null)
                {
                    message = "Cannot get current location";
                }
                else
                {
                    BasicGeoposition basicPosition = geoposition.Coordinate.Point.Position;
                    message = $"Longitude = {basicPosition.Longitude}, Latitude = {basicPosition.Latitude}";
                }
            }
            MainPage.DisplayToast(message);
        }

        private void EndExtendedExecution()
        {
            ClearExtendedExecution();
            UpdateUI();
        }

        private async void SessionRevoked(object sender, ExtendedExecutionRevokedEventArgs args)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                switch (args.Reason)
                {
                    case ExtendedExecutionRevokedReason.Resumed:
                        rootPage.NotifyUser("Extended execution revoked due to returning to foreground.", NotifyType.StatusMessage);
                        break;

                    case ExtendedExecutionRevokedReason.SystemPolicy:
                        rootPage.NotifyUser("Extended execution revoked due to system policy.", NotifyType.StatusMessage);
                        break;
                }

                EndExtendedExecution();
            });
        }
    }
}
