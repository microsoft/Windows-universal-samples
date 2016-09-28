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
using System.Threading.Tasks;
using Windows.Foundation.Collections;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.Streaming.Adaptive;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

using StringBuilder = System.Text.StringBuilder;

namespace SDKTemplate
{
    /// See the README.md for discussion of this scenario.
    ///
    /// Note: We register but do not unregister event handlers in this scenario.
    /// See the EventHandler scenario for patterns that can be used to clean up.
    public sealed partial class Scenario6_AdInsertion : Page
    {
        public Scenario6_AdInsertion()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            mpItem = null;
            var mp = mediaPlayerElement.MediaPlayer;
            if (mp != null)
            {
                mp.DisposeSource();
                mediaPlayerElement.SetMediaPlayer(null);
                mp.Dispose();
            }
        }

        private AdaptiveContentModel adaptiveContentModel;
        private MediaPlaybackItem mpItem;
        private bool isLiveMediaSource = false;

        private async void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            var mediaPlayer = new MediaPlayer();
            RegisterForMediaPlayerEvents(mediaPlayer);

            // Ensure we have PlayReady support, in case the user enters a DASH/PR Uri in the text box.
            var prHelper = new PlayReadyHelper(LoggerControl);
            prHelper.SetUpProtectionManager(mediaPlayer);

            mediaPlayerElement.SetMediaPlayer(mediaPlayer);

            // Choose a default content.
            SelectedContent.ItemsSource = MainPage.ContentManagementSystemStub;
            adaptiveContentModel = MainPage.FindContentById(1);
            SelectedContent.SelectedItem = adaptiveContentModel;

