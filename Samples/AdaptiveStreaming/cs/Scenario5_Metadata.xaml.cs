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
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

using StringBuilder = System.Text.StringBuilder;

namespace SDKTemplate
{
    /// See the README.md for discussion of this scenario.
    ///
    /// Note: We register but do not unregister event handlers in this scenario, see the EventHandler
    ///       scenario for patterns that can be used to clean up.
    public sealed partial class Scenario5_Metadata : Page
    {
        public Scenario5_Metadata()
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

        private async void Page_OnLoaded(object sender, RoutedEventArgs e)
        {
            var mediaPlayer = new MediaPlayer();
            mediaPlayer.AutoPlay = true;
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

            await LoadSourceFromUriAsync(uri);
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

                // Register for events before resolving the MediaSource.
                RegisterForAdaptiveMediaSourceEvents(adaptiveMS);

                MediaSource source = MediaSource.CreateFromAdaptiveMediaSource(adaptiveMS);

                // You can save additional information in the CustomPropertySet for future retrieval.
                // Note: MediaSource.CustomProperties is a ValueSet and therefore can store
                // only serializable types.

                // Save the original Uri.
                source.CustomProperties.Add("uri", uri.ToString());

                // You're likely to put a content tracking id into the CustomProperties.
                source.CustomProperties.Add("contentId", Guid.NewGuid());

                RegisterForMediaSourceEvents(source);

                // Register for events before resolving the MediaSource.
                mpItem = new MediaPlaybackItem(source);
                RegisterForMediaPlaybackItemEvents(mpItem);
            }
            else
            {
                Log($"Error creating the AdaptiveMediaSource: {result.Status}");
            }
        }
        #endregion

        #region MediaPlayer Event Handlers
        private void RegisterForMediaPlayerEvents(MediaPlayer mediaPlayer)
        {
            // Player Events
            // See the EventHandlers scenario for details on when to unregister the event handlers.
            mediaPlayer.SourceChanged += (sender, args) => Log("mediaPlayer.SourceChanged");
            mediaPlayer.MediaOpened += (sender, args) => Log($"MediaPlayer_MediaOpened, Duration: {sender.PlaybackSession.NaturalDuration}");
            mediaPlayer.MediaEnded += (sender, args) => Log("mediaPlayer.MediaEnded");
            mediaPlayer.MediaFailed += (sender, args) => Log($"MediaPlayer_MediaFailed Error: {args.Error}, ErrorMessage: {args.ErrorMessage}, ExtendedErrorCode.Message: {args.ExtendedErrorCode.Message}");
        }
        #endregion

        #region MediaSource Event Handlers
        private void RegisterForMediaSourceEvents(MediaSource source)
        {
            source.StateChanged += (sender, args) => Log($"Source.StateChanged:{args.OldState} to {args.NewState}");
            source.OpenOperationCompleted += Source_OpenOperationCompleted;
        }

        private void Source_OpenOperationCompleted(MediaSource source, MediaSourceOpenOperationCompletedEventArgs args)
        {
            // Here we create our own metadata track.
            // Our goal is to track progress through the asset and report it back to a server.
            var contentId = (Guid)source.CustomProperties["contentId"];
            var reportingUriFormat = "http://myserver/reporting/{0}?id={1}";
            //From \Shared\TrackingEventCue.cs :
            TrackingEventCue.CreateTrackingEventsTrack(source, contentId.ToString(), reportingUriFormat);
        }

        #endregion

        #region MediaPlaybackItem Event Handlers

