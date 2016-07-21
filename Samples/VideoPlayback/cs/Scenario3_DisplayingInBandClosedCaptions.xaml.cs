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
using Windows.Foundation.Collections;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates closed captions delivered in-band, specifically SRT in an MKV.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario3()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Create the playback item
            var source = MediaSource.CreateFromUri(rootPage.CaptionedMediaUri);
            var playbackItem = new MediaPlaybackItem(source);

            // Turn on English captions by default
            playbackItem.TimedMetadataTracksChanged += (sender, args) =>
            {
                if (args.CollectionChange == CollectionChange.ItemInserted)
                {
                    uint changedTrackIndex = args.Index;
                    TimedMetadataTrack changedTrack = playbackItem.TimedMetadataTracks[(int)changedTrackIndex];

                    if (changedTrack.Language == "en")
                    {
                        playbackItem.TimedMetadataTracks.SetPresentationMode(changedTrackIndex, TimedMetadataTrackPresentationMode.PlatformPresented);
                    }
                }
            };
            
            // Set the source to begin playback of the item
            this.mediaElement.SetPlaybackSource(playbackItem);
        }
    }
}
