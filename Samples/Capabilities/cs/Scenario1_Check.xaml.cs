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
using Windows.Devices.Geolocation;
using Windows.Foundation;
using Windows.Security.Authorization.AppCapabilityAccess;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Check : Page
    {
        private MainPage rootPage = MainPage.Current;
        AppCapability locationCapability = null;

        public Scenario1_Check()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Specify which capability we want to query/monitor.
            locationCapability = AppCapability.Create("location");

            // Register a handler to be called when access changes.
            locationCapability.AccessChanged += OnCapabilityAccessChanged;

            // Update UI to match current access.
            UpdateCapabilityStatus();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            locationCapability.AccessChanged -= OnCapabilityAccessChanged;
        }

        async void OnCapabilityAccessChanged(AppCapability sender, object e)
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => UpdateCapabilityStatus());
        }

        async void StreamLocationButton_Click(object sender, RoutedEventArgs e)
        {
            switch (locationCapability.CheckAccess())
            {
                case AppCapabilityAccessStatus.Allowed:
                    // Access was already granted.
                    // AccessChanged event will trigger a recalc.
                    await StreamLocationAsync();
                    break;

                case AppCapabilityAccessStatus.UserPromptRequired:
                    // Request access directly for a better experience than going to Settings.
                    // This is equivalent to calling Geolocator::RequestAccessAsync().
                    if (await locationCapability.RequestAccessAsync() == AppCapabilityAccessStatus.Allowed)
                    {
                        // The user granted access.
                        // AccessChanged event will trigger a recalc.
                        await StreamLocationAsync();
                    }
                    break;

                case AppCapabilityAccessStatus.DeniedByUser:
                case AppCapabilityAccessStatus.DeniedBySystem:
                default:
                    // Send user to Settings to obtain location permission
                    // or explain why access is denied.
                    await Launcher.LaunchUriAsync(new Uri("ms-settings:privacy-location"));
                    break;
            }
        }

        async Task StreamLocationAsync()
        {
            if (await Geolocator.RequestAccessAsync() == GeolocationAccessStatus.Allowed)
            {
                // Need try/catch because we can lose geolocator access at any time.
                try
                {
                    // Specify a message to explain to the user why we are tracking their location.
                    locationCapability.DisplayMessage = "Streaming location";

                    LocationTextBlock.Text = "Finding your current location...";
                    Geolocator geolocator = new Geolocator();

                    for (int i = 0; i < 4; i++)
                    {
                        Geoposition pos = await geolocator.GetGeopositionAsync();

                        if (pos == null)
                        {
                            LocationTextBlock.Text = "Current location unknown.";
                        }
                        else
                        {
                            LocationTextBlock.Text = $"Your location is: Latitude {pos.Coordinate.Point.Position.Latitude:F}, Longitude {pos.Coordinate.Point.Position.Longitude:F}";
                        }

                        await Task.Delay(500);

                        // You can change the message as your app's usage changes.
                        locationCapability.DisplayMessage = "Tracking your current location...";
                    }
                }
                catch (Exception ex) when (ex.HResult == unchecked((int)0x80070005)) // E_ACCESSDENIED
                {
                    // Lost access in the middle of the operation.
                    // AccessChanged event will trigger a recalc.
                }

                LocationTextBlock.Text = "";

                // Even though we clear the message immediately, it will take time before the message
                // disappear from the system. The system has a minimum display time for the message,
                // so that users can observe brief usage.
                locationCapability.DisplayMessage = "";
            }
            else
            {
                // Lost access before operation started.
                // AccessChanged event will trigger a recalc.
            }
        }

        void UpdateCapabilityStatus()
        {
            AppCapabilityAccessStatus status = locationCapability.CheckAccess();
            if (status == AppCapabilityAccessStatus.Allowed)
            {
                LocationAccessBlock.Visibility = Visibility.Collapsed;
                LocationTextBlock.Visibility = Visibility.Visible;
            }
            else
            {
                LocationTextBlock.Visibility = Visibility.Collapsed;

                switch (status)
                {
                    case AppCapabilityAccessStatus.NotDeclaredByApp:
                        // The app neglected to declare the capability in its manifest.
                        // This is a developer error.
                        LocationAccessBlock.Text = "App misconfiguration error. Contact vendor for support.";
                        LocationAccessBlock.Visibility = Visibility.Visible;
                        break;

                    default:
                    case AppCapabilityAccessStatus.DeniedBySystem:
                        // We can send the user to the Settings page to obtain access
                        // or at least explain why access is denied.
                        LocationAccessBlock.Text = "The system has blocked access to location.";
                        LocationAccessBlock.Visibility = Visibility.Visible;
                        break;

                    case AppCapabilityAccessStatus.DeniedByUser:
                        // We can send the user to the Settings page to obtain access.
                        LocationAccessBlock.Text = "You must enable location access in Settings.";
                        LocationAccessBlock.Visibility = Visibility.Visible;
                        break;

                    case AppCapabilityAccessStatus.UserPromptRequired:
                        // We can prompt the user to give us access.
                        LocationAccessBlock.Visibility = Visibility.Collapsed;
                        break;
                }
            }
        }
    }
}