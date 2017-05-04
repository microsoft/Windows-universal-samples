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
using SDKTemplate.Logging;
using SDKTemplate.Logging.Extensions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Media;
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

    public sealed partial class Scenario7_LiveSeekableRange : Page
    {
        public Scenario7_LiveSeekableRange()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            // Release handles on various media objects to ensure a quick clean-up
            ContentSelectorControl.MediaPlaybackItem = null;
            var mediaPlayer = mediaPlayerElement.MediaPlayer;
            if (mediaPlayer != null)
            {
                mediaPlayerLogger?.Dispose();
                mediaPlayerLogger = null;

                UnregisterForDiscreteControls(mediaPlayer);
                UnregisterForPositionUpdateControl(mediaPlayer);

                UnregisterHandlers(mediaPlayer);

                mediaPlayer.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mediaPlayer.Dispose();
            }
        }

        private void UnregisterHandlers(MediaPlayer mediaPlayer)
        {
            mediaPlaybackItemLogger?.Dispose();
            mediaPlaybackItemLogger = null;

            mediaSourceLogger?.Dispose();
            mediaSourceLogger = null;

            adaptiveMediaSourceLogger?.Dispose();
            adaptiveMediaSourceLogger = null;
        }

        private void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            var mediaPlayer = new MediaPlayer();

            // We use a helper class that logs all the events for the MediaPlayer:
            mediaPlayerLogger = new MediaPlayerLogger(LoggerControl, mediaPlayer);

            // In addition to logging, we use the callbacks to update some UI elements in this scenario:
            RegisterForDiscreteControls(mediaPlayer);
            RegisterForPositionUpdateControls(mediaPlayer);

            // Ensure we have PlayReady support, in case the user enters a DASH/PR Uri in the text box.
            var prHelper = new PlayReadyHelper(LoggerControl);
            prHelper.SetUpProtectionManager(mediaPlayer);
            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            ContentSelectorControl.Initialize(
                mediaPlayer,
                MainPage.ContentManagementSystemStub.Where(m => !m.Aes && m.Live),
                null,
                LoggerControl,
                LoadSourceFromUriAsync);

            // Initialize scenario specific layout:
            ContentSelectorControl.SetAutoPlay(true);
            mediaPlayerElement.AreTransportControlsEnabled = false;
            LoggerControl.Height = 200;
        }

        #region Content Loading

        private async Task<MediaPlaybackItem> LoadSourceFromUriAsync(Uri uri, HttpClient httpClient = null)
        {
            UnregisterHandlers(mediaPlayerElement.MediaPlayer);
            mediaPlayerElement.MediaPlayer?.DisposeSource();

            AdaptiveMediaSourceCreationResult result = null;
            if (httpClient != null)
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri, httpClient);
            }
            else
            {
                result = await AdaptiveMediaSource.CreateFromUriAsync(uri);
            }

            MediaSource source;
            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                var adaptiveMediaSource = result.MediaSource;

                // We use a helper class that logs all the events for the AdaptiveMediaSource:
                adaptiveMediaSourceLogger = new AdaptiveMediaSourceLogger(LoggerControl, adaptiveMediaSource);

                source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMediaSource);

                if (adaptiveMediaSource.MaxSeekableWindowSize.HasValue)
                {
                    UpdateSeekableWindowControls(adaptiveMediaSource);
                }
            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource. Status: {result.Status}, ExtendedError.Message: {result.ExtendedError.Message}, ExtendedError.HResult: {result.ExtendedError.HResult.ToString("X8")}");
                return null;
            }

            // We use a helper class that logs all the events for the MediaSource:
            mediaSourceLogger = new MediaSourceLogger(LoggerControl, source);

            // Save the original Uri.
            source.CustomProperties["uri"] = uri.ToString();

            // You're likely to put a content tracking id into the CustomProperties.
            source.CustomProperties["contentId"] = Guid.NewGuid().ToString();

            var mpItem = new MediaPlaybackItem(source);

            // We use a helper class that logs all the events for the MediaPlaybackItem:
            mediaPlaybackItemLogger = new MediaPlaybackItemLogger(LoggerControl, mpItem);

            HideDescriptionOnSmallScreen();

            return mpItem;
        }

        private async void HideDescriptionOnSmallScreen()
        {
            // On small screens, hide the description text to make room for the video.
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                DescriptionText.Visibility = (ActualHeight < 500) ? Visibility.Collapsed : Visibility.Visible;
            });
        }
        #endregion

        #region CustomMediaControl

        #region SeekableWindowControls

        private void DesiredSeekableWindowSizeButton_Click(object sender, RoutedEventArgs e)
        {
            var adaptiveMediaSource = (mediaPlayerElement.MediaPlayer.Source as MediaPlaybackItem)?.Source?.AdaptiveMediaSource;
            if (adaptiveMediaSource == null)
                return;
            TimeSpan result = adaptiveMediaSource.DesiredSeekableWindowSize.GetValueOrDefault();
            if (TimeSpan.TryParse(DesiredSeekableWindowSize.Text, out result))
            {
                adaptiveMediaSource.DesiredSeekableWindowSize = result;
            }
            UpdateSeekableWindowControls();
        }

        private void DesiredLiveOffsetButton_Click(object sender, RoutedEventArgs e)
        {
            var adaptiveMediaSource = (mediaPlayerElement.MediaPlayer.Source as MediaPlaybackItem)?.Source?.AdaptiveMediaSource;
            if (adaptiveMediaSource == null)
                return;
            TimeSpan result = adaptiveMediaSource.DesiredLiveOffset;
            if (TimeSpan.TryParse(DesiredLiveOffset.Text, out result))
            {
                adaptiveMediaSource.DesiredLiveOffset = result;
            }
            UpdateSeekableWindowControls();
        }

        private void UpdateSeekableWindowControls()
        {
            var adaptiveMediaSource = (mediaPlayerElement.MediaPlayer.Source as MediaPlaybackItem)?.Source?.AdaptiveMediaSource;
            UpdateSeekableWindowControls(adaptiveMediaSource);
        }

        private void UpdateSeekableWindowControls(AdaptiveMediaSource adaptiveMediaSource)
        {
            if (adaptiveMediaSource == null)
                return;

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                MaxSeekableWindowSize.Text = adaptiveMediaSource.MaxSeekableWindowSize.HasValue ? $"{adaptiveMediaSource.MaxSeekableWindowSize.Value.ToString("c")}" : "";
                DesiredSeekableWindowSize.Text = adaptiveMediaSource.DesiredSeekableWindowSize.HasValue ? $"{adaptiveMediaSource.DesiredSeekableWindowSize.Value.ToString("c")}" : "";
                DesiredLiveOffset.Text = $"{adaptiveMediaSource.DesiredLiveOffset.ToString("c")}";
                MinLiveOffset.Text = adaptiveMediaSource.MinLiveOffset.HasValue ? $"{adaptiveMediaSource.MinLiveOffset.Value.ToString("c")}" : "";
            });
        }

        #endregion

        #region PositionUpdateControl

        double m_SliderFullScale = 100000.0; // Sets granularity of slider

        TimeSpan m_PlaybackPositionMaxTime = TimeSpan.FromHours(1);
        TimeSpan m_PlaybackPositionMinTime= TimeSpan.Zero;

        private void RegisterForPositionUpdateControls(MediaPlayer mp)
        {
            // Setup slider
            PositionSlider.Minimum = 0.0;
            PositionSlider.Maximum = m_SliderFullScale;
            PositionSlider.Value = 0.0;

            // Register for events
            mp.SourceChanged += PositionUpdateControl_SourceChanged;
            mp.PlaybackSession.PositionChanged += PositionUpdateControl_PositionChanged;
            mp.PlaybackSession.SeekableRangesChanged += PositionUpdateControl_SeekableRangesChanged;
        }

        private void PositionUpdateControl_SourceChanged(MediaPlayer sender, object args)
        {
            m_PlaybackPositionMaxTime = TimeSpan.FromHours(1);
            m_PlaybackPositionMinTime = TimeSpan.Zero;
        }

        private void UnregisterForPositionUpdateControl(MediaPlayer mp)
        {
            mp.SourceChanged -= PositionUpdateControl_SourceChanged;
            mp.PlaybackSession.PositionChanged -= PositionUpdateControl_PositionChanged;
            mp.PlaybackSession.SeekableRangesChanged -= PositionUpdateControl_SeekableRangesChanged;
        }

        private void PositionUpdateControl_SeekableRangesChanged(MediaPlaybackSession sender, object args)
        {
            var seekableRanges = sender.GetSeekableRanges();
            UpdateSeekableRangesControl(seekableRanges);
        }

        private void UpdateSeekableRangesControl(IReadOnlyList<MediaTimeRange> SeekableRanges)
        {
            if (SeekableRanges.Count > 0)
            {
                var SeekableRangeMin = SeekableRanges.First();
                m_PlaybackPositionMinTime = SeekableRangeMin.Start;

                var SeekableRangeMax = SeekableRanges.Last();
                m_PlaybackPositionMaxTime = SeekableRangeMax.End;
            }
            else
            {
                m_PlaybackPositionMaxTime = TimeSpan.FromHours(1);
                m_PlaybackPositionMinTime = TimeSpan.Zero;
            }

            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                StartPosition.Text = m_PlaybackPositionMinTime.ToString("c");
                EndPosition.Text = m_PlaybackPositionMaxTime.ToString("c");
            });

            string ranges = "";
            foreach (var range in SeekableRanges)
            {
                ranges += $"[{range.Start},{range.End}],";
            }
            ranges = ranges.TrimEnd(',');
            Log($"MediaPlayer_PlaybackSession_SeekableRangesChanged: {ranges}");
        }

        private async void PositionUpdateControl_PositionChanged(MediaPlaybackSession sender, object args)
        {
            var postion = sender.Position;
            var newSliderValue = GetSliderPositionAtPlaybackPosition(postion, m_SliderFullScale, m_PlaybackPositionMinTime, m_PlaybackPositionMaxTime);
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                CurrentPosition.Text = postion.ToString("c");
                PositionSlider.Value = newSliderValue;
            });
        }

        #region PositionSlider events

        private void SeekToRequestedPosition(TimeSpan requestedPosition)
        {
            Log($"Seeking to:{requestedPosition.ToString("c")}");
            mediaPlayerElement.MediaPlayer.PlaybackSession.Position = requestedPosition;
        }

        #endregion

        /// <summary>
        /// We will use slider ticks to drive a playback Position.
        ///
        /// This is a linear interpolation, so:
        /// y = m*x + b
        /// Where:
        /// Position = m*sliderPosition + b
        ///
        /// Where we have some data points:
        ///
        /// At MinPosition, we are at slider position=0
        /// MinPosition = m * 0 + b
        /// Thus:
        ///   b = MinPosition
        ///
        /// We choose a 100000 tick granularity for the slider,
        /// at which point it will be at MaxPosition:
        /// MaxPosition = m * 100000 + b
        /// MaxPosition = m * 100000 + MinPosition
        ///
        /// Thus:
        ///   m = (MaxPosition - MinPosition) / 100000
        ///   m = (MaxPosition - MinPosition) / sliderFullScale
        ///
        /// And more generally, we have:
        ///
        /// PlaybackPosition =  [(MaxPosition - MinPosition) / sliderFullScale] * SliderPosition + MinPosition
        /// or:
        /// PlaybackPosition =  ((MaxPosition - MinPosition) * SliderPosition / sliderFullScale)   + MinPosition
        ///
        /// </summary>
        public TimeSpan GetPlaybackPosition(double sliderPosition, double sliderFullScale, TimeSpan MinPosition, TimeSpan MaxPosition)
        {
            if (sliderFullScale == 0)
                return TimeSpan.Zero;
            return TimeSpan.FromTicks((long)((MaxPosition - MinPosition).Ticks * sliderPosition / sliderFullScale) + MinPosition.Ticks);
        }

        /// <summary>
        /// Given:
        /// y = m * x + b
        /// x = (y - b) /m
        ///
        /// Where from above:
        ///  b = MinPosition
        ///  m = (MaxPosition - MinPosition) / sliderFullScale
        ///
        /// We have:
        ///  Position = (PlaybackPosition - MinPosition) * sliderFullScale / (MaxPosition - MinPosition)
        ///
        /// </summary>
        public double GetSliderPositionAtPlaybackPosition(TimeSpan playbackPosition, double sliderFullScale, TimeSpan MinPosition, TimeSpan MaxPosition)
        {
            if ((MaxPosition - MinPosition).Ticks == 0)
                return 0;

            if (playbackPosition == TimeSpan.Zero)
            {
                LoggerControl.Log("Transient playbackPosition == TimeSpan.Zero", LogViewLoggingLevel.Error);
            }

            var interpolatedPosition = (playbackPosition - MinPosition).Ticks * sliderFullScale / (MaxPosition - MinPosition).Ticks;

            if (interpolatedPosition < 0)
            {
                interpolatedPosition = 0;
                LoggerControl.Log("Transient calculatedPosition < 0", LogViewLoggingLevel.Error);
            }

            return interpolatedPosition;
        }

        #endregion


        #region DiscreteControls

        private void SeekFixedAmount_Click(object sender, RoutedEventArgs e)
        {
            TimeSpan amount = TimeSpan.Zero;
            string selected = (sender as Button).Tag.ToString();
            TimeSpan.TryParse(selected, out amount);
            if (amount != TimeSpan.Zero)
            {
                var requestedPosition = mediaPlayerElement.MediaPlayer.PlaybackSession.Position + amount;
                SeekToRequestedPosition(requestedPosition);
            }
        }

        private void Play_Click(object sender, RoutedEventArgs e)
        {
          if(mediaPlayerElement.MediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Paused)
            {
                mediaPlayerElement.MediaPlayer.Play();
            }
        }

        private void Pause_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayerElement.MediaPlayer.PlaybackSession.PlaybackState == MediaPlaybackState.Playing)
            {
                mediaPlayerElement.MediaPlayer.Pause();
            }
        }

        private void GoToStart_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayerElement.MediaPlayer != null)
            {
                var seekableRanges = mediaPlayerElement.MediaPlayer.PlaybackSession.GetSeekableRanges();
                if (seekableRanges.Count > 0)
                {
                    UpdateSeekableRangesControl(seekableRanges);
                    var requestedPosition = seekableRanges[0].Start;
                    SeekToRequestedPosition(requestedPosition);
                }
            }
        }

        private void GoToLive_Click(object sender, RoutedEventArgs e)
        {
            if (mediaPlayerElement.MediaPlayer != null)
            {
                var seekableRanges = mediaPlayerElement.MediaPlayer.PlaybackSession.GetSeekableRanges();
                if (seekableRanges.Count > 0)
                {
                    UpdateSeekableRangesControl(seekableRanges);
                    var requestedPosition = seekableRanges[seekableRanges.Count-1].End;
                    SeekToRequestedPosition(requestedPosition);
                }
            }
        }

        private void RegisterForDiscreteControls(MediaPlayer mediaPlayer)
        {
            // PlaybackSession Events
            mediaPlayer.PlaybackSession.PlaybackStateChanged += MediaPlayer_PlaybackSession_PlaybackStateChanged;
        }

        private void UnregisterForDiscreteControls(MediaPlayer mediaPlayer)
        {
            if (mediaPlayer == null)
            {
                return;
            }

            // PlaybackSession Events
            mediaPlayer.PlaybackSession.PlaybackStateChanged -= MediaPlayer_PlaybackSession_PlaybackStateChanged;
        }

        private async void MediaPlayer_PlaybackSession_PlaybackStateChanged(MediaPlaybackSession sender, object args)
        {
            Log($"PlaybackSession_PlaybackStateChanged, PlaybackState: {sender.PlaybackState}");

            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                switch (sender.PlaybackState)
                {
                    case MediaPlaybackState.None:
                    case MediaPlaybackState.Opening:
                    case MediaPlaybackState.Buffering:
                        PlayButton.IsEnabled = false;
                        PauseButton.IsEnabled = false;
                        break;
                    case MediaPlaybackState.Playing:
                        PlayButton.IsEnabled = false;
                        PauseButton.IsEnabled = true;
                        break;
                    case MediaPlaybackState.Paused:
                        PlayButton.IsEnabled = true;
                        PauseButton.IsEnabled = false;
                        break;
                    default:
                        break;
                }
            });
        }

        private void LogCurrentTimeCorrelation_Click(object sender, RoutedEventArgs e)
        {
            var adaptiveMediaSource = (mediaPlayerElement.MediaPlayer.Source as MediaPlaybackItem)?.Source?.AdaptiveMediaSource;
            if (adaptiveMediaSource == null)
                return;
            var times = adaptiveMediaSource.GetCorrelatedTimes();
            Log(times.ToJsonObject().ToString());
        }
        #endregion

        #endregion // CustomMediaControl

        #region Utilities
        private void Log(string message)
        {
            LoggerControl.Log(message);
        }
        MediaPlayerLogger mediaPlayerLogger;
        MediaSourceLogger mediaSourceLogger;
        MediaPlaybackItemLogger mediaPlaybackItemLogger;
        AdaptiveMediaSourceLogger adaptiveMediaSourceLogger;
        #endregion

    }

}
