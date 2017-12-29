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
using Windows.Media.Playback;

namespace SDKTemplate.Logging
{
    public class MediaPlayerLogger : IDisposable
    {
        #region Common pattern for all loggers

        private LogView logView;
        private MediaPlayer mediaPlayer;

        public MediaPlayerLogger(LogView _logView, MediaPlayer _mediaPlayer)
        {
            if (_logView == null)
                throw new ArgumentNullException(nameof(_logView));
            this.logView = _logView;

            if (_mediaPlayer == null)
                throw new ArgumentNullException(nameof(_mediaPlayer));
            this.mediaPlayer = _mediaPlayer;

            this.RegisterForMediaPlayerEvents();
        }

        public void Dispose()
        {
            UnregisterForMediaPlayerEvents();

            this.logView = null;
            this.mediaPlayer = null;
        }

        #endregion

        #region MediaPlayer Event Handlers

        private void RegisterForMediaPlayerEvents()
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
            mediaPlayer.PlaybackSession.BufferedRangesChanged += MediaPlayer_PlaybackSession_BufferedRangesChanged;
            mediaPlayer.PlaybackSession.BufferingEnded += MediaPlayer_PlaybackSession_BufferingEnded;
            mediaPlayer.PlaybackSession.BufferingProgressChanged += MediaPlayer_PlaybackSession_BufferingProgressChanged;
            mediaPlayer.PlaybackSession.BufferingStarted += MediaPlayer_PlaybackSession_BufferingStarted;
            mediaPlayer.PlaybackSession.DownloadProgressChanged += MediaPlayer_PlaybackSession_DownloadProgressChanged;
            mediaPlayer.PlaybackSession.NaturalDurationChanged += MediaPlayer_PlaybackSession_NaturalDurationChanged;
            mediaPlayer.PlaybackSession.NaturalVideoSizeChanged += MediaPlayer_PlaybackSession_NaturalVideoSizeChanged;
            mediaPlayer.PlaybackSession.PlaybackRateChanged += MediaPlayer_PlaybackSession_PlaybackRateChanged;
            mediaPlayer.PlaybackSession.PlaybackStateChanged += MediaPlayer_PlaybackSession_PlaybackStateChanged;
            mediaPlayer.PlaybackSession.PlayedRangesChanged += MediaPlayer_PlaybackSession_PlayedRangesChanged;
            mediaPlayer.PlaybackSession.PositionChanged += MediaPlayer_PlaybackSession_PositionChanged;
            mediaPlayer.PlaybackSession.SeekableRangesChanged += MediaPlayer_PlaybackSession_SeekableRangesChanged;
            mediaPlayer.PlaybackSession.SeekCompleted += MediaPlayer_PlaybackSession_SeekCompleted;
            mediaPlayer.PlaybackSession.SupportedPlaybackRatesChanged += MediaPlayer_PlaybackSession_SupportedPlaybackRatesChanged;
        }

