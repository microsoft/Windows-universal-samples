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
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Geotag : Page
    {
        private MainPage rootPage = MainPage.Current;
        private StorageFile file = null;

        public Scenario1_Geotag()
        {
            this.InitializeComponent();
        }

        private async void ChooseFileButton_Click()
        {
            var picker = new FileOpenPicker();
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".jpeg");
            picker.FileTypeFilter.Add(".mp4");
            picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
            file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                FileDisplayName.Text = file.DisplayName;
                FileOperationsPanel.Visibility = Visibility.Visible;
            }
            else
            {
                FileOperationsPanel.Visibility = Visibility.Collapsed;
            }
        }

        private async void GetGeotagButton_Click()
        {
            Geopoint geopoint = await GeotagHelper.GetGeotagAsync(file);
            if (geopoint != null)
            {
                rootPage.NotifyUser($"Latitude = {geopoint.Position.Latitude}, Longitude = {geopoint.Position.Longitude}", NotifyType.StatusMessage);
            }
            else
            {
                // File I/O errors are converted to "No information".
                rootPage.NotifyUser("No location information available", NotifyType.ErrorMessage);
            }
        }

        private async void SetGeotagFromGeolocatorButton_Click()
        {
            // Call RequestAccessAsync from the UI thread to get user permission on location usage.
            // Otherwise SetGeotagFromGeolocator will fail.
            GeolocationAccessStatus status = await Geolocator.RequestAccessAsync();
            if (status != GeolocationAccessStatus.Allowed)
            {
                rootPage.NotifyUser("Location access is not allowed", NotifyType.ErrorMessage);
                return;
            }

            Geolocator geolocator = new Geolocator();
            geolocator.DesiredAccuracy = PositionAccuracy.High;

            try
            {
                await GeotagHelper.SetGeotagFromGeolocatorAsync(file, geolocator);
                rootPage.NotifyUser("Geolocation set to current location", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                // File I/O errors are reported as exceptions.
                // AccessDeniedExcetion can be raised if the user revoked location access
                // after RequestAccessAsync completed.
                rootPage.NotifyUser("Exception: " + ex.Message, NotifyType.ErrorMessage);
            }
        }

        private async void SetGeotagButton_Click()
        {
            // Set the approximate position of the observation deck of the Seattle Space Needle.
            BasicGeoposition position = new BasicGeoposition();
            position.Latitude = 47.620491;
            position.Longitude = -122.349319;
            position.Altitude = 158.12;
            Geopoint geopoint = new Geopoint(position);

            try
            {
                await GeotagHelper.SetGeotagAsync(file, geopoint);
                rootPage.NotifyUser("Geolocation set to Seattle Space Needle", NotifyType.StatusMessage);
            }
            catch (Exception ex)
            {
                // File I/O errors are reported as exceptions
                rootPage.NotifyUser("Exception: " + ex.Message, NotifyType.ErrorMessage);
            }
        }
    }
}
