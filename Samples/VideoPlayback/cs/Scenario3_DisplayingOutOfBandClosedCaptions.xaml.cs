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
using System.Collections.Generic;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates playing media with closed captions delivered out-of-band, specifically an MP4 video supplemented by external SRT files.
    /// </summary>
    public sealed partial class Scenario3 : Page
    {
        private MainPage rootPage = MainPage.Current;

        // Keep a map to correlate sources with their URIs for error handling
        private Dictionary<TimedTextSource, Uri> ttsMap = new Dictionary<TimedTextSource, Uri>();

        public Scenario3()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Create the media source and supplement with external timed text sources.
            var source = MediaSource.CreateFromUri(rootPage.UncaptionedMediaUri);

            var ttsEnUri = new Uri("ms-appx:///Assets/Media/ElephantsDream-Clip-SRT_en.srt");
            var ttsEn = TimedTextSource.CreateFromUri(ttsEnUri);
            ttsMap[ttsEn] = ttsEnUri;

            var ttsPtUri = new Uri("ms-appx:///Assets/Media/ElephantsDream-Clip-SRT_pt.srt");
            var ttsPt = TimedTextSource.CreateFromUri(ttsPtUri);
            ttsMap[ttsPt] = ttsPtUri;

            var ttsSvUri = new Uri("ms-appx:///Assets/Media/ElephantsDream-Clip-SRT_sv.srt");
            var ttsSv = TimedTextSource.CreateFromUri(ttsSvUri);
            ttsMap[ttsSv] = ttsSvUri;

            ttsEn.Resolved += Tts_Resolved;
            ttsPt.Resolved += Tts_Resolved;
            ttsSv.Resolved += Tts_Resolved;

            source.ExternalTimedTextSources.Add(ttsEn);
            source.ExternalTimedTextSources.Add(ttsPt);
            source.ExternalTimedTextSources.Add(ttsSv);

            // Create the playback item from the source
            var playbackItem = new MediaPlaybackItem(source);

            // Present the first track
            playbackItem.TimedMetadataTracksChanged += (item, args) =>
            {
                playbackItem.TimedMetadataTracks.SetPresentationMode(0, TimedMetadataTrackPresentationMode.PlatformPresented);
            };

            // Set the source to start playback of the item
            this.mediaPlayerElement.Source = playbackItem;
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            MediaPlayerHelper.CleanUpMediaPlayerSource(mediaPlayerElement.MediaPlayer);
        }

        private void Tts_Resolved(TimedTextSource sender, TimedTextSourceResolveResultEventArgs args)
        {
            var ttsUri = ttsMap[sender];

            // Handle errors
            if(args.Error != null)
            {
                var ignoreAwaitWarning = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    rootPage.NotifyUser("Error resolving track " + ttsUri + " due to error " + args.Error.ErrorCode, NotifyType.ErrorMessage);
                });
                return;
            }

            // Update label manually since the external SRT does not contain it
            var ttsUriString = ttsUri.AbsoluteUri;
            if (ttsUriString.Contains("_en"))
                args.Tracks[0].Label = "English";
            else if (ttsUriString.Contains("_pt"))
                args.Tracks[0].Label = "Portuguese";
            else if (ttsUriString.Contains("_sv"))
                args.Tracks[0].Label = "Swedish";
        }
    }
}