            UriBox.Text = adaptiveContentModel.ManifestUri.ToString();
            await LoadSourceFromUriAsync(adaptiveContentModel.ManifestUri);
        }

        #region Content Loading

        private async void LoadId_Click(object sender, RoutedEventArgs e)
        {
            adaptiveContentModel = (AdaptiveContentModel)SelectedContent.SelectedItem;

            UriBox.Text = adaptiveContentModel.ManifestUri.ToString();
            await LoadSourceFromUriAsync(adaptiveContentModel.ManifestUri);
        }

        private async void LoadUri_Click(object sender, RoutedEventArgs e)
        {
            Uri uri;
            if (!Uri.TryCreate(UriBox.Text, UriKind.Absolute, out uri))
            {
                Log("Malformed Uri in Load text box.");
            }

            mpItem = null;
            await LoadSourceFromUriAsync(uri);
        }

        private void AutoPlay_Checked(object sender, RoutedEventArgs e)
        {
            mediaPlayerElement.AutoPlay = (bool)(sender as CheckBox).IsChecked;
        }

        private void SetSource_Click(object sender, RoutedEventArgs e)
        {
            // It is at this point that the MediaSource (within a MediaPlaybackItem) will be fully resolved.
            // For an AdaptiveMediaSource, this is the point at which media is first requested and parsed.
            mediaPlayerElement.Source = mpItem;
        }

        private async Task LoadSourceFromUriAsync(Uri uri, HttpClient httpClient = null)
        {
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
            if (result.Status == AdaptiveMediaSourceCreationStatus.Success)
            {
                var adaptiveMS = result.MediaSource;
                isLiveMediaSource = adaptiveMS.IsLive;
                RegisterForAdaptiveMediaSourceEvents(adaptiveMS);

                MediaSource source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMS);
                source.CustomProperties.Add("uri", uri.ToString());
                source.CustomProperties.Add("contentId", "MainContent_ID");
                RegisterForMediaSourceEvents(source);

                mpItem = new MediaPlaybackItem(source);
                RegisterForMediaPlaybackItemEvents(mpItem);
            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource: {result.Status}");
            }

            // Now that we should have a MediaPlaybackItem, we will insert pre- mid- and post-roll ads
            // with the MediaBreak API.
            if (mpItem != null)
            {
                CreateMediaBreaksForItem(mpItem);
            }
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

        #region MediaPlayer Event Handlers
        private void RegisterForMediaPlayerEvents(MediaPlayer mediaPlayer)
        {
            // Player Events
            mediaPlayer.SourceChanged += (sender, args) => Log("mediaPlayer.SourceChanged");
            mediaPlayer.MediaOpened += (sender, args) => Log($"MediaPlayer_MediaOpened, Duration: {sender.PlaybackSession.NaturalDuration}");
            mediaPlayer.MediaEnded += (sender, args) => Log("mediaPlayer.MediaEnded");
            mediaPlayer.MediaFailed += (sender, args) => Log($"MediaPlayer_MediaFailed Error: {args.Error}, ErrorMessage: {args.ErrorMessage}, ExtendedErrorCode.Message: {args.ExtendedErrorCode.Message}");
        }
        #endregion

        #region MediaSource Event Handlers
        private void RegisterForMediaSourceEvents(MediaSource source)
        {
            if (source != null)
            {
                source.StateChanged += (sender, args) => Log($"Source.StateChanged:{args.OldState} to {args.NewState}");
                source.OpenOperationCompleted += Source_OpenOperationCompleted;
            }
            else
            {
                Log("ERROR: Tried RegisterForMediaSourceEvents with a null source");
            }
        }

        private void Source_OpenOperationCompleted(MediaSource source, MediaSourceOpenOperationCompletedEventArgs args)
        {
            // Here we create our own metadata track.
            // Our goal is to track progress through the asset and report it back to a server.
            var contentId = (string)source.CustomProperties["contentId"];
            var reportingUriFormat = "http://myserver/reporting/{0}?id={1}";
            //From \Shared\TrackingEventCue.cs :
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
            item.TimedMetadataTracksChanged += (MediaPlaybackItem sender, IVectorChangedEventArgs args) =>
            {
                Log($"item.TimedMetadataTracksChanged: CollectionChange:{args.CollectionChange} Index:{args.Index} Total:{sender.TimedMetadataTracks.Count}");

                if (args.CollectionChange == CollectionChange.ItemInserted)
                {
                    RegisterMetadataHandlers(sender, (int)args.Index);
                }
                if (args.CollectionChange == CollectionChange.Reset)
                {
                    for (int index = 0; index < item.TimedMetadataTracks.Count; index++)
                    {
                        RegisterMetadataHandlers(item, index);
                    }
                }
            };
        }


        private void RegisterMetadataHandlers(MediaPlaybackItem mediaPlaybackItem, int index)
        {
            var timedTrack = mediaPlaybackItem.TimedMetadataTracks[index];
            var contentId = (string)mediaPlaybackItem.Source.CustomProperties["contentId"];

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
                logMsg.AppendLine($"Did not register CueEntered for TimedMetadataTrack.");
            }
            logMsg.AppendLine($"TimedMetadataKind: {timedTrack.TimedMetadataKind}, Id: {timedTrack.Id}, Label: {timedTrack.Label}, DispatchType: {timedTrack.DispatchType}, Language: {timedTrack.Language}");

            Log(logMsg.ToString());
        }

         private void metadata_TrackingEvents_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();

            var contentId = (string)timedMetadataTrack.PlaybackItem.Source.CustomProperties["contentId"];
            logMsg.AppendLine("{contentId} TrackingEventCue CueEntered raised.");
            var trackingEventCue = args.Cue as TrackingEventCue;
            if (trackingEventCue != null)
            {
                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {trackingEventCue.Id}, StartTime: {trackingEventCue.StartTime}, Duration: {trackingEventCue.Duration}");
                logMsg.Append($"{trackingEventCue.TrackingEventUri}");
                // TODO: Use the reporting Uri.
            }
            Log(logMsg.ToString());
        }
        #endregion

        #region AdaptiveMediaSource Event Handlers
        private void RegisterForAdaptiveMediaSourceEvents(AdaptiveMediaSource aMS)
        {
            aMS.DownloadFailed += DownloadFailed;
        }

        private void DownloadFailed(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadFailedEventArgs args)
        {
            Log($"DownloadFailed: {args.HttpResponseMessage}, {args.ResourceType}, {args.ResourceUri}");
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
