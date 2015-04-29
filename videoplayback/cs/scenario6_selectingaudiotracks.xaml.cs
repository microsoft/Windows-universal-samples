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
using System.Collections.Generic;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace VideoPlayback
{
    /// <summary>
    /// Demonstrates multi-track audio playback and language selection.
    /// </summary>
    public sealed partial class Scenario6 : Page
    {
        MainPage rootPage;
        MediaPlaybackItem item;

        public Scenario6()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            this.item = new MediaPlaybackItem(
                MediaSource.CreateFromUri(
                    new Uri("ms-appx:///Assets/Media/ElephantsDream-Clip-H264_SD-AAC_eng-AAC_spa-AAC_eng_commentary-SRT_eng-SRT_por-SRT_swe.mkv")));

            this.mainVideoElement.SetPlaybackSource(item);
        }

        private void TrackButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the camera number based on the button name
            var button = sender as Button;
            var buttonNumberText = button.Name.Substring("button".Length);
            int audioTrackNumber = int.Parse(buttonNumberText);
            var audioTrackIndex = audioTrackNumber - 1;

            // Select the clicked audio track
            this.item.AudioTracks.SelectedIndex = audioTrackIndex;
            rootPage.NotifyUser("Switched to audio track #" + audioTrackNumber
                + " | Id: " + this.item.AudioTracks[audioTrackIndex].Id
                + " | Label: " + this.item.AudioTracks[audioTrackIndex].Label
                + " | Language: " + this.item.AudioTracks[audioTrackIndex].Language, NotifyType.StatusMessage);
        }
    }
}
