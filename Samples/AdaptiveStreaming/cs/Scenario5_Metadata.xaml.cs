// *********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// *********************************************************

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
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using Windows.Web.Http;

namespace SDKTemplate
{
    // See the README.md for discussion of this scenario.

    public sealed partial class Scenario5_Metadata : Page
    {
        public Scenario5_Metadata()
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

            ContentSelectorControl.SetAutoPlay(true);  // Force AutoPlay in this scenario.
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
            source.CustomProperties["contentId"] = Guid.NewGuid().ToString();

            // In addition to logging, this scenario uses MediaSource events:
            RegisterForMediaSourceEvents(source);

            var mpItem = new MediaPlaybackItem(source);

            // We use a helper class that logs all the events for the MediaPlaybackItem:
            mediaPlaybackItemLogger = new MediaPlaybackItemLogger(LoggerControl, mpItem);

            // In addition to logging, this scenario uses MediaPlaybackItem events:
            RegisterForMediaPlaybackItemEvents(mpItem);

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
            // From \Shared\TrackingEventCue.cs :
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
                else if (string.Equals(dispatchType, "emsg:mp4", StringComparison.OrdinalIgnoreCase)) // For 'emsg' box embedded in mp4 container
                {
                    timedTrack.CueEntered += metadata_emsg_mp4_CueEntered;
                    timedTrack.Label = "emsg payload";
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

        private void UnregisterMetadataHandlers(TimedMetadataTrack timedTrack)
        {
            if (timedTrack.Label == "HLS Manifest comments")
            {
                timedTrack.CueEntered -= metadata_extm3u_CueEntered;
            }
            if(timedTrack.Label == "ID3 Tags")
            {
                timedTrack.CueEntered -= metadata_id3_CueEntered;
            }
            if(timedTrack.Label == "emsg payload")
            {
                timedTrack.CueEntered -= metadata_emsg_mp4_CueEntered;
            }
            if (timedTrack.Label == "Tracking Events")
            {
                timedTrack.CueEntered -= metadata_TrackingEvents_CueEntered;
                timedTrack.CueExited -= metadata_TrackingEvents_CueExited;
            }
        }


        private void metadata_extm3u_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();

            object customValue = null;
            timedMetadataTrack.PlaybackItem.Source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;
            logMsg.AppendLine($"{contentId} M3U8 DataCue CueEntered raised.");
            var dataCue = args.Cue as DataCue;
            if (dataCue != null && dataCue.Data != null)
            {
                // The payload is a UTF-16 Little Endian null-terminated string.
                // It is any comment line in a manifest that is not part of the HLS spec.
                var dr = DataReader.FromBuffer(dataCue.Data);
                dr.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf16LE;
                var m3u8Comment = dr.ReadString(dataCue.Data.Length / 2 - 1);
                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {dataCue.Id}, StartTime: {dataCue.StartTime}, Duration: {dataCue.Duration}");
                logMsg.Append(m3u8Comment);
                // TODO: Use the m3u8Comment string for something useful.
            }
            Log(logMsg.ToString());
        }

