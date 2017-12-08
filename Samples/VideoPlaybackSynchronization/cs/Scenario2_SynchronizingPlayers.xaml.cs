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
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Multi-video playback syncronization using TimelineControllerPositionOffset.
    /// </summary>
    public sealed partial class Scenario2_SynchronizingPlayers : Page
    {
        private List<MediaPlayerElement> mediaPlayerElements;
        private MediaTimelineController timelineController = null;
        private TimeSpan maxNaturalDurationForController = TimeSpan.Zero;

        // These videos will be played in the corresponding MediaPlayerElements.
        private Uri[] mediaUris = new Uri[2]
        {
                new Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/multiangle-right-back.mp4"),
                new Uri("https://mediaplatstorage1.blob.core.windows.net/windows-universal-samples-media/multiangle-right-back-offset-5-seconds.mp4"),
        };

        public Scenario2_SynchronizingPlayers()
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

        private void MediaPlayer_MediaOpened(MediaPlayer sender, object args)
        {
            // Maintain maxNaturalDurationForController so it is as long as the longest media source.
            var naturalDurationForController = sender.PlaybackSession.NaturalDuration - sender.TimelineControllerPositionOffset;
            if (naturalDurationForController > maxNaturalDurationForController)
            {
                maxNaturalDurationForController = naturalDurationForController;
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

        private void MediaTimelineController_PositionChanged(MediaTimelineController sender, object args)
        {
            // Pause when playback reaches the maximum duration.
            if (sender.Position > maxNaturalDurationForController)
            {
                sender.Pause();
            }
        }

        private void EnablePlaybackControls(bool enabled)
        {
            Seek2SecondsBack.IsEnabled = enabled;
            PlayPause.IsEnabled = enabled;
            Seek2SecondsForward.IsEnabled = enabled;
        }

        /// <summary>
        /// Sets the content on the MediaPlayers
        /// </summary>
        private void Load_Click(object sender, RoutedEventArgs e)
        {
            EnablePlaybackControls(false);

            // First reset the max duration, as the content is about to change.
            maxNaturalDurationForController = TimeSpan.Zero;

            // Correct for offset between the two videos.
            // Do this before setting the Source on the MediaPlayer to avoid a visible seek on MediaOpened.
            mediaPlayerElements[1].MediaPlayer.TimelineControllerPositionOffset = TimeSpan.FromSeconds(5.1);

            for (int i = 0; i < mediaUris.Length; i++)
            {
                MediaPlayer mp = mediaPlayerElements[i].MediaPlayer;

                // Set the Source property to a MediaSource, MediaPlaybackItem or MediaPlaybackList.
                mp.Source = MediaSource.CreateFromUri(mediaUris[i]);

                // In this demo, all of the videos has the same audio track, so mute all but the first one.
                mp.IsMuted = (i != 0);
            }

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

        private void Seek2SecondsForward_Click(object sender, RoutedEventArgs e)
        {
            // Go forward 2 seconds, but not beyond our maximum value.
            var requestedPosition = timelineController.Position + TimeSpan.FromSeconds(2);
            if (requestedPosition <= maxNaturalDurationForController)
            {
                timelineController.Position = requestedPosition;
            }
            else
            {
                timelineController.Position = maxNaturalDurationForController;
            }
        }

        private void Seek2SecondsBack_Click(object sender, RoutedEventArgs e)
        {
            timelineController.Position -= TimeSpan.FromSeconds(2);
        }
    }
}