        private void RegisterForMediaPlaybackItemEvents(MediaPlaybackItem item)
        {
            // If we encountered some unknown tags in an m3u8 manifest,
            // we might already have metadata tracks in the source,
            // which is used as the constructor for the MediaPlaybackItem,
            // therefore, the MediaPlaybackItem may already have metadata tracks.
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
            var contentId = (Guid)mediaPlaybackItem.Source.CustomProperties["contentId"];

            StringBuilder logMsg = new StringBuilder($"{contentId} ");

            if (timedTrack.TrackKind == MediaTrackKind.TimedMetadata &&
                (timedTrack.TimedMetadataKind == TimedMetadataKind.Data ||
                timedTrack.TimedMetadataKind == TimedMetadataKind.Custom))
            {
                // Check for known dispatchTypes:
                var dispatchType = mediaPlaybackItem.TimedMetadataTracks[index].DispatchType;
                if (String.Equals(dispatchType, "EXTM3U", StringComparison.OrdinalIgnoreCase))
                {
                    timedTrack.CueEntered += metadata_extm3u_CueEntered;
                    timedTrack.Label = "HLS Manifest comments";
                    // Tell the platform that the app will render these.
                    // Without this, the CueEntered event will not be raised by the platform.
                    mediaPlaybackItem.TimedMetadataTracks.SetPresentationMode((uint)index, TimedMetadataTrackPresentationMode.ApplicationPresented);
                }
                else if (string.Equals(dispatchType, "15260DFFFF49443320FF49443320000F", StringComparison.OrdinalIgnoreCase)) // per Apple TS spec for ID3 meta
                {
                    timedTrack.CueEntered += metadata_id3_CueEntered;
                    timedTrack.Label = "ID3 Tags";
                    mediaPlaybackItem.TimedMetadataTracks.SetPresentationMode((uint)index, TimedMetadataTrackPresentationMode.ApplicationPresented);
                }

                // Check for our Custom TimedMetadataTrack
                if (mediaPlaybackItem.TimedMetadataTracks[index].Id == "TrackingEvents")
                {
                    timedTrack.CueEntered += metadata_TrackingEvents_CueEntered;
                    timedTrack.CueExited += metadata_TrackingEvents_CueExited;
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

        private void metadata_extm3u_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();

            var contentId = (Guid)timedMetadataTrack.PlaybackItem.Source.CustomProperties["contentId"];
            logMsg.AppendLine($"{contentId} M3U8 DataCue CueEntered raised.");
            var dataCue = args.Cue as DataCue;
            if (dataCue != null && dataCue.Data != null)
            {
                // The payload is a UTF-16 Little Endian null-terminated string.
                // It is any comment line in a manifest that is not part of the HLS spec.
                var dr = DataReader.FromBuffer(dataCue.Data);
                dr.UnicodeEncoding = UnicodeEncoding.Utf16LE;
                var m3u8Comment = dr.ReadString(dataCue.Data.Length / 2 - 1);
                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {dataCue.Id}, StartTime: {dataCue.StartTime}, Duration: {dataCue.Duration}");
                logMsg.Append(m3u8Comment);
                // TODO: Use the m3u8Comment string.
            }
            Log(logMsg.ToString());
        }

        private void metadata_id3_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();
            var contentId = (Guid)timedMetadataTrack.PlaybackItem.Source.CustomProperties["contentId"];
            logMsg.AppendLine($"{contentId} ID3 DataCue CueEntered raised.");
            var dataCue = args.Cue as DataCue;
            if (dataCue != null && dataCue.Data != null && dataCue.Data.Length >= 10)
            {
                // The payload is the raw ID3 bytes found in a TS stream
                // Ref: http://id3.org/id3v2.4.0-structure
                var dr = DataReader.FromBuffer(dataCue.Data);
                var header_ID3 = dr.ReadString(3);
                var header_version_major = dr.ReadByte();
                var header_version_minor = dr.ReadByte();
                var header_flags = dr.ReadByte();
                var header_tagSize = dr.ReadUInt32();
                // TODO: Use the ID3 bytes.

                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {dataCue.Id}, StartTime: {dataCue.StartTime}, Duration: {dataCue.Duration}");
                logMsg.Append($"{header_ID3}, {header_version_major}.{header_version_minor}, {header_flags}, {header_tagSize}");
            }
            Log(logMsg.ToString());
        }

        private void metadata_TrackingEvents_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();
            var contentId = (Guid)timedMetadataTrack.PlaybackItem.Source.CustomProperties["contentId"];
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

        private void metadata_TrackingEvents_CueExited(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();
            var contentId = (Guid)timedMetadataTrack.PlaybackItem.Source.CustomProperties["contentId"];
            logMsg.AppendLine($"{contentId} TrackingEventCue CueExited raised.");
            var trackingEventCue = args.Cue as TrackingEventCue;
            if (trackingEventCue != null)
            {
                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {trackingEventCue.Id}, StartTime: {trackingEventCue.StartTime}, Duration: {trackingEventCue.Duration}");
                logMsg.Append($"{trackingEventCue.TrackingEventUri}");
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
