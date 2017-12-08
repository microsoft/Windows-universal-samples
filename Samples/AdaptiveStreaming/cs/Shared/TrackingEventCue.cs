using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Core;

namespace SDKTemplate
{
    /// <summary>
    /// The custom data that can be created, scheduled then consumed as part of a TimedMetadataTrack which has been added by the app.
    /// 
    /// In these samples, it creates a set of cues at the start, 1/4, 1/2, 3/4 and upon completion of playback.  
    /// For each of these points, it will fire an MediaCue with the custom TrackingEventUri property to be consumed by
    /// the app in the CueEntered event handler of the TimedMetadataTrack.
    /// 
    /// We also demonstrate the use of the .Duration IMediaCue property, which will fire CueExited for the midpoint Cue at the 55% mark.
    /// </summary>
    public class TrackingEventCue : IMediaCue
    {
        public TimeSpan Duration { get; set; }
        public string Id { get; set; }
        public TimeSpan StartTime { get; set; }
        public Uri TrackingEventUri { get; set; }

        /// <summary>
        /// Creates a TimedMetadataTrack on the source
        /// </summary>
        /// <param name="source">the source on which to create a TimedMetadataTrack</param>
        /// <param name="contentId">the content id for this source</param>
        /// <param name="reportingUriFormat">the format string used to create reporting Uri for each event</param>
        public static void CreateTrackingEventsTrack(MediaSource source, string contentId, string reportingUriFormat)
        {
            if (source.Duration.HasValue && !string.IsNullOrEmpty(contentId))
            {
                long sourceDurationTicks = source.Duration.Value.Ticks;
                sourceDurationTicks = AdjustDurationIfLive(sourceDurationTicks);

                var trackingEvents = new TimedMetadataTrack("TrackingEvents", "", TimedMetadataKind.Custom);
                trackingEvents.AddCue(new TrackingEventCue()
                {
                    // This method is called once the media has loaded. If it is set to AutoPlay, then
                    // we may miss Position=0.0. Avoid the race by "starting" at the 2-second mark.
                    StartTime = TimeSpan.FromSeconds(2),
                    Id = "2s",
                    TrackingEventUri = new Uri(string.Format(reportingUriFormat, "started", contentId)),
                });
                trackingEvents.AddCue(new TrackingEventCue()
                {
                    StartTime = TimeSpan.FromTicks((long)Math.Round(sourceDurationTicks * 0.25)),
                    Id = "0.25",
                    TrackingEventUri = new Uri(string.Format(reportingUriFormat, "firstQuartile", contentId)),
                });
                trackingEvents.AddCue(new TrackingEventCue()
                {
                    StartTime = TimeSpan.FromTicks((long)Math.Round(sourceDurationTicks * 0.50)),
                    Id = "0.50",
                    TrackingEventUri = new Uri(string.Format(reportingUriFormat, "midpoint", contentId)),
                    // We can cause CueExited to fire at 55% by setting a Duration:
                    Duration = TimeSpan.FromTicks((long)Math.Round(sourceDurationTicks * 0.05)),
                });
                trackingEvents.AddCue(new TrackingEventCue()
                {
                    StartTime = TimeSpan.FromTicks((long)Math.Round(sourceDurationTicks * 0.75)),
                    Id = "0.75",
                    TrackingEventUri = new Uri(string.Format(reportingUriFormat, "thirdQuartile", contentId)),
                });
                trackingEvents.AddCue(new TrackingEventCue()
                {
                    StartTime = TimeSpan.FromTicks(sourceDurationTicks),
                    Id = "1.00",
                    TrackingEventUri = new Uri(string.Format(reportingUriFormat, "complete", contentId)),
                });
                source.ExternalTimedMetadataTracks.Add(trackingEvents);
            }
        }

        public static long AdjustDurationIfLive(long sourceDurationTicks)
        {
            // Live sources will have an duration of TimeSpan.MaxValue,
            // so assume they are 2 minutes long for these scenarios.
            if (sourceDurationTicks == TimeSpan.MaxValue.Ticks)
            {
                sourceDurationTicks = TimeSpan.FromMinutes(2).Ticks;
            }
            return sourceDurationTicks;
        }
    }
}