        private void UnregisterForMediaPlayerEvents()
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
            mediaPlayer.PlaybackSession.BufferedRangesChanged -= MediaPlayer_PlaybackSession_BufferedRangesChanged;
            mediaPlayer.PlaybackSession.BufferingEnded -= MediaPlayer_PlaybackSession_BufferingEnded;
            mediaPlayer.PlaybackSession.BufferingProgressChanged -= MediaPlayer_PlaybackSession_BufferingProgressChanged;
            mediaPlayer.PlaybackSession.BufferingStarted -= MediaPlayer_PlaybackSession_BufferingStarted;
            mediaPlayer.PlaybackSession.DownloadProgressChanged -= MediaPlayer_PlaybackSession_DownloadProgressChanged;
            mediaPlayer.PlaybackSession.NaturalDurationChanged -= MediaPlayer_PlaybackSession_NaturalDurationChanged;
            mediaPlayer.PlaybackSession.NaturalVideoSizeChanged -= MediaPlayer_PlaybackSession_NaturalVideoSizeChanged;
            mediaPlayer.PlaybackSession.PlaybackRateChanged -= MediaPlayer_PlaybackSession_PlaybackRateChanged;
            mediaPlayer.PlaybackSession.PlaybackStateChanged -= MediaPlayer_PlaybackSession_PlaybackStateChanged;
            mediaPlayer.PlaybackSession.PlayedRangesChanged -= MediaPlayer_PlaybackSession_PlayedRangesChanged;
            mediaPlayer.PlaybackSession.PositionChanged -= MediaPlayer_PlaybackSession_PositionChanged;
            mediaPlayer.PlaybackSession.SeekableRangesChanged -= MediaPlayer_PlaybackSession_SeekableRangesChanged;
            mediaPlayer.PlaybackSession.SeekCompleted -= MediaPlayer_PlaybackSession_SeekCompleted;
            mediaPlayer.PlaybackSession.SupportedPlaybackRatesChanged += MediaPlayer_PlaybackSession_SupportedPlaybackRatesChanged;
        }

        private void MediaPlayer_SourceChanged(MediaPlayer sender, object args)
        {
            logView.Log("MediaPlayer_SourceChanged", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_MediaOpened(MediaPlayer sender, object args)
        {
            logView.Log($"MediaPlayer_MediaOpened, Duration: {sender.PlaybackSession.NaturalDuration}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_MediaEnded(MediaPlayer sender, object args)
        {
            logView.Log("MediaPlayer_MediaEnded", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_MediaFailed(MediaPlayer sender, MediaPlayerFailedEventArgs args)
        {
            logView.Log($"MediaPlayer_MediaFailed Error: {args.Error}, ErrorMessage: {args.ErrorMessage}, ExtendedError.Message:{args.ExtendedErrorCode.Message}, ExtendedError.HResult:{args.ExtendedErrorCode.HResult.ToString("X8")},", LogViewLoggingLevel.Critical);
        }
        private void MediaPlayer_VolumeChanged(MediaPlayer sender, object args)
        {
            logView.Log($"MediaPlayer_VolumeChanged, Volume: {sender.Volume}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_IsMutedChanged(MediaPlayer sender, object args)
        {
            logView.Log($"MediaPlayer_IsMutedChanged, IsMuted={sender.IsMuted}", LogViewLoggingLevel.Information);
        }
        // PlaybackSession Events
        private void MediaPlayer_PlaybackSession_BufferedRangesChanged(MediaPlaybackSession sender, object args)
        {
            var rangeList = sender.GetBufferedRanges();
            string ranges = "";
            foreach (var range in rangeList)
            {
                ranges += $"[{range.Start},{range.End}],";
            }
            ranges = ranges.TrimEnd(',');
            logView.Log($"MediaPlayer_PlaybackSession_BufferedRangesChanged: {ranges}", LogViewLoggingLevel.Verbose);
        }
        private void MediaPlayer_PlaybackSession_BufferingEnded(MediaPlaybackSession sender, object args)
        {
            logView.Log("PlaybackSession_BufferingEnded", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_PlaybackSession_BufferingProgressChanged(MediaPlaybackSession sender, object args)
        {
            logView.Log($"PlaybackSession_BufferingProgressChanged, BufferingProgress: {sender.BufferingProgress}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_PlaybackSession_BufferingStarted(MediaPlaybackSession sender, object args)
        {
            logView.Log("PlaybackSession_BufferingStarted", LogViewLoggingLevel.Warning);
        }
        private void MediaPlayer_PlaybackSession_DownloadProgressChanged(MediaPlaybackSession sender, object args)
        {
            logView.Log($"PlaybackSession_DownloadProgressChanged, DownloadProgress: {sender.DownloadProgress}", LogViewLoggingLevel.Verbose);
        }
        private void MediaPlayer_PlaybackSession_NaturalDurationChanged(MediaPlaybackSession sender, object args)
        {
            logView.Log($"PlaybackSession_NaturalDurationChanged, NaturalDuration: {sender.NaturalDuration}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_PlaybackSession_NaturalVideoSizeChanged(MediaPlaybackSession sender, object args)
        {
            logView.Log($"PlaybackSession_NaturalVideoSizeChanged,  NaturalVideoWidth: {sender.NaturalVideoWidth}, NaturalVideoHeight: {sender.NaturalVideoHeight}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_PlaybackSession_PlaybackRateChanged(MediaPlaybackSession sender, object args)
        {
            logView.Log($"PlaybackSession_PlaybackRateChanged, PlaybackRate: {sender.PlaybackRate}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_PlaybackSession_PlaybackStateChanged(MediaPlaybackSession sender, object args)
        {
            logView.Log($"PlaybackSession_PlaybackStateChanged, PlaybackState: {sender.PlaybackState}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_PlaybackSession_PlayedRangesChanged(MediaPlaybackSession sender, object args)
        {
            var rangeList = sender.GetPlayedRanges();
            string ranges = "";
            foreach (var range in rangeList)
            {
                ranges += $"[{range.Start},{range.End}],";
            }
            ranges = ranges.TrimEnd(',');
            logView.Log($"MediaPlayer_PlaybackSession_PlayedRangesChanged: {ranges}", LogViewLoggingLevel.Verbose);
        }
        public bool logPositionChanged { get; set; } = false;
        private void MediaPlayer_PlaybackSession_PositionChanged(MediaPlaybackSession sender, object args)
        {
            if(logPositionChanged)
                logView.Log("PlaybackSession_PositionChanged, Position: " + sender.Position, LogViewLoggingLevel.Verbose);
        }
        private void MediaPlayer_PlaybackSession_SeekableRangesChanged(MediaPlaybackSession sender, object args)
        {
            var rangeList = sender.GetSeekableRanges();
            string ranges = "";
            foreach (var range in rangeList)
            {
                ranges += $"[{range.Start},{range.End}],";
            }
            ranges = ranges.TrimEnd(',');
            logView.Log($"MediaPlayer_PlaybackSession_SeekableRangesChanged: {ranges}", LogViewLoggingLevel.Verbose);
        }
        private void MediaPlayer_PlaybackSession_SeekCompleted(MediaPlaybackSession sender, object args)
        {
            logView.Log($"PlaybackSession_SeekCompleted, Position: {sender.Position}", LogViewLoggingLevel.Information);
        }
        private void MediaPlayer_PlaybackSession_SupportedPlaybackRatesChanged(MediaPlaybackSession sender, object args)
        {
            double minRate, maxRate;
            GetMinMaxRates(sender, out minRate, out maxRate);
            logView.Log($"MediaPlayer_PlaybackSession_SupportedPlaybackRatesChanged, MinRate: {minRate}, MaxRate: {maxRate}", LogViewLoggingLevel.Information);
        }

        private static void GetMinMaxRates(MediaPlaybackSession sender, out double minRate, out double maxRate)
        {
            minRate = -256.0;
            bool foundMinRate = false;
            for (; !foundMinRate; minRate = minRate / 2.0)
            {
                foundMinRate = sender.IsSupportedPlaybackRateRange(minRate, 1);
                if (-0.25 < minRate)
                {
                    foundMinRate = true;
                    minRate = 0;
                }
            }
            maxRate = 256.0;
            bool foundMaxRate = false;
            for (; !foundMaxRate; maxRate = maxRate / 2.0)
            {
                foundMaxRate = sender.IsSupportedPlaybackRateRange(1, maxRate);
                if (maxRate < 1.0)
                {
                    foundMaxRate = true;
                    maxRate = 1;
                }
            }
        }
        #endregion
    }
}
