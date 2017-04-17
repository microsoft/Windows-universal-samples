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

using SDKTemplate.Helpers;
using SDKTemplate.Models;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Globalization;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

namespace SDKTemplate
{
    /// See the README.md for discussion of this scenario.
    public sealed partial class Scenario2_EventHandlers : Page
    {
        AdaptiveMediaSource adaptiveMediaSource;
        Task LoadSourceFromUriTask = Task.CompletedTask;
        private BitrateHelper bitrateHelper;

        public Scenario2_EventHandlers()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            // To insure we don't clean-up half-constructed objects,
            // wait for any active source loading to complete.
            await LoadSourceFromUriTask;

            var mediaPlayer = mediaPlayerElement.MediaPlayer;
            if (mediaPlayer != null)
            {
                UnregisterForMediaPlayerEvents(mediaPlayer);
                UnregisterHandlers(mediaPlayer);
                mediaPlayer.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mediaPlayer.Dispose();
            }
        }

        private void UnregisterHandlers(MediaPlayer mediaPlayer)
        {
            MediaPlaybackItem mpItem = mediaPlayer.Source as MediaPlaybackItem;
            if (mpItem != null)
            {
                UnregisterForMediaPlaybackItemEvents(mpItem);
                UnregisterForMediaSourceEvents(mpItem.Source);
            }
            MediaSource source = mediaPlayer.Source as MediaSource;
            if (source != null)
            {
                UnregisterForMediaSourceEvents(source);
            }
            UnregisterForAdaptiveMediaSourceEvents(adaptiveMediaSource);
        }

        private async void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            // Explicitly create the instance of MediaPlayer if you need to register for its events
            // (like MediaOpened / MediaFailed) prior to setting an IMediaPlaybackSource.
            var mediaPlayer = new MediaPlayer();
            RegisterForMediaPlayerEvents(mediaPlayer);

