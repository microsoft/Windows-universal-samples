using LiveDash.Util;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Windows.Data.Xml.Dom;
using Windows.Storage.Streams;

namespace LiveDash.Models
{
    /// <summary>
    /// Represents a DASH manifest of type .mpd
    /// </summary>
    public class Manifest
    {
        private bool isLive;
        private bool isSupportedProfile; //only support urn:mpeg:dash:profile:isoff-live:2011 
        private DateTimeOffset availabilityStartTime; //anchor for computation of earliest availibility time for any segment
        private TimeSpan minBufferTime; //amount of data to buffer before start of playback
        private TimeSpan minimumUpdatePeriod; //amount of time to elapse before reloading a new manifest (for dynamic content only)
        private TimeSpan timeShiftBufferDepth; //for live playback, specifies duration of the smallist time shifting buffer
        private DateTimeOffset publishTime; //for the publish time of a new manifest
        private TimeSpan mediaPresentationDuration;
        private TimeSpan lastPeriodDuration;
        private List<Period> periods;
        private string sourceUrl;
        private string baseUrl;
        private bool hasPresentationDuration;
        private bool hasMinimumUpdatePeriod;
        private bool hasMultiplePeriods;
        private bool treatAsOnDemand;

        public static string VideoMimeType = "video/mp4";
        public static string AudioMimeType = "audio/mp4";
        public static double MinMinimumUpdatePeriod = 3; //3 seconds
        

        internal Manifest(string url)
        {
            periods = new List<Period>();
            sourceUrl = url;
            //trim the filename of the url
            int index = url.LastIndexOf("/");
            int lastChunkLength = url.Length - index;
            int baseUrlEndIndex = url.Length - lastChunkLength;
            baseUrl = url.Substring(0, baseUrlEndIndex);
        }

        #region properties
        internal string BaseUrl
        {
            get { return baseUrl; }
            set { baseUrl = value; }
        }

        internal string SourceUrl
        {
            get { return sourceUrl; }
        }

        internal bool IsLive
        {
            get { return isLive; }

            set { isLive = value; }
        }

        internal bool IsSupportedProfile
        {
            get { return isSupportedProfile; }

            set { isSupportedProfile = value; }
        }

        internal DateTimeOffset AvailibilityStartTime
        {
            get { return availabilityStartTime; }

            set { availabilityStartTime = value; }
        }

        internal TimeSpan MinBufferTime
        {
            get { return minBufferTime; }

            set { minBufferTime = value; }
        }

        internal TimeSpan MinimumUpdatePeriod
        {
            get { return minimumUpdatePeriod; }

            set { minimumUpdatePeriod = value; }
        }

        internal bool HasMinimumUpdatePeriod
        {
            get { return hasMinimumUpdatePeriod; }

            set { hasMinimumUpdatePeriod = value; }
        }

        internal TimeSpan TimeShiftBufferDepth
        {
            get { return (timeShiftBufferDepth.TotalMilliseconds == 0) ? TimeSpan.MinValue : timeShiftBufferDepth; }

            set { timeShiftBufferDepth = value; }
        }

        internal TimeSpan MediaPresentationDuration
        {
            get { return mediaPresentationDuration; }

            set { mediaPresentationDuration = value; }
        }

        internal TimeSpan LastPeriodDuration
        {
            get { return lastPeriodDuration; }

            set { lastPeriodDuration = value; }
        }

        internal bool HasMultiplePeriods
        {
            get { return hasMultiplePeriods; }

            set { hasMultiplePeriods = value; }
        }

        internal bool HasPresentationDuration
        {
            get { return hasPresentationDuration; }

            set { hasPresentationDuration = value; }
        }

        public DateTimeOffset PublishTime
        {
            get { return publishTime; }

            set { publishTime = value; }
        }

        public bool TreatAsOnDemand
        {
            get { return treatAsOnDemand; }

            set { treatAsOnDemand = value; }
        }

        #endregion

        internal void AddPeriod(Period period)
        {
            periods.Add(period);
        }

        internal Period GetSelectedPeriod()
        {
            foreach (var period in periods)
            {
                if (period.IsSelected)
                {
                    return period;
                }
            }
            return null;
        }

        internal async Task<XmlDocument> LoadManifestAsync(string url)
        {
            Uri uri = null;
            IBuffer buffer = null;
            try
            {
                uri = new Uri(url);
            }
            catch
            {
#if DEBUG
                Logger.Log("Bad url: " + url);
#endif
                throw new Exception("Invalid URL");
            }

            buffer = await Downloader.DownloadBufferAsync(uri);

            if (buffer == null)
            {
#if DEBUG
                Logger.Log("Failed to get buffer of manifest");
#endif
                throw new Exception("Failed to get buffer from url");
            }
            XmlDocument document = new XmlDocument();

            try 
            {

                var bufferString = DataReader.FromBuffer(buffer).ReadString(buffer.Length);
                document.LoadXmlFromBuffer(buffer);
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Failed to load XML from buffer = " + Logger.Display(e));
#endif
                throw new Exception("Invalid XML file");
            }

            return document;
        }

        internal void AppendToBaseUrl(string url)
        {
            if (!baseUrl.EndsWith("/"))
            {
                baseUrl += "/";
            }

            baseUrl += url;
        }

        internal void SetFirstSelectedPeriod()
        {
            if (periods.Count() > 0)
            {
                periods.First<Period>().IsSelected = true;
            }
        }

        internal void SetNextSelectedPeriod()
        {
            var periodCount = periods.Count();
            for (int x = 0; x < periodCount; x++)
            {
                if (periods.ElementAt(x).IsSelected && (x + 1) < periodCount)
                {
                    periods.ElementAt(x).IsSelected = false;
                    periods.ElementAt(x + 1).IsSelected = true;
                    return;
                }
            }
            HasMultiplePeriods = false;
            
        }
    }


}
