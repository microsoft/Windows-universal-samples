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
            var source = MediaSource.CreateFromUri(new Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/elephantsdream-clip-h264_sd-aac_eng-aac_spa-aac_eng_commentary-srt_eng-srt_por-srt_swe.mkv"));
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
