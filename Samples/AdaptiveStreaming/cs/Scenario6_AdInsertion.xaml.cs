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
using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation.Collections;
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

    public sealed partial class Scenario6_AdInsertion : Page
    {
        public Scenario6_AdInsertion()
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

                UnregisterHandlers(mediaPlayer);

                mediaPlayer.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mediaPlayer.Dispose();
            }
        }

        private void UnregisterHandlers(MediaPlayer mediaPlayer)
        {
            AdaptiveMediaSource adaptiveMediaSource = null;
            MediaPlaybackItem mpItem = mediaPlayer.Source as MediaPlaybackItem;
            if (mpItem != null)
            {
                adaptiveMediaSource = mpItem.Source.AdaptiveMediaSource;
                UnregisterForMediaSourceEvents(mpItem.Source);
                UnregisterForMediaPlaybackItemEvents(mpItem);
            }
            MediaSource source = mediaPlayer.Source as MediaSource;
            if (source != null)
            {
                adaptiveMediaSource = source.AdaptiveMediaSource;
                UnregisterForMediaSourceEvents(source);
            }

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

            // Ensure we have PlayReady support, in case the user enters a DASH/PR Uri in the text box.
            var prHelper = new PlayReadyHelper(LoggerControl);
            prHelper.SetUpProtectionManager(mediaPlayer);
            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            ContentSelectorControl.Initialize(
                mediaPlayer,
                MainPage.ContentManagementSystemStub.Where(m => !m.Aes),
                null,
                LoggerControl,
                LoadSourceFromUriAsync);
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
            source.CustomProperties["contentId"] = "MainContent_ID";

            // In addition to logging, this scenario uses MediaSource events:
            RegisterForMediaSourceEvents(source);

            var mpItem = new MediaPlaybackItem(source);

            // We use a helper class that logs all the events for the MediaPlaybackItem:
            mediaPlaybackItemLogger = new MediaPlaybackItemLogger(LoggerControl, mpItem);

            // In addition to logging, this scenario uses MediaPlaybackItem events:
            RegisterForMediaPlaybackItemEvents(mpItem);

            // Now that we should have a MediaPlaybackItem, we will insert pre- mid- and post-roll ads
            // with the MediaBreak API.
            CreateMediaBreaksForItem(mpItem);

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

        private void CreateMediaBreaksForItem(MediaPlaybackItem item)
        {
            if (item != null)
            {
                // We have two ads that will be repeated.
                var redSkyUri = new Uri("http://az29176.vo.msecnd.net/videocontent/RedSky_FoxRiverWisc_GettyImagesRF-499617760_1080_HD_EN-US.mp4");
                var flowersUri = new Uri("http://az29176.vo.msecnd.net/videocontent/CrocusTL_FramePoolRM_688-580-676_1080_HD_EN-US.mp4");

                // One option is to create a separate MediaPlaybackItem for each of your ads.
                // You might choose to do this if each ad needs different reporting information.
                // Another option is to re-use MediaPlaybackItems in different MediaBreaks.
                // This scenario demonstrates both patterns.
                var ad1 = new MediaPlaybackItem(MediaSource.CreateFromUri(redSkyUri));
                ad1.Source.CustomProperties["contentId"] = "Ad1_ID";
                ad1.Source.CustomProperties["description"] = "Red Sky";
                ad1.Source.CustomProperties["uri"] = redSkyUri.ToString();
                RegisterForMediaSourceEvents(ad1.Source);
                RegisterForMediaPlaybackItemEvents(ad1);

                var ad2 = new MediaPlaybackItem(MediaSource.CreateFromUri(flowersUri));
                ad2.Source.CustomProperties["contentId"] = "Ad2_ID";
                ad2.Source.CustomProperties["description"] = "Flowers";
                ad2.Source.CustomProperties["uri"] = flowersUri.ToString();
                RegisterForMediaSourceEvents(ad2.Source);
                RegisterForMediaPlaybackItemEvents(ad2);

                var ad3 = new MediaPlaybackItem(MediaSource.CreateFromUri(redSkyUri));
                ad3.Source.CustomProperties["contentId"] = "Ad3_ID";
                ad3.Source.CustomProperties["description"] = "Red Sky 2";
                ad3.Source.CustomProperties["uri"] = redSkyUri.ToString();
                RegisterForMediaSourceEvents(ad3.Source);
                RegisterForMediaPlaybackItemEvents(ad3);

                // Create a PrerollBreak on your main content.
                if (item.BreakSchedule.PrerollBreak == null)
                {
                    item.BreakSchedule.PrerollBreak = new MediaBreak(MediaBreakInsertionMethod.Interrupt);
                }

                // Add the ads to the PrerollBreak in the order you want them played
                item.BreakSchedule.PrerollBreak.PlaybackList.Items.Add(ad1);
                item.BreakSchedule.PrerollBreak.PlaybackList.Items.Add(ad2);

                // Add the ads to the MidRoll break at 10% into the main content.
                // To do this, we need to wait until the main MediaPlaybackItem is fully loaded by the player
                // so that we know its Duration. This will happen on MediaSource.OpenOperationCompleted.
                item.Source.OpenOperationCompleted += (sender, args) =>
                {
                    var attachedItem = MediaPlaybackItem.FindFromMediaSource(sender);
                    if (sender.Duration.HasValue)
                    {
                        // For live streaming, the duration will be TimeSpan.MaxValue, which won't work for this scenario,
                        // so we'll assume the total duration is 2 minutes for the purpose of ad insertion.
                        bool isLiveMediaSource = item.Source.AdaptiveMediaSource != null ? item.Source.AdaptiveMediaSource.IsLive : false;
                        long sourceDurationTicks = isLiveMediaSource ? TimeSpan.FromMinutes(2).Ticks : sender.Duration.Value.Ticks;
                        var positionAt10PercentOfMainContent = TimeSpan.FromTicks(sourceDurationTicks / 10);

                        // If the content is live, then the ad break replaces the streaming content.
                        // If the content is not live, then the content pauses for the ad, and then resumes
                        // after the ad is complete.
                        MediaBreakInsertionMethod insertionMethod = isLiveMediaSource ? MediaBreakInsertionMethod.Replace : MediaBreakInsertionMethod.Interrupt;
                        var midRollBreak = new MediaBreak(insertionMethod, positionAt10PercentOfMainContent);
                        midRollBreak.PlaybackList.Items.Add(ad2);
                        midRollBreak.PlaybackList.Items.Add(ad1);
                        attachedItem.BreakSchedule.InsertMidrollBreak(midRollBreak);
                        Log($"Added MidRoll at {positionAt10PercentOfMainContent}");
                    }
                };

                // Create a PostrollBreak:
                // Note: for Live content, it will only play once the presentation transitions to VOD.
                if (item.BreakSchedule.PostrollBreak == null)
                {
                    item.BreakSchedule.PostrollBreak = new MediaBreak(MediaBreakInsertionMethod.Interrupt);
                }
                // Add the ads to the PostrollBreak in the order you want them played
                item.BreakSchedule.PostrollBreak.PlaybackList.Items.Add(ad3);
            }
        }
        #endregion


        #region MediaSource Event Handlers

        private void RegisterForMediaSourceEvents(MediaSource source)
        {
            source.OpenOperationCompleted += Source_OpenOperationCompleted;
        }
        private void UnregisterForMediaSourceEvents(MediaSource source)
        {
            source.OpenOperationCompleted -= Source_OpenOperationCompleted;
        }

        private void Source_OpenOperationCompleted(MediaSource source, MediaSourceOpenOperationCompletedEventArgs args)
        {
            // Here we create our own metadata track.
            // Our goal is to track progress through the asset and report it back to a server.
            object customValue = null;
            source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;
            var reportingUriFormat = "http://myserver/reporting/{0}?id={1}";
            // From \Shared\TrackingEventCue.cs:
            TrackingEventCue.CreateTrackingEventsTrack(source, contentId, reportingUriFormat);
        }

        #endregion


        #region MediaPlaybackItem Event Handlers

        private void RegisterForMediaPlaybackItemEvents(MediaPlaybackItem item)
        {
            // If we encountered some unknown tags in an m3u8 manifest,
            // we might already have metadata tracks in the source,
            // which is used as the constructor for the MediaPlaybackItem.
            // Therefore, the MediaPlaybackItem may already have metadata tracks.
            // Since TracksChanged will not be raised for these it is best to check
            // if we have tracks, and set up the handlers:
            if (item.TimedMetadataTracks.Count > 0)
            {
                Log($"Registering existing TimedMetadataTracks, Count: {item.TimedMetadataTracks.Count}");

                for (int index = 0; index < item.TimedMetadataTracks.Count; index++)
                {
                    RegisterMetadataHandlers(item, index);
                }
            }

            // For any tracks found as the content is parsed, we register for the event handler:
            item.TimedMetadataTracksChanged += Item_TimedMetadataTracksChanged;
        }

        private void UnregisterForMediaPlaybackItemEvents(MediaPlaybackItem item)
        {
            item.TimedMetadataTracksChanged -= Item_TimedMetadataTracksChanged;
            foreach (var track in item.TimedMetadataTracks)
            {
                UnregisterMetadataHandlers(track);
            }
        }

        private void Item_TimedMetadataTracksChanged(MediaPlaybackItem item, IVectorChangedEventArgs args)
        {
            Log($"item.TimedMetadataTracksChanged: CollectionChange:{args.CollectionChange} Index:{args.Index} Total:{item.TimedMetadataTracks.Count}");

            if (args.CollectionChange == CollectionChange.ItemInserted)
            {
                RegisterMetadataHandlers(item, (int)args.Index);
            }
            if (args.CollectionChange == CollectionChange.Reset)
            {
                for (int index = 0; index < item.TimedMetadataTracks.Count; index++)
                {
                    RegisterMetadataHandlers(item, index);
                }
            }
        }

        private void RegisterMetadataHandlers(MediaPlaybackItem mediaPlaybackItem, int index)
        {
            var timedTrack = mediaPlaybackItem.TimedMetadataTracks[index];
            object customValue = null;
            mediaPlaybackItem.Source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;

            StringBuilder logMsg = new StringBuilder($"{contentId} ");

            if (timedTrack.TrackKind == MediaTrackKind.TimedMetadata &&
                timedTrack.TimedMetadataKind == TimedMetadataKind.Custom)
            {
                // Check for our Custom TimedMetadataTrack
                if (mediaPlaybackItem.TimedMetadataTracks[index].Id == "TrackingEvents")
                {
                    timedTrack.CueEntered += metadata_TrackingEvents_CueEntered;
                    timedTrack.Label = "Tracking Events";
                    mediaPlaybackItem.TimedMetadataTracks.SetPresentationMode((uint)index, TimedMetadataTrackPresentationMode.ApplicationPresented);
                }

                logMsg.AppendLine($"Registered CueEntered for {timedTrack.Label}.");
            }
            else
            {
                logMsg.AppendLine("Did not register CueEntered for TimedMetadataTrack.");
            }
            logMsg.AppendLine($"TimedMetadataKind: {timedTrack.TimedMetadataKind}, Id: {timedTrack.Id}, Label: {timedTrack.Label}, DispatchType: {timedTrack.DispatchType}, Language: {timedTrack.Language}");

            Log(logMsg.ToString());
        }

        private void UnregisterMetadataHandlers(TimedMetadataTrack timedTrack)
        {
            if (timedTrack.Label == "Tracking Events")
            {
                timedTrack.CueEntered -= metadata_TrackingEvents_CueEntered;
            }
        }

        private void metadata_TrackingEvents_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();
            object customValue = null;
            timedMetadataTrack.PlaybackItem.Source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;
            logMsg.AppendLine($"{contentId} TrackingEventCue CueEntered raised.");
            var trackingEventCue = args.Cue as TrackingEventCue;
            if (trackingEventCue != null)
            {
                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {trackingEventCue.Id}, StartTime: {trackingEventCue.StartTime}, Duration: {trackingEventCue.Duration}");
                logMsg.Append($"{trackingEventCue.TrackingEventUri}");
                // TODO: Use the reporing Uri.
            }
            Log(logMsg.ToString());
        }
        #endregion


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
