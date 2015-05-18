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
using Windows.Media.Core;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace VideoPlayback
{
    /// <summary>
    /// Demonstrates basic video playback using MediaElement.
    /// </summary>
    public sealed partial class Scenario1 : Page
    {
        private MainPage rootPage;

        public Scenario1()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Load example video on startup
            mediaElement.Source = new Uri("ms-appx:///Assets/Media/ElephantsDream-Clip-H264_SD-AAC_eng-AAC_spa-AAC_eng_commentary-SRT_eng-SRT_por-SRT_swe.mkv");
        }

        /// <summary>
        /// Handles the pick file button click event to load a video.
        /// </summary>
        private async void pickFileButton_Click(object sender, RoutedEventArgs e)
        {
            // Clear previous returned file name, if it exists, between iterations of this scenario 
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            // Create and open the file picker
            FileOpenPicker openPicker = new FileOpenPicker();
            openPicker.ViewMode = PickerViewMode.Thumbnail;
            openPicker.SuggestedStartLocation = PickerLocationId.VideosLibrary;
            openPicker.FileTypeFilter.Add(".mp4");
            openPicker.FileTypeFilter.Add(".mkv");
            openPicker.FileTypeFilter.Add(".avi");

            StorageFile file = await openPicker.PickSingleFileAsync();
            if (file != null)
            {
                rootPage.NotifyUser("Picked video: " + file.Name, NotifyType.StatusMessage);
                this.mediaElement.SetPlaybackSource(MediaSource.CreateFromStorageFile(file));
                this.mediaElement.Play();
            }
            else
            {
                rootPage.NotifyUser("Operation cancelled.", NotifyType.ErrorMessage);
            }
        }
    }
}
