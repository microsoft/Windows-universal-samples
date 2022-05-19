//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using System;
using Windows.Storage;

namespace SDKTemplate
{
    public sealed partial class Scenario5_Msappdata : Page
    {
        public Scenario5_Msappdata()
        {
            InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            ApplicationData appData = ApplicationData.Current;

            StorageFile sourceFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///assets/appDataLocal.png"));
            try
            {
                await sourceFile.CopyAsync(appData.LocalFolder);
            }
            catch (Exception)
            {
                // If the image has already been copied the CopyAsync() method above will fail.
                // Ignore this error.
            }

            sourceFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///assets/appDataRoaming.png"));
            try
            {
                await sourceFile.CopyAsync(appData.RoamingFolder);
            }
            catch (Exception)
            {
                // If the image has already been copied the CopyAsync() method above will fail.
                // Ignore this error.
            }

            sourceFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///assets/appDataTemp.png"));
            try
            {                
                await sourceFile.CopyAsync(appData.TemporaryFolder);
            }
            catch (Exception)
            {
                // If the image has already been copied the CopyAsync() method above will fail.
                // Ignore this error.
            }

            LocalImage.Source = new BitmapImage(new Uri("ms-appdata:///local/appDataLocal.png"));
            RoamingImage.Source = new BitmapImage(new Uri("ms-appdata:///roaming/appDataRoaming.png"));
            TempImage.Source = new BitmapImage(new Uri("ms-appdata:///temp/appDataTemp.png"));
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            LocalImage.Source = null;
            RoamingImage.Source = null;
            TempImage.Source = null;
        }
    }
}
