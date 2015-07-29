//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
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
using Windows.Storage;

namespace ApplicationDataSample
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Msappdata : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;

        public Msappdata()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            ApplicationData appData = ApplicationData.Current;

            try
            {
                StorageFile sourceFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///assets/appDataLocal.png"));
                await sourceFile.CopyAsync(appData.LocalFolder);
            }
            catch (Exception)
            {
                // If the image has already been copied the CopyAsync() method above will fail.
                // Ignore this error.
            }

            try
            {
                StorageFile sourceFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///assets/appDataRoaming.png"));
                await sourceFile.CopyAsync(appData.RoamingFolder);
            }
            catch (Exception)
            {
                // If the image has already been copied the CopyAsync() method above will fail.
                // Ignore this error.
            }

            try
            {
                StorageFile sourceFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///assets/appDataTemp.png"));
                await sourceFile.CopyAsync(appData.TemporaryFolder);
            }
            catch (Exception)
            {
                // If the image has already been copied the CopyAsync() method above will fail.
                // Ignore this error.
            }

            LocalImage.Source = new Windows.UI.Xaml.Media.Imaging.BitmapImage(new Uri("ms-appdata:///local/appDataLocal.png"));
            RoamingImage.Source = new Windows.UI.Xaml.Media.Imaging.BitmapImage(new Uri("ms-appdata:///roaming/appDataRoaming.png"));
            TempImage.Source = new Windows.UI.Xaml.Media.Imaging.BitmapImage(new Uri("ms-appdata:///temp/appDataTemp.png"));
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            LocalImage.Source = null;
            RoamingImage.Source = null;
            TempImage.Source = null;
        }
    }
}
