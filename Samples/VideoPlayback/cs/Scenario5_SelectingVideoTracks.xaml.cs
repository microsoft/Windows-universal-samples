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
using System.Collections.Generic;
using System;
using Windows.Media.Playback;
using Windows.Media.Core;

namespace VideoPlayback
{
    /// <summary>
    /// Demonstrates multi-track video playback and camera selection.
    /// </summary>
    public sealed partial class Scenario5 : Page
    {
        MainPage rootPage;
        MediaPlaybackItem item;

        public Scenario5()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            this.item = new MediaPlaybackItem(
                MediaSource.CreateFromUri(
                    new Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/multivideo-with-captions.mkv")));

            this.mainVideoElement.SetPlaybackSource(item);
        }

        private void TrackButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the video track index based on value of the 'tag' field
            var button = sender as Button;
            int videoTrackIndex = int.Parse(button.Tag as string);

            // Select the clicked camera track
            if (this.item != null && this.item.VideoTracks != null && this.item.VideoTracks.Count -1 >= videoTrackIndex)
            {
                this.item.VideoTracks.SelectedIndex = videoTrackIndex;
                rootPage.NotifyUser("Switched to video track #" + (videoTrackIndex + 1)
                    + " | Id: " + this.item.VideoTracks[videoTrackIndex].Id
                    + " | Label: " + this.item.VideoTracks[videoTrackIndex].Label, NotifyType.StatusMessage);
            }
        }
    }
}
