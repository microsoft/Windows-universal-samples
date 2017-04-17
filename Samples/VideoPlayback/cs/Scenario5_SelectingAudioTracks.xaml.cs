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
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates multi-track audio playback and language selection.
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

            this.item = new MediaPlaybackItem(MediaSource.CreateFromUri(rootPage.CaptionedMediaUri));

            this.mediaPlayerElement.Source = item;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            MediaPlayerHelper.CleanUpMediaPlayerSource(mediaPlayerElement.MediaPlayer);
        }

        private void TrackButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the audio track index based on value of the 'tag' field
            var button = sender as Button;
            var audioTrackIndex = int.Parse(button.Tag as string);

            // Select the clicked audio track
            if (this.item != null && this.item.AudioTracks != null && this.item.AudioTracks.Count - 1 >= audioTrackIndex)
            {
                this.item.AudioTracks.SelectedIndex = audioTrackIndex;
                rootPage.NotifyUser("Switched to audio track #" + (audioTrackIndex + 1)
                    + " | Id: " + this.item.AudioTracks[audioTrackIndex].Id
                    + " | Label: " + this.item.AudioTracks[audioTrackIndex].Label
                    + " | Language: " + this.item.AudioTracks[audioTrackIndex].Language, NotifyType.StatusMessage);
            }
        }
    }
}
