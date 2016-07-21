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
    /// Demonstrates multi-track video playback and camera selection.
    /// </summary>
    public sealed partial class Scenario5 : Page
    {
        MainPage rootPage = MainPage.Current;
        MediaPlaybackItem item;

        public Scenario5()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            this.item = new MediaPlaybackItem(MediaSource.CreateFromUri(rootPage.MultiTrackVideoMediaUri));

            this.mainVideoElement.SetPlaybackSource(item);
        }

        private void TrackButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the camera number based on the button name
            var button = sender as Button;
            var buttonNumberText = button.Name.Substring("button".Length);
            int videoTrackNumber = int.Parse(buttonNumberText);
            var videoTrackIndex = videoTrackNumber - 1;

            // Select the clicked camera track
            this.item.VideoTracks.SelectedIndex = videoTrackIndex;
            rootPage.NotifyUser("Switched to video track #" + videoTrackNumber 
                + " | Id: " + this.item.VideoTracks[videoTrackIndex].Id
                + " | Label: " + this.item.VideoTracks[videoTrackIndex].Label, NotifyType.StatusMessage);
        }
    }
}