            // Ensure we have PlayReady support, if the user enters a DASH/PR Uri in the text box:
            var prHelper = new PlayReadyHelper(LoggerControl);
            prHelper.SetUpProtectionManager(mediaPlayer);
            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            AdaptiveContentModel adaptiveContentModel = MainPage.FindContentById(1);
            UriBox.Text = adaptiveContentModel.ManifestUri.ToString();
            LoadSourceFromUriTask = LoadSourceFromUriAsync(adaptiveContentModel.ManifestUri);
            await LoadSourceFromUriTask;
        }

        private async void LoadUri_Click(object sender, RoutedEventArgs e)
        {
            Uri uri;
            if (!Uri.TryCreate(UriBox.Text, UriKind.Absolute, out uri))
            {
                Log("Malformed Uri in Load text box.");
                return;
            }

            LoadSourceFromUriTask = LoadSourceFromUriAsync(uri);
            await LoadSourceFromUriTask;

            // On small screens, hide the description text to make room for the video.
            DescriptionText.Visibility = (ActualHeight < 500) ? Visibility.Collapsed : Visibility.Visible;
        }

        private async Task LoadSourceFromUriAsync(Uri uri, HttpClient httpClient = null)
        {
            if (mediaPlayerElement.MediaPlayer?.Source != null)
            {
                UnregisterHandlers(mediaPlayerElement.MediaPlayer);
                mediaPlayerElement.MediaPlayer.DisposeSource();
            }

            AdaptiveMediaSourceCreationResult result = null;
            if (httpClient != null)
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri, httpClient);
            }
            else
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri);
            }

            // We don't need to save a reference to the MediaSource,
            // because we can obtain it from the MediaPlaybackItem.Source in event handlers.
            MediaSource source;

            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                adaptiveMediaSource = result.MediaSource;

                // At this point, we have read the manifest of the media source, and all bitrates are known.
                bitrateHelper = new BitrateHelper(adaptiveMediaSource.AvailableBitrates);

                // The AdaptiveMediaSource chooses initial playback and download bitrates.
                // See the Tuning scenario for examples of customizing these bitrates.
                await UpdatePlaybackBitrate(adaptiveMediaSource.CurrentPlaybackBitrate);
                await UpdateDownloadBitrateAsync(adaptiveMediaSource.CurrentDownloadBitrate);

                // Register for events before resolving the MediaSource.
                RegisterForAdaptiveMediaSourceEvents(adaptiveMediaSource);

                source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMediaSource);
            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource: {result.Status}");

                // Try to load the URI as a progressive download URI.
                Log($"Attempting to create a MediaSource from uri: {uri}");
                source = MediaSource.CreateFromUri(uri);
            }

            // You can save additional information in the CustomPropertySet for future retrieval.
            // Note: MediaSource.CustomProperties is a ValueSet and therefore can store
            // only serializable types.

            // Save the original Uri.
            source.CustomProperties.Add("uri", uri.ToString());

            // You're likely to put a content tracking id into the CustomProperties.
            source.CustomProperties.Add("contentId", Guid.NewGuid());

            RegisterForMediaSourceEvents(source);

            // Register for events before resolving the MediaSource.
            var mpItem = new MediaPlaybackItem(source);
            RegisterForMediaPlaybackItemEvents(mpItem);

            // It is at this point that the MediaSource (within a MediaPlaybackItem) will be fully resolved.
            // It gets opened, and events start being raised.

            // Since we are in an async function, the user may have user navigated away, which will null the MediaPlayer.
            if (mediaPlayerElement.MediaPlayer != null)
            {
                mediaPlayerElement.MediaPlayer.Source = mpItem;
            }
        }

        #region MediaPlayer Event Handlers
        private void RegisterForMediaPlayerEvents(MediaPlayer mediaPlayer)
        {
            // Player Events
            mediaPlayer.SourceChanged += MediaPlayer_SourceChanged;
            mediaPlayer.MediaOpened += MediaPlayer_MediaOpened;
            mediaPlayer.MediaEnded += MediaPlayer_MediaEnded;
            mediaPlayer.MediaFailed += MediaPlayer_MediaFailed;
            mediaPlayer.VolumeChanged += MediaPlayer_VolumeChanged;
            mediaPlayer.IsMutedChanged += MediaPlayer_IsMutedChanged;
            // NOTE: There are a number of deprecated events on MediaPlayer.
            // Please use the equivalent events on the MediaPlayer.PlaybackSession as shown below.

            // PlaybackSession Events
            mediaPlayer.PlaybackSession.BufferingEnded += MediaPlayer_PlaybackSession_BufferingEnded;
            mediaPlayer.PlaybackSession.BufferingProgressChanged += MediaPlayer_PlaybackSession_BufferingProgressChanged;
            mediaPlayer.PlaybackSession.BufferingStarted += MediaPlayer_PlaybackSession_BufferingStarted;
            mediaPlayer.PlaybackSession.DownloadProgressChanged += MediaPlayer_PlaybackSession_DownloadProgressChanged;
            mediaPlayer.PlaybackSession.NaturalDurationChanged += MediaPlayer_PlaybackSession_NaturalDurationChanged;
            mediaPlayer.PlaybackSession.NaturalVideoSizeChanged += MediaPlayer_PlaybackSession_NaturalVideoSizeChanged;
            mediaPlayer.PlaybackSession.PlaybackRateChanged += MediaPlayer_PlaybackSession_PlaybackRateChanged;
            mediaPlayer.PlaybackSession.PlaybackStateChanged += MediaPlayer_PlaybackSession_PlaybackStateChanged;
            // The .PositionChanged is excessively verbose for the UI logging we are doing in this sample:
            // mediaPlayer.PlaybackSession.PositionChanged += MediaPlayer_PlaybackSession_PositionChanged;
            mediaPlayer.PlaybackSession.SeekCompleted += MediaPlayer_PlaybackSession_SeekCompleted;
        }

        private void UnregisterForMediaPlayerEvents(MediaPlayer mediaPlayer)
        {
            if (mediaPlayer == null)
            {
                return;
            }

            // Player Events
            mediaPlayer.SourceChanged -= MediaPlayer_SourceChanged;
            mediaPlayer.MediaOpened -= MediaPlayer_MediaOpened;
            mediaPlayer.MediaEnded -= MediaPlayer_MediaEnded;
            mediaPlayer.MediaFailed -= MediaPlayer_MediaFailed;
            mediaPlayer.VolumeChanged -= MediaPlayer_VolumeChanged;
            mediaPlayer.IsMutedChanged -= MediaPlayer_IsMutedChanged;

            // PlaybackSession Events
            mediaPlayer.PlaybackSession.BufferingEnded -= MediaPlayer_PlaybackSession_BufferingEnded;
            mediaPlayer.PlaybackSession.BufferingProgressChanged -= MediaPlayer_PlaybackSession_BufferingProgressChanged;
            mediaPlayer.PlaybackSession.BufferingStarted -= MediaPlayer_PlaybackSession_BufferingStarted;
            mediaPlayer.PlaybackSession.DownloadProgressChanged -= MediaPlayer_PlaybackSession_DownloadProgressChanged;
            mediaPlayer.PlaybackSession.NaturalDurationChanged -= MediaPlayer_PlaybackSession_NaturalDurationChanged;
            mediaPlayer.PlaybackSession.NaturalVideoSizeChanged -= MediaPlayer_PlaybackSession_NaturalVideoSizeChanged;
            mediaPlayer.PlaybackSession.PlaybackRateChanged -= MediaPlayer_PlaybackSession_PlaybackRateChanged;
            mediaPlayer.PlaybackSession.PlaybackStateChanged -= MediaPlayer_PlaybackSession_PlaybackStateChanged;
            mediaPlayer.PlaybackSession.SeekCompleted -= MediaPlayer_PlaybackSession_SeekCompleted;
        }

        private void MediaPlayer_SourceChanged(MediaPlayer sender, object args)
        {
            Log("mediaPlayer.SourceChanged");
        }
        private void MediaPlayer_MediaOpened(MediaPlayer sender, object args)
        {
            Log($"MediaPlayer_MediaOpened, Duration: {sender.PlaybackSession.NaturalDuration}");
        }
        private void MediaPlayer_MediaEnded(MediaPlayer sender, object args)
        {
            Log("MediaPlayer_MediaEnded");
        }
        private void MediaPlayer_MediaFailed(MediaPlayer sender, MediaPlayerFailedEventArgs args)
        {
            Log($"MediaPlayer_MediaFailed Error: {args.Error}, ErrorMessage: {args.ErrorMessage}, ExtendedErrorCode.Message: {args.ExtendedErrorCode.Message}");
        }
        private void MediaPlayer_VolumeChanged(MediaPlayer sender, object args)
        {
            Log($"MediaPlayer_VolumeChanged, Volume: {sender.Volume}");
        }
        private void MediaPlayer_IsMutedChanged(MediaPlayer sender, object args)
        {
            Log($"MediaPlayer_IsMutedChanged, IsMuted={sender.IsMuted}");
        }
        // PlaybackSession Events
        private void MediaPlayer_PlaybackSession_BufferingEnded(MediaPlaybackSession sender, object args)
        {
            Log("PlaybackSession_BufferingEnded");
        }
        private void MediaPlayer_PlaybackSession_BufferingProgressChanged(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_BufferingProgressChanged, BufferingProgress: {sender.BufferingProgress}");
        }
        private void MediaPlayer_PlaybackSession_BufferingStarted(MediaPlaybackSession sender, object args)
        {
            Log("PlaybackSession_BufferingStarted");
        }
        private void MediaPlayer_PlaybackSession_DownloadProgressChanged(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_DownloadProgressChanged, DownloadProgress: {sender.DownloadProgress}");
        }
        private void MediaPlayer_PlaybackSession_NaturalDurationChanged(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_NaturalDurationChanged, NaturalDuration: {sender.NaturalDuration}");
        }
        private void MediaPlayer_PlaybackSession_NaturalVideoSizeChanged(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_NaturalVideoSizeChanged,  NaturalVideoWidth: {sender.NaturalVideoWidth}, NaturalVideoHeight: {sender.NaturalVideoHeight}");
        }
        private void MediaPlayer_PlaybackSession_PlaybackRateChanged(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_PlaybackRateChanged, PlaybackRate: {sender.PlaybackRate}");
        }
        private void MediaPlayer_PlaybackSession_PlaybackStateChanged(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_PlaybackStateChanged, PlaybackState: {sender.PlaybackState}");
        }
        // private void MediaPlayer_PlaybackSession.PositionChanged(MediaPlaybackSession sender, object args)
        // {
        //     LogOnUI("PlaybackSession_PositionChanged, Position: " + sender.Position);
        // }

        private void MediaPlayer_PlaybackSession_SeekCompleted(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_SeekCompleted, Position: {sender.Position}");
        }
        #endregion

        #region MediaSource Event Handlers
        private void RegisterForMediaSourceEvents(MediaSource source)
        {
            source.StateChanged += Source_StateChanged;
            source.OpenOperationCompleted += Source_OpenOperationCompleted;
        }

        private void UnregisterForMediaSourceEvents(MediaSource source)
        {
            if (source == null)
            {
                return;
            }
            source.StateChanged -= Source_StateChanged;
            source.OpenOperationCompleted -= Source_OpenOperationCompleted;
        }

        private void Source_StateChanged(MediaSource sender, MediaSourceStateChangedEventArgs args)
        {
            Log($"Source.StateChanged:{args.OldState} to {args.NewState}");
        }

        private void Source_OpenOperationCompleted(MediaSource sender, MediaSourceOpenOperationCompletedEventArgs args)
        {
            // Get the MediaPlaybackItem that corresponds to the MediaSource.
            MediaPlaybackItem item = MediaPlaybackItem.FindFromMediaSource(sender);
            if (item != null && item.AudioTracks.Count > 0)
            {
                // The MediaPlaybackItem contains additional information about the underlying media.
                string audioCodec = item.AudioTracks[item.AudioTracks.SelectedIndex].GetEncodingProperties().Subtype;

                // We can read values from the MediaSource.CustomProperties.
                var uri = (string)sender.CustomProperties["uri"];
                var contentId = (Guid)sender.CustomProperties["contentId"];
                Log($"Opened Media Source with Uri: {uri}, ContentId: {contentId}, Codec: {audioCodec}");

                // This extension method in MediaPlaybackItemStringExtensions dumps all the properties from all the tracks.
                var allProperties = item.ToFormattedString();
                Log($"MediaPlaybackItem nested properties: {allProperties}");
                // The AdaptiveMediaSource can manage multiple video tracks internally,
                // but only a single video track is exposed in the MediaPlaybackItem, not a collection.
            }
        }
        #endregion

        #region MediaPlaybackItem Event Handlers

        private void RegisterForMediaPlaybackItemEvents(MediaPlaybackItem item)
        {
            item.AudioTracks.SelectedIndexChanged += AudioTracks_SelectedIndexChanged;
            item.AudioTracksChanged += Item_AudioTracksChanged;
            item.VideoTracksChanged += Item_VideoTracksChanged;
            item.TimedMetadataTracksChanged += Item_TimedMetadataTracksChanged;
        }

        private void UnregisterForMediaPlaybackItemEvents(MediaPlaybackItem item)
        {
            if (item == null)
            {
                return;
            }
            item.AudioTracks.SelectedIndexChanged -= AudioTracks_SelectedIndexChanged;
            item.AudioTracksChanged -= Item_AudioTracksChanged;
            item.VideoTracksChanged -= Item_VideoTracksChanged;
            item.TimedMetadataTracksChanged -= Item_TimedMetadataTracksChanged;
            foreach (AudioTrack audioTrack in item.AudioTracks)
            {
                audioTrack.OpenFailed -= AudioTrack_OpenFailed;
            }
            foreach (VideoTrack videoTrack in item.VideoTracks)
            {
                videoTrack.OpenFailed -= VideoTrack_OpenFailed;
            }
        }

        private void AudioTracks_SelectedIndexChanged(ISingleSelectMediaTrackList sender, object args)
        {
            MediaPlaybackAudioTrackList list = sender as MediaPlaybackAudioTrackList;
            AudioTrack audioTrack = list[sender.SelectedIndex];
            MediaPlaybackItem mpItem = audioTrack.PlaybackItem;
            var contentId = (Guid)mpItem.Source.CustomProperties["contentId"];
            string audioCodec = audioTrack.GetEncodingProperties().Subtype;
            var msg = $"The newly selected audio track of {contentId} has Codec {audioCodec}";
            var language = audioTrack.Language;
            if (!String.IsNullOrEmpty(language))
            {
                // Transform the language code into a UI-localized language name.
                msg += ", Language: " + (new Language(language)).DisplayName;
            }
            Log(msg);
        }

        private void Item_AudioTracksChanged(MediaPlaybackItem sender, IVectorChangedEventArgs args)
        {
            Log($"item.AudioTracksChanged: CollectionChange:{args.CollectionChange} Index:{args.Index} Total:{sender.AudioTracks.Count}");

            switch (args.CollectionChange)
            {
                case CollectionChange.Reset:
                    foreach (AudioTrack track in sender.AudioTracks)
                    {
                        // Tracks are added once as a source discovers them in the source media.
                        // This occurs prior to the track media being opened.
                        // Register here so that we can receive the Track.OpenFailed event.
                        track.OpenFailed += AudioTrack_OpenFailed;
                    }
                    break;
                case CollectionChange.ItemInserted:
                    // Additional tracks added after loading the main source should be registered here.
                    AudioTrack newTrack = sender.AudioTracks[(int)args.Index];
                    newTrack.OpenFailed += AudioTrack_OpenFailed;
                    break;
            }
        }

        private void Item_VideoTracksChanged(MediaPlaybackItem sender, IVectorChangedEventArgs args)
        {
            Log($"item.VideoTracksChanged: CollectionChange:{args.CollectionChange} Index:{args.Index} Total:{sender.VideoTracks.Count}");

            switch (args.CollectionChange)
            {
                case CollectionChange.Reset:
                    foreach (VideoTrack track in sender.VideoTracks)
                    {
                        track.OpenFailed += VideoTrack_OpenFailed;
                    }
                    break;
                case CollectionChange.ItemInserted:
                    VideoTrack newTrack = sender.VideoTracks[(int)args.Index];
                    newTrack.OpenFailed += VideoTrack_OpenFailed;
                    break;
            }
        }

        private void Item_TimedMetadataTracksChanged(MediaPlaybackItem sender, IVectorChangedEventArgs args)
        {
            Log($"item.TimedMetadataTracksChanged: CollectionChange:{args.CollectionChange} Index:{args.Index} Total:{sender.TimedMetadataTracks.Count}");

            // This is the proper time to register for timed metadata Events the app cares to consume.
            // See the Metadata scenario for more details.
        }

        private void AudioTrack_OpenFailed(AudioTrack sender, AudioTrackOpenFailedEventArgs args)
        {
            Log($"AudioTrack.OpenFailed: ExtendedError:{args.ExtendedError} DecoderStatus:{sender.SupportInfo.DecoderStatus} MediaSourceStatus:{sender.SupportInfo.MediaSourceStatus}");
        }

        private void VideoTrack_OpenFailed(VideoTrack sender, VideoTrackOpenFailedEventArgs args)
        {
            Log($"VideoTrack.OpenFailed: ExtendedError:{args.ExtendedError} DecoderStatus:{sender.SupportInfo.DecoderStatus} MediaSourceStatus:{sender.SupportInfo.MediaSourceStatus}");
        }


        #endregion

        #region AdaptiveMediaSource Event Handlers

        private void RegisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource adaptiveMediaSource)
        {
            adaptiveMediaSource.DownloadRequested += DownloadRequested;
            adaptiveMediaSource.DownloadFailed += DownloadFailed;
            adaptiveMediaSource.DownloadCompleted += DownloadCompleted;
            adaptiveMediaSource.DownloadBitrateChanged += DownloadBitrateChanged;
            adaptiveMediaSource.PlaybackBitrateChanged += PlaybackBitrateChanged;
        }

        private void UnregisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource adaptiveMediaSource)
        {
            adaptiveMediaSource.DownloadRequested -= DownloadRequested;
            adaptiveMediaSource.DownloadFailed -= DownloadFailed;
            adaptiveMediaSource.DownloadCompleted -= DownloadCompleted;
            adaptiveMediaSource.DownloadBitrateChanged -= DownloadBitrateChanged;
            adaptiveMediaSource.PlaybackBitrateChanged -= PlaybackBitrateChanged;
        }

        private void VerboseLog_Click(object sender, RoutedEventArgs e)
        {
            verbose = (sender as CheckBox).IsChecked.Value;
        }

        private bool verbose = false;
        private void DownloadRequested(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            if (verbose)
            {
                Log($"DownloadRequested: {args.ResourceType}, {args.ResourceUri}");
            }
        }

        private void DownloadFailed(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadFailedEventArgs args)
        {
            Log($"DownloadFailed: {args.HttpResponseMessage}, {args.ResourceType}, {args.ResourceUri}");
        }

        private void DownloadCompleted(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadCompletedEventArgs args)
        {
            if (verbose)
            {
                Log($"DownloadCompleted: {args.ResourceType}, {args.ResourceUri}");
            }
            // You can get a copy of the http response bytes for additional processing.
            // Note that the AdaptiveMediaSource has already consumed these bytes. Modifying them has no effect.
            // If you need to modify the request or send specific bytes to AdaptiveMediaSource, do so in
            // DownloadRequested.  See the RequestModification scenario for more details.
            // var buffer = await args.HttpResponseMessage.Content.ReadAsBufferAsync();
        }

        private async void DownloadBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadBitrateChangedEventArgs args)
        {
            uint downloadBitrate = args.NewValue;
            await UpdateDownloadBitrateAsync(downloadBitrate);
        }

        private async Task UpdateDownloadBitrateAsync(uint downloadBitrate)
        {
            Log($"DownloadBitrateChanged: {downloadBitrate}");
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                iconDownloadBitrate.Symbol = bitrateHelper.GetBitrateSymbol(downloadBitrate);
                txtDownloadBitrate.Text = downloadBitrate.ToString();
            });
        }

        private async void PlaybackBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourcePlaybackBitrateChangedEventArgs args)
        {
            uint playbackBitrate = args.NewValue;
            await UpdatePlaybackBitrate(playbackBitrate);
        }

        private async Task UpdatePlaybackBitrate(uint playbackBitrate)
        {
            Log($"PlaybackBitrateChanged: {playbackBitrate}");
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                iconPlaybackBitrate.Symbol = bitrateHelper.GetBitrateSymbol(playbackBitrate);
                txtPlaybackBitrate.Text = playbackBitrate.ToString();
            });
        }

        #endregion

        #region Utilities
        private void Log(string message)
        {
            LoggerControl.Log(message);
        }
        #endregion
    }
}

