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
using Windows.Media;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace SDKTemplate
{
    /// <summary>
    /// Multi-video live playback synchronization using an AdaptiveMediaSource.
    /// 
    /// Live sync has not been extensively tested for this release. There are limitations on the
    /// seekable and buffered ranges of data that are available to the MediaTimelineController.
    /// Exceeding these limits can cause stuttering or buffering.  This scenario is provided as
    /// a technology demonstration and is not intended for production use.
    /// </summary>
    public sealed partial class Scenario3_AdaptiveSync : Page
    {
        private MainPage rootPage = MainPage.Current;

        private List<MediaPlayerElement> mediaPlayerElements;
        private MediaTimelineController timelineController = null;
        private TimeSpan maxNaturalDurationForController = TimeSpan.Zero;
        private int openedMediaCount = 0;

        public Scenario3_AdaptiveSync()
        {
            this.InitializeComponent();

            mediaPlayerElements = new List<MediaPlayerElement>() { mpeMain, mpeCopy };

            foreach (MediaPlayerElement mpe in mediaPlayerElements)
            {
                // Create a corresponding MediaPlayer for each MediaPlayerElement.
                mpe.SetMediaPlayer(new MediaPlayer());
            }
        }

        private void Page_OnUnloaded(object sender, RoutedEventArgs e)
        {
            // Dispose the MediaPlayer objects so they will stop streaming.
            foreach (MediaPlayerElement mpe in mediaPlayerElements)
            {
                MediaPlayer mp = mpe.MediaPlayer;
                mp.MediaOpened -= MediaPlayer_MediaOpened;
                mp.Dispose();
            }
        }

        private void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            // Create a single MediaTimelineController which will control all of the MediaPlayers.
            timelineController = new MediaTimelineController();

            foreach (MediaPlayerElement mpe in mediaPlayerElements)
            {
                MediaPlayer mp = mpe.MediaPlayer;

                // The CommandManager object must be disabled when attaching a MediaTimelineController.
                // The CommandManager is usually used to route play/pause/seek commands to the MediaPlayer,
                // and apply additional business logic.
                // In a TimelineController scenario, it is important to control the MediaPlayer via a single mechanism.
                mp.CommandManager.IsEnabled = false;
                // Similarly, adding a MediaTimelineController to MediaPlayer will disable media transport controls in the UI.
                mp.TimelineController = timelineController;

                // As noted above, the transport controls are partially disabled due to the MediaTimelineController.
                // To avoid confusing the user, it is best to turn them off.
                mpe.AreTransportControlsEnabled = false;

                // MediaOpened is the right time to find the highest natural duration.
                mp.MediaOpened += MediaPlayer_MediaOpened;
            }

            // Use the MediaTimelineController.StateChanged to control the Play/Pause button content.
            timelineController.StateChanged += MediaTimelineController_StateChanged;

            // Since this is a sample of video on demand, constrain the MediaTimelineController
            // to pause when it reaches the natural end of the last video.
            timelineController.PositionChanged += MediaTimelineController_PositionChanged;

            // We are now set up to load content.
            Load.IsEnabled = true;
        }

        private async void MediaPlayer_MediaOpened(MediaPlayer mediaPlayer, object args)
        {
            // maxNaturalDurationForController is the longest media source of fixed duration.
            if (mediaPlayer.PlaybackSession.NaturalDuration != TimeSpan.MaxValue)
            {
                var naturalDurationForController = mediaPlayer.PlaybackSession.NaturalDuration - mediaPlayer.TimelineControllerPositionOffset;
                if (naturalDurationForController > maxNaturalDurationForController)
                {
                    maxNaturalDurationForController = naturalDurationForController;
                }
            }

            // If both media sources have resolved, update our UI with the duration information.
            if (System.Threading.Interlocked.Increment(ref openedMediaCount) == 2)
            {
                string message =
                    (maxNaturalDurationForController == TimeSpan.Zero) ?
                    " (live stream)" :
                    $" / {(int)maxNaturalDurationForController.TotalSeconds}";
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => NaturalDuration.Text = message);
            }
        }

        private async void MediaTimelineController_StateChanged(MediaTimelineController sender, object args)
        {
            // Update the Play/Pause button based on the new state.
            Symbol newSymbol;
            switch (sender.State)
            {
                case MediaTimelineControllerState.Paused:
                    newSymbol = Symbol.Play;
                    break;

                case MediaTimelineControllerState.Running:
                    newSymbol = Symbol.Pause;
                    break;

                default:
                    return;
            }
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => PlayPauseIcon.Symbol = newSymbol);
        }

        private void EnablePlaybackControls(bool enabled)
        {
            Seek30SecondsBack.IsEnabled = enabled;
            Seek2SecondsBack.IsEnabled = enabled;
            PlayPause.IsEnabled = enabled;
            Seek2SecondsForward.IsEnabled = enabled;
            Seek30SecondsForward.IsEnabled = enabled;
        }

        /// <summary>
        /// Sets the content on the MediaPlayers
        /// </summary>
        private void Load_Click(object sender, RoutedEventArgs e)
        {
            EnablePlaybackControls(false);

            // First reset some app logic.
            timelineController.Pause();
            maxNaturalDurationForController = TimeSpan.Zero;
            openedMediaCount = 0;
            foreach (MediaPlayerElement mpe in mediaPlayerElements)
            {
                MainPage.CleanUpMediaPlayerSource(mpe.MediaPlayer);
            }
            timelineController.Position = TimeSpan.Zero;

            // Apply any requested for offset between two videos.
            // Do this before setting the Source on the MediaPlayer to avoid a visible seek on MediaOpened.
            double d = 0.0;
            if (Double.TryParse(mpeCopyOffset.Text, out d))
            {
                mpeCopy.MediaPlayer.TimelineControllerPositionOffset = TimeSpan.FromSeconds(d);
            }
            else
            {
                mpeCopy.MediaPlayer.TimelineControllerPositionOffset = TimeSpan.Zero;
                mpeCopyOffset.Text = "0.0";
            }

            // Set the sources. For this demonstration, we play audio only from the first video.
            Uri uri;
            if (!Uri.TryCreate(mpeMainUri.Text, UriKind.Absolute, out uri))
            {
                rootPage.NotifyUser("Invalid URI for first source.", NotifyType.ErrorMessage);
                return;
            }
            mpeMain.MediaPlayer.Source = MediaSource.CreateFromUri(uri);
            mpeMain.MediaPlayer.IsMuted = false;

            if (!Uri.TryCreate(mpeCopyUri.Text, UriKind.Absolute, out uri))
            {
                rootPage.NotifyUser("Invalid URI for second source.", NotifyType.ErrorMessage);
                return;
            }
            mpeCopy.MediaPlayer.Source = MediaSource.CreateFromUri(uri);
            mpeMain.MediaPlayer.IsMuted = true;

            // Enable our custom playback controls now that everything is set up.
            EnablePlaybackControls(true);
        }

        private void PlayPause_Click(object sender, RoutedEventArgs e)
        {
            if (timelineController.State == MediaTimelineControllerState.Paused)
            {
                timelineController.Resume();
            }
            else if (timelineController.State == MediaTimelineControllerState.Running)
            {
                timelineController.Pause();
            }
        }

        #region Seek and Position management

        TimeSpan LastPositionUpdate = TimeSpan.Zero;
        TimeSpan PositionChangeThreshold = TimeSpan.FromSeconds(1);
        private async void MediaTimelineController_PositionChanged(MediaTimelineController sender, object args)
        {
            if (Math.Abs(sender.Position.Ticks - LastPositionUpdate.Ticks) > PositionChangeThreshold.Ticks)
            {
                LastPositionUpdate = sender.Position;
                await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => SeekPosition.Text = $"{(int)LastPositionUpdate.TotalSeconds}");
            }

            // Pause when playback reaches the maximum duration, if there is one.
            if (maxNaturalDurationForController != TimeSpan.Zero && sender.Position > maxNaturalDurationForController)
            {
                sender.Pause();
            }
        }

        private void Seek2SecondsForward_Click(object sender, RoutedEventArgs e)
        {
            SeekDelta(TimeSpan.FromSeconds(2));
        }
        private void Seek30SecondsForward_Click(object sender, RoutedEventArgs e)
        {
            SeekDelta(TimeSpan.FromSeconds(30));
        }

        private void Seek2SecondsBack_Click(object sender, RoutedEventArgs e)
        {
            SeekDelta(TimeSpan.FromSeconds(-2));
        }

        private void Seek30SecondsBack_Click(object sender, RoutedEventArgs e)
        {
            SeekDelta(TimeSpan.FromSeconds(-30));
        }

        private void SeekDelta(TimeSpan amount)
        {
            var requestedPosition = timelineController.Position + amount;
            if (maxNaturalDurationForController > TimeSpan.Zero)
            {
                if (requestedPosition <= maxNaturalDurationForController)
                {
                    timelineController.Position = requestedPosition;
                }
                else
                {
                    timelineController.Position = maxNaturalDurationForController;
                }
            }
            else
            {
                timelineController.Position = requestedPosition;
            }
        }
        #endregion
    }
}
