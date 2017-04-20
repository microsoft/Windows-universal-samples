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
using Windows.Globalization;
using Windows.Media.Core;
using Windows.Media.Playback;

namespace SDKTemplate.Logging
{
    public class MediaPlaybackItemLogger : IDisposable
    {
        #region Common pattern for all loggers

        private LogView logView;
        private MediaPlaybackItem item;

        public MediaPlaybackItemLogger(LogView _logView, MediaPlaybackItem _mediaPlaybackItem)
        {
            if (_logView == null)
                throw new ArgumentNullException(nameof(_logView));
            this.logView = _logView;

            if (_mediaPlaybackItem == null)
                throw new ArgumentNullException(nameof(_mediaPlaybackItem));
            this.item = _mediaPlaybackItem;

            this.RegisterForMediaPlaybackItemEvents();
        }

        public void Dispose()
        {
            this.UnregisterForMediaPlaybackItemEvents();

            this.logView = null;
            this.item = null;
        }
        #endregion

        #region MediaPlaybackItem Event Handlers

        private void RegisterForMediaPlaybackItemEvents()
        {
            item.AudioTracks.SelectedIndexChanged += AudioTracks_SelectedIndexChanged;
            item.AudioTracksChanged += Item_AudioTracksChanged;
            item.VideoTracksChanged += Item_VideoTracksChanged;
            item.TimedMetadataTracksChanged += Item_TimedMetadataTracksChanged;
        }

        private void UnregisterForMediaPlaybackItemEvents()
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
            object customValue;
            mpItem.Source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;
            string audioCodec = audioTrack.GetEncodingProperties().Subtype;
            var msg = $"The newly selected audio track of {contentId} has Codec {audioCodec}";
            var language = audioTrack.Language;
            if (!String.IsNullOrEmpty(language))
            {
                // Transform the language code into a UI-localized language name.
                msg += ", Language: " + (new Language(language)).DisplayName;
            }
            logView.Log(msg, LogViewLoggingLevel.Information);
        }

        private void Item_AudioTracksChanged(MediaPlaybackItem sender, IVectorChangedEventArgs args)
        {
            logView.Log($"item.AudioTracksChanged: CollectionChange:{args.CollectionChange}, Index:{args.Index}, Total:{sender.AudioTracks.Count}", LogViewLoggingLevel.Information);

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
            logView.Log($"item.VideoTracksChanged: CollectionChange:{args.CollectionChange}, Index:{args.Index}, Total:{sender.VideoTracks.Count}", LogViewLoggingLevel.Information);

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
            logView.Log($"item.TimedMetadataTracksChanged: CollectionChange:{args.CollectionChange}, Index:{args.Index}, Total:{sender.TimedMetadataTracks.Count}", LogViewLoggingLevel.Information);

            // This is the proper time to register for timed metadata Events the app cares to consume.
            // See the Metadata scenario for more details.
        }

        private void AudioTrack_OpenFailed(AudioTrack sender, AudioTrackOpenFailedEventArgs args)
        {
            logView.Log($"AudioTrack.OpenFailed: ExtendedError.Message:{args.ExtendedError.Message}, ExtendedError.HResult:{args.ExtendedError.HResult.ToString("X8")}, DecoderStatus:{sender.SupportInfo.DecoderStatus}, MediaSourceStatus:{sender.SupportInfo.MediaSourceStatus}", LogViewLoggingLevel.Error);
        }

        private void VideoTrack_OpenFailed(VideoTrack sender, VideoTrackOpenFailedEventArgs args)
        {
            logView.Log($"VideoTrack.OpenFailed: ExtendedError.Message:{args.ExtendedError.Message}, ExtendedError.HResult:{args.ExtendedError.HResult.ToString("X8")}, DecoderStatus:{sender.SupportInfo.DecoderStatus}, MediaSourceStatus:{sender.SupportInfo.MediaSourceStatus}", LogViewLoggingLevel.Error);
        }
                
        #endregion
    }
}