        private void metadata_id3_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();
            object customValue = null;
            timedMetadataTrack.PlaybackItem.Source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;
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
                // TODO: Use the ID3 bytes for something useful.

                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {dataCue.Id}, StartTime: {dataCue.StartTime}, Duration: {dataCue.Duration}");
                logMsg.Append($"{header_ID3}, {header_version_major}.{header_version_minor}, {header_flags}, {header_tagSize}");
            }
            Log(logMsg.ToString());
        }

        private UInt32 lastProcessedAdId = 0;
        private void metadata_emsg_mp4_CueEntered(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();
            object customValue = null;
            timedMetadataTrack.PlaybackItem.Source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;
            logMsg.AppendLine($"{contentId} mp4 emsg DataCue CueEntered raised.");
            var dataCue = args.Cue as DataCue;
            if (dataCue != null)
            {
                // Ref: ISO/IEC 20009-1, Section 5.10.3.3.2 Definition of Box Type: 'emsg'
                string scheme_id_uri = string.Empty;
                string value = string.Empty;
                UInt32 timescale = (UInt32) TimeSpan.TicksPerSecond;
                UInt32 presentation_time_delta = (UInt32)dataCue.StartTime.Ticks;
                UInt32 event_duration = (UInt32)dataCue.Duration.Ticks;
                UInt32 id = 0;
                Byte[] message_data = null;

                const string scheme_id_uri_key = "emsg:scheme_id_uri";
                object propValue = null;
                dataCue.Properties.TryGetValue(scheme_id_uri_key, out propValue);
                scheme_id_uri = propValue!=null ? (string)propValue : "";

                const string value_key = "emsg:value";
                propValue = null;
                dataCue.Properties.TryGetValue(value_key, out propValue);
                value = propValue != null ? (string)propValue : "";

                const string timescale_key = "emsg:timescale";
                propValue = null;
                dataCue.Properties.TryGetValue(timescale_key, out propValue);
                timescale = propValue != null ? (UInt32)propValue : timescale;

                const string presentation_time_delta_key = "emsg:presentation_time_delta";
                propValue = null;
                dataCue.Properties.TryGetValue(presentation_time_delta_key, out propValue);
                presentation_time_delta = propValue != null ? (UInt32)propValue : presentation_time_delta;

                const string event_duration_key = "emsg:event_duration";
                propValue = null;
                dataCue.Properties.TryGetValue(event_duration_key, out propValue);
                event_duration = propValue != null ? (UInt32)propValue : event_duration;

                const string id_key = "emsg:id";
                propValue = null;
                dataCue.Properties.TryGetValue(id_key, out propValue);
                id = propValue != null ? (UInt32)propValue : 0;

                logMsg.AppendLine($"Label: {timedMetadataTrack.Label}, Id: {dataCue.Id}, StartTime: {dataCue.StartTime}, Duration: {dataCue.Duration}");
                logMsg.Append($"scheme_id_uri: {scheme_id_uri}, value: {value}, timescale: {timescale}, presentation_time_delta: {presentation_time_delta}, event_duration: {event_duration}, id: {id}");

                if (dataCue.Data != null)
                {
                    var dr = DataReader.FromBuffer(dataCue.Data);

                    // Check if this is a SCTE ad message:
                    // Ref:  http://dashif.org/identifiers/event-schemes/
                    if (scheme_id_uri.ToLower() == "urn:scte:scte35:2013:xml")
                    {
                        // SCTE recommends publishing emsg more than once, so we avoid reprocessing the same message id:
                        if (lastProcessedAdId != id)
                        {
                            lastProcessedAdId = id; // TODO: Requires robustness. Messages may be out of order, which would break this sample's logic.

                            dr.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
                            var scte35payload = dr.ReadString(dataCue.Data.Length);
                            logMsg.Append($", message_data: {scte35payload}");
                            ScheduleAdFromScte35Payload(timedMetadataTrack, presentation_time_delta, timescale, event_duration, scte35payload);
                        }
                        else
                        {
                            logMsg.AppendLine($"This emsg.Id, {id}, has already been processed.");
                        }
                    }
                    else
                    {
                        message_data = new byte[dataCue.Data.Length];
                        dr.ReadBytes(message_data);
                        // TODO: Use the 'emsg' bytes for something useful.
                        logMsg.Append($", message_data.Length: {message_data.Length}");
                    }
                }
            }
            Log(logMsg.ToString());
        }

        /// <summary>
        /// This function attempts to parse the SCTE-35 payload, then schedule an appropriately offset Ad using the MediaBreak APIs
        /// </summary>
        /// <param name="timedMetadataTrack">Track which fired the cue</param>
        /// <param name="presentation_time_delta"></param>
        /// <param name="timescale"></param>
        /// <param name="event_duration"></param>
        /// <param name="scte35payload"></param>
        private async void ScheduleAdFromScte35Payload(TimedMetadataTrack timedMetadataTrack, uint presentation_time_delta, uint timescale, uint event_duration, string scte35payload)
        {
            // TODO: Parse SCTE-35
            //
            // Ref:
            // http://www.scte.org/SCTEDocs/Standards/ANSI_SCTE%20214-3%202015.pdf
            //
            // Eg:
            //
            // <SpliceInfoSection ptsAdjustment="0" scte35:tier="4095">
            //  <SpliceInsert spliceEventId="147467497"
            //                spliceEventCancelIndicator="false"
            //                outOfNetworkIndicator="false"
            //                uniqueProgramId="0"
            //                availNum="0"
            //                availsExpected="0"
            //                spliceImmediateFlag="false" >
            //    <Program>
            //      <SpliceTime ptsTime="6257853600"/>
            //    </Program>
            //    <BreakDuration autoReturn="true" duration="900000"/>
            //  </SpliceInsert>
            // </SpliceInfoSection>
            //
            //

            // We use the metadata track object which fired the Cue to walk our way back up the
            // media object stack to find our original AdaptiveMediaSource.
            //
            // The AdaptiveMediaSource is required because the SCTE-35 payload includes
            // timing offsets which are relative to the original content PTS -- see below.
            var ams = timedMetadataTrack.PlaybackItem.Source.AdaptiveMediaSource;

            if (ams != null && timescale != 0)
            {
                // ++++
                // NOTE: DO NOT PARSE SCTE35 LIKE THIS IN PRODUCTION CODE!
                //
                // Reminder:
                //
                // THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
                // ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
                // IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
                // PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
                //
                // We will not demonstrate proper SCTE35 parsing in this sample,
                // but instead we search through the xml find the ptsTime if present,
                // and use it to schedule an ad; keeping in place the various time offsets.
                //
                // e.g.: var sampleScte35Payload = "<SpliceInfoSection ptsAdjustment=\"0\" scte35:tier=\"4095\">< SpliceInsert spliceEventId = \"147467497\" spliceEventCancelIndicator = \"false\" outOfNetworkIndicator = \"false\" uniqueProgramId = \"0\" availNum = \"0\" availsExpected = \"0\" spliceImmediateFlag = \"false\" > < Program >< SpliceTime ptsTime = \"6257853600\" /></ Program >  < BreakDuration autoReturn = \"true\" duration = \"900000\" />  </ SpliceInsert > </ SpliceInfoSection > ";
                //
                var xmlStrings = scte35payload.Split(new string[] { "<", "/", ">", " ", "=", "\"" }, StringSplitOptions.RemoveEmptyEntries);
                string ptsTime = string.Empty;
                for (int i = 0; i < xmlStrings.Length; i++)
                {
                    if (xmlStrings[i] == "ptsTime")
                    {
                        if (i + 1 < xmlStrings.Length)
                        {
                            ptsTime = xmlStrings[i + 1];
                            break;
                        }
                    }
                }
                // ----

                // The AdaptiveMediaSource keeps track of the original PTS in an AdaptiveMediaSourceCorrelatedTimes
                // object that can be retrieved via ams.GetCorrelatedTimes().  All the while, it provids the
                // media pipeline with a consistent timeline that re-aligns zero at the begining  of the SeekableRange
                // when first joining a Live stream.
                long pts = 0;
                long.TryParse(ptsTime, out pts);
                var timeCorrelation = ams.GetCorrelatedTimes();
                if (timeCorrelation.Position.HasValue && timeCorrelation.PresentationTimeStamp.HasValue)
                {
                    TimeSpan currentPosition;
                    await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        currentPosition = mediaPlayerElement.MediaPlayer.PlaybackSession.Position;
                    });
                    TimeSpan emsgPresentationTimeDelta = TimeSpan.FromSeconds(presentation_time_delta/timescale);
                    long delayInTicks = timeCorrelation.PresentationTimeStamp.Value.Ticks - pts;
                    TimeSpan correctionForAsyncCalls = currentPosition - timeCorrelation.Position.Value;
                    TimeSpan targetAdPosition = emsgPresentationTimeDelta + TimeSpan.FromTicks(delayInTicks) + correctionForAsyncCalls;

                    Log($"Timing Info: PlaybackSession.Position:{currentPosition}  targetAdPosition:{targetAdPosition}  Delta:{targetAdPosition-currentPosition}  Ams.Position:{timeCorrelation.Position.GetValueOrDefault().Ticks}  SCTE ptsTime:{pts}  emsgPresentationTimeDelta:{emsgPresentationTimeDelta} Ams.PresentationTimeStamp:{timeCorrelation.PresentationTimeStamp.GetValueOrDefault().Ticks}  Ams.ProgramDateTime:{timeCorrelation.ProgramDateTime.GetValueOrDefault()}");

                    MediaBreakInsertionMethod insertionMethod = ams.IsLive ? MediaBreakInsertionMethod.Replace : MediaBreakInsertionMethod.Interrupt;
                    var redSkyUri = new Uri("http://az29176.vo.msecnd.net/videocontent/RedSky_FoxRiverWisc_GettyImagesRF-499617760_1080_HD_EN-US.mp4");
                    if (targetAdPosition != TimeSpan.Zero && targetAdPosition >= currentPosition)
                    {
                        // Schedule ad in the future:
                        Log($"Ad insertion triggerd by 'emsg'.  Scheduled: {targetAdPosition.ToString()} Current:{currentPosition.ToString()}");
                        var midRollBreak = new MediaBreak(insertionMethod, targetAdPosition);
                        midRollBreak.PlaybackList.Items.Add(new MediaPlaybackItem(MediaSource.CreateFromUri(redSkyUri)));
                        timedMetadataTrack.PlaybackItem.BreakSchedule.InsertMidrollBreak(midRollBreak);
                    }
                    else
                    {
                        // Play now!
                        Log($"Ad inserted immediately.  Scheduled: {targetAdPosition.ToString()} Current:{currentPosition.ToString()}");
                        var midRollBreak = new MediaBreak(insertionMethod);
                        midRollBreak.PlaybackList.Items.Add(new MediaPlaybackItem(MediaSource.CreateFromUri(redSkyUri)));
                        await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            mediaPlayerElement.MediaPlayer.BreakManager.PlayBreak(midRollBreak);
                        });
                    }
                }
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
                // TODO: Use the reporing Uri for something useful.
            }
            Log(logMsg.ToString());
        }

        private void metadata_TrackingEvents_CueExited(TimedMetadataTrack timedMetadataTrack, MediaCueEventArgs args)
        {
            StringBuilder logMsg = new StringBuilder();
            object customValue = null;
            timedMetadataTrack.PlaybackItem.Source.CustomProperties.TryGetValue("contentId", out customValue);
            string contentId = (string)customValue;
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
