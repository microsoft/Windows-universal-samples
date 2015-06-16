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

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Devices.Geolocation;

namespace GeotagCSSample
{
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        private void LogStatus(string message)
        {
            rootPage.NotifyUser(message, NotifyType.StatusMessage);
        }

        private void LogError(string message)
        {
            rootPage.NotifyUser(message, NotifyType.ErrorMessage);
        }

        // Apps need to call RequestAccessAsync to get user permission on location usage,
        // otherwise SetGeotagFromGeolocator will fail. Also RequestAccessAsync needs 
        // to be called from a UI thread.
        private async Task RequestLocationAccessAsync()
        {
            try
            {
                GeolocationAccessStatus status = await Geolocator.RequestAccessAsync();
                if (status != GeolocationAccessStatus.Allowed)
                {
                    LogError("Location access is NOT allowed");
                }
            }
            catch (Exception e)
            {
                LogError("Exception: " + e.Message);
            }
        }

        private async Task GetGeotagAsync()
        {
            try
            {
                StorageFile file = await KnownFolders.PicturesLibrary.GetFileAsync(filename.Text);
                Geopoint geopoint = await GeotagHelper.GetGeotagAsync(file);
                if (geopoint != null)
                {
                    LogStatus("GetGeotagAsync complete -" + " latitude: " + geopoint.Position.Latitude + " longitude: " + geopoint.Position.Longitude);
                }
                else
                {
                    LogStatus("GetGeotagAsync complete - location info not available");
                }

            }
            catch (Exception e)
            {
                LogError("Exception: " + e.Message);
            }
        }

        private async Task SetGeotagFromGeolocatorAsync()
        {
            try
            {
                StorageFile file = await KnownFolders.PicturesLibrary.GetFileAsync(filename.Text);

                Geolocator geolocator = new Geolocator();
                geolocator.DesiredAccuracy = PositionAccuracy.High;

                await GeotagHelper.SetGeotagFromGeolocatorAsync(file, geolocator);
                LogStatus("SetGeotagFromGeolocatorAsync complete");

            }
            catch (Exception e)
            {
                LogError("Exception: " + e.Message);
            }
        }

        private async Task SetGeotagAsync()
        {
            try
            {
                StorageFile file = await KnownFolders.PicturesLibrary.GetFileAsync(filename.Text);

                BasicGeoposition position = new BasicGeoposition();
                position.Latitude = 10.0; // Use latitude 10.0, longitude 20.0 as an example
                position.Longitude = 20.0;
                position.Altitude = 0.0;
                Geopoint geopoint = new Geopoint(position);

                await GeotagHelper.SetGeotagAsync(file, geopoint);
                LogStatus("SetGeotagAsync complete");
            }
            catch (Exception e)
            {
                LogError("Exception: " + e.Message);
            }
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;
            await RequestLocationAccessAsync();
        }

        private async void Button_Click_GetGeotag(object sender, RoutedEventArgs e)
        {
            await GetGeotagAsync();
        }

        private async void Button_Click_SetGeotagFromGeolocator(object sender, RoutedEventArgs e)
        {
            await SetGeotagFromGeolocatorAsync();
        }

        private async void Button_Click_SetGeotag(object sender, RoutedEventArgs e)
        {
            await SetGeotagAsync();
        }
    }
}
