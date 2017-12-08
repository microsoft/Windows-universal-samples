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

using SDKTemplate.Logging.Extensions;
using System;
using Windows.Media.Core;
using Windows.Media.Playback;

namespace SDKTemplate.Logging
{
    public class MediaSourceLogger : IDisposable
    {
        #region Common pattern for all loggers

        private LogView logView;
        private MediaSource source;

        public MediaSourceLogger(LogView _logView, MediaSource _mediaSource)
        {
            if (_logView == null)
                throw new ArgumentNullException(nameof(_logView));
            this.logView = _logView;

            if (_mediaSource == null)
                throw new ArgumentNullException(nameof(_mediaSource));
            this.source = _mediaSource;

            this.RegisterForMediaSourceEvents();
        }

        public void Dispose()
        {
            this.UnregisterForMediaSourceEvents();

            this.logView = null;
            this.source = null;
        }
        
        #endregion

        #region MediaSource Event Handlers
        private void RegisterForMediaSourceEvents()
        {
            source.StateChanged += Source_StateChanged;
            source.OpenOperationCompleted += Source_OpenOperationCompleted;
        }

        private void UnregisterForMediaSourceEvents()
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
            logView.Log($"Source.StateChanged:{args.OldState} to {args.NewState}", LogViewLoggingLevel.Information);
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
                object customValue = null;
                sender.CustomProperties.TryGetValue("uri", out customValue);
                string uri = (string)customValue;
                customValue = null;
                sender.CustomProperties.TryGetValue("contentId", out customValue);
                string contentId = (string)customValue;
                logView.Log($"Opened Media Source with Uri: {uri}, ContentId: {contentId}, Codec: {audioCodec}", LogViewLoggingLevel.Information);

                // This extension method in MediaPlaybackItemStringExtensions dumps all the properties from all the tracks.
                var allProperties = item.ToFormattedString();
                logView.Log($"MediaPlaybackItem nested properties: {allProperties}", LogViewLoggingLevel.Verbose);
                // The AdaptiveMediaSource can manage multiple video tracks internally,
                // but only a single video track is exposed in the MediaPlaybackItem, not a collection.
            }
        }
        #endregion
    }
}
