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
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace VideoPlayback
{
    /// <summary>
    /// Demonstrates closed captions delivered in-band, specifically SRT in an MKV.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        public Scenario3()
        {
            this.InitializeComponent();

            // Create the playback item
            var source = MediaSource.CreateFromUri(new Uri("ms-appx:///Assets/Media/ElephantsDream-Clip-H264_SD-AAC_eng-AAC_spa-AAC_eng_commentary-SRT_eng-SRT_por-SRT_swe.mkv"));
            var playbackItem = new MediaPlaybackItem(source);

            // Turn on English captions by default
            playbackItem.TimedMetadataTracksChanged += (sender, args) =>
            {
                var changedTrackIndex = args.Index;
                var changedTrack = playbackItem.TimedMetadataTracks[(int)changedTrackIndex];

                if (changedTrack.Language == "eng")
                    playbackItem.TimedMetadataTracks.SetPresentationMode((uint)changedTrackIndex, TimedMetadataTrackPresentationMode.PlatformPresented);
            };
            
            // Set the source to begin playback of the item
            this.mediaElement.SetPlaybackSource(playbackItem);
        }
    }
}
