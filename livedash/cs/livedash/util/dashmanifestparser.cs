using LiveDash.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Windows.Data.Xml.Dom;
using System.Text.RegularExpressions;

namespace LiveDash.Util
{
    public class DashManifestParser
    {
        public const string LiveProfile = "urn:mpeg:dash:profile:isoff-live:2011";
        private const string HttpPrefix = "http://";
        private const string HttpsPrefix = "https://";
        public const int LiveSegmentCount = int.MaxValue / 2;
        private XmlDocument document;
        private Manifest manifest;

        private bool hasPeriodStartAttribute;
        private TimeSpan periodStart;
        public DashManifestParser(XmlDocument document, ref Manifest manifest)
        {
            this.document = document;
            this.manifest = manifest;
        }
        public bool Parse()
        {
            try
            {
#if DEBUG
                Logger.Log("Begin parsing DASH manifest");
#endif
                ParseMPDElement();
                UpdateBaseUrl();

                var periods = document.DocumentElement.ChildNodes.Where(node => node.NodeName.Equals("Period"));
#if DEBUG
                Logger.Log(periods.Count().ToString() + " Periods found");
#endif
                foreach (var period in periods)
                {
                    Period dashPeriod = new Period();
                    periodStart = TimeSpan.FromSeconds(0);
                    ParsePeriodElement(period, ref dashPeriod);

                    var adaptationSets = period.ChildNodes.Where(node => node.NodeName.Equals("AdaptationSet"));
#if DEBUG
                    Logger.Log(adaptationSets.Count().ToString() + " Adaptation Sets found");
#endif
                    foreach (var adaptationSet in adaptationSets)
                    {
                        MediaStream stream = new MediaStream();

                        if (adaptationSet.Attributes.GetNamedItem("mimeType") != null)
                        {
                            stream.MimeType = adaptationSet.Attributes.GetNamedItem("mimeType").InnerText;
                        }
                        if (adaptationSet.Attributes.GetNamedItem("lang") != null)
                        {
                            stream.Language = adaptationSet.Attributes.GetNamedItem("lang").InnerText.Replace(" ", string.Empty);
                        }

                        var representations = adaptationSet.ChildNodes.Where(node => node.NodeName.Equals("Representation"));
                        //We are only grabbing one Representation in an Adaptation Set (the lowest one, just to be safe)
                        //(No support for adaptive streaming)
                        IXmlNode representation = GetLowestBitrateRepresentation(representations);
#if DEBUG
                        Logger.Log(representations.Count().ToString() + " Representations in " + stream.MimeType + " Adaptation Set with bitrate " + uint.Parse(representation.Attributes.GetNamedItem("bandwidth").InnerText));
#endif

                        //Parse Representation Element
                        uint bitrate;
                        string repID = string.Empty;
                        ParseRepresentationElement(representation,
                            out bitrate,
                            out repID);

                        var segmentTemplateQuery = adaptationSet.ChildNodes.Where(node => node.NodeName.Equals("SegmentTemplate"));
                        var segmentListQuery = adaptationSet.ChildNodes.Where(node => node.NodeName.Equals("SegmentList"));
                        var segmentBaseQuery = adaptationSet.ChildNodes.Where(node => node.NodeName.Equals("SegmentBase"));
                        if (segmentTemplateQuery.Count() != 0)
                        {
                            stream.SegmentInfoType = MediaStream.SegmentInformationType.Template;
                            //Parse Segment Template Element
                            UInt64 timescale = 1;
                            string initializationFormat = string.Empty;
                            string mediaFormat = string.Empty;
                            UInt64 segmentDuration = 0;
                            uint startNumber = 1;
                            UInt64 presentationTimeOffset = 0;
                            var segmentTemplate = segmentTemplateQuery.First();

                            ParseSegmentTemplateElement(segmentTemplate,
                                ref timescale, ref initializationFormat,
                                ref mediaFormat, ref segmentDuration,
                                ref startNumber, ref presentationTimeOffset);

                            DeterminePeriodStart(presentationTimeOffset, period, ref dashPeriod);

                            string initializationUrl;
                            //$Number$ and $Time$ will never be present in the initialization URL
                            ExpandDASHUrlSegmentTemplate(initializationFormat, manifest.BaseUrl,
                                bitrate, repID, 0, 0, out initializationUrl);
                            stream.InitSegmentUrl = initializationUrl;
                            
                            var segmentTimelineQuery = segmentTemplate.ChildNodes.Where(node => node.NodeName.Equals("SegmentTimeline"));
                            
                            MediaSegmentInformation segmentInfo = new MediaSegmentInformation();
                            segmentInfo.Timescale = timescale;
                            segmentInfo.RepresentationID = repID;
                            segmentInfo.Bitrate = bitrate;

                            //No timeline present.
                            //Instead of populating the segments in the stream, we will give just enough information to produce
                            //the information to construct media segment urls on the fly in the SourceBufferManager
                            if (segmentTimelineQuery.Count() == 0)
                            {
#if DEBUG
                                Logger.Log("Making number base segment info from SegmentTemplate");
#endif
                                stream.SegmentTemplateBase = MediaStream.SegmentTemplateIdentifier.Number;

                                segmentInfo.Duration = segmentDuration;
                                segmentInfo.UrlTemplate = mediaFormat;

                                UInt64 segmentNumber = startNumber;
                                if (manifest.IsLive)
                                {
                                    var hnsFragmentDuration = ((segmentDuration * 10000000) / timescale);
                                    CalculateNumberIdentifierForLive(hnsFragmentDuration, ref segmentNumber);
                                }
                                segmentInfo.StartNumber = segmentNumber;

                                segmentInfo.StartTimestamp = segmentDuration * (segmentNumber - 1);

                                stream.CanGenerateSegmentsDynamically = true;
                            }
                            else
                            {
                                stream.SegmentTemplateBase = MediaStream.SegmentTemplateIdentifier.Time;

                                MakeTimelineInfo(segmentTimelineQuery.First(), bitrate, timescale,
                                    repID, startNumber, presentationTimeOffset,
                                    mediaFormat, ref stream);
                            }

                            stream.SegmentInformation = segmentInfo;
                        }
                        else if (segmentListQuery.Count() != 0)
                        {
#if DEBUG
                            Logger.Log("Manifest parsing error: " + "Does not support a SegmentList implementation");
#endif
                            stream.SegmentInfoType = MediaStream.SegmentInformationType.List;
                        }
                        else
                        {
#if DEBUG
                            Logger.Log("Manifest parsing error: " + "Does not support a SegmentBase or BaseUrl implementation");
#endif
                            stream.SegmentInfoType = MediaStream.SegmentInformationType.Base;
                        }

                        
                        dashPeriod.AddStream(stream);
                    } // each adaptation set

                    manifest.AddPeriod(dashPeriod);
                }// each period
                manifest.SetFirstSelectedPeriod();
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Manifest parsing error " + Logger.Display(e));
#endif
                return false;
            }
            return true;
        }

        private void DeterminePeriodStart(ulong presentationTimeOffset, IXmlNode periodNode, ref Period period)
        {
            if (hasPeriodStartAttribute)
            {
                return;
            }
        }

        private IXmlNode GetLowestBitrateRepresentation(IEnumerable<IXmlNode> representations)
        {
            uint minBitrate = uint.MaxValue;
            IXmlNode minBitrateRepresentation = null;
            foreach (var representation in representations)
            {
                var bandwidthNode = representation.Attributes.GetNamedItem("bandwidth");
                if (bandwidthNode != null)
                {
                    uint bitrate = uint.Parse(bandwidthNode.InnerText);
                    if (bitrate < minBitrate)
                    {
                        minBitrateRepresentation = representation;
                        minBitrate = bitrate;
                    }
                }
            }
            return minBitrateRepresentation;
        }

        private void ParseMPDElement()
        {
            XmlElement root = document.DocumentElement;
            if (root.GetAttribute("type").ToLower().Equals("dynamic"))
            {
                manifest.IsLive = true;
            }
            if (root.GetAttribute("profiles").ToLower().Contains(LiveProfile))
            {
                manifest.IsSupportedProfile = true;
            }
            if (!root.GetAttribute("availabilityStartTime").Equals(""))
            {
                string availabilityStartTime = root.GetAttribute("availabilityStartTime");
                manifest.AvailibilityStartTime = XmlConvert.ToDateTimeOffset(availabilityStartTime);
            }
            if (!root.GetAttribute("minBufferTime").Equals(""))
            {
                string minBufferTime = root.GetAttribute("minBufferTime");
                manifest.MinBufferTime = XmlConvert.ToTimeSpan(minBufferTime);
            }
            if (!root.GetAttribute("minimumUpdatePeriod").Equals(""))
            {
                manifest.HasMinimumUpdatePeriod = true;
                string minUpdatePeriod = root.GetAttribute("minimumUpdatePeriod");
                manifest.MinimumUpdatePeriod = XmlConvert.ToTimeSpan(minUpdatePeriod);
            }
            if (!root.GetAttribute("mediaPresentationDuration").Equals(""))
            {
                manifest.HasPresentationDuration = true;
                string mediaPresentationDuration = root.GetAttribute("mediaPresentationDuration");
                manifest.MediaPresentationDuration = XmlConvert.ToTimeSpan(mediaPresentationDuration);
            }
            if (!root.GetAttribute("timeShiftBufferDepth").Equals(""))
            {
                string timeShiftBufferDepth = root.GetAttribute("timeShiftBufferDepth");
                manifest.TimeShiftBufferDepth = XmlConvert.ToTimeSpan(timeShiftBufferDepth);
            }
            if (!root.GetAttribute("publishTime").Equals(""))
            {
                string publishTime = root.GetAttribute("publishTime");
                manifest.PublishTime = XmlConvert.ToDateTimeOffset(publishTime);
            }
            else
            {
                manifest.PublishTime = DateTimeOffset.MinValue;
            }
            XmlNodeList periods = root.GetElementsByTagName("Period");
            if (periods.Count() > 1)
            {
                manifest.HasMultiplePeriods = true;
            }

            IXmlNode lastPeriod = periods.Last<IXmlNode>();
            if (lastPeriod.Attributes.GetNamedItem("duration") != null)
            {
                manifest.LastPeriodDuration = XmlConvert.ToTimeSpan(lastPeriod.Attributes.GetNamedItem("duration").InnerText);
            }
        }

        private void ParsePeriodElement(IXmlNode periodElement, ref Period period)
        {
            if (periodElement.Attributes.GetNamedItem("id") != null)
            {
                period.Id = periodElement.Attributes.GetNamedItem("id").InnerText;
            }
            if (periodElement.Attributes.GetNamedItem("duration") != null)
            {
                string periodDuration = periodElement.Attributes.GetNamedItem("duration").InnerText;
                period.Duration = XmlConvert.ToTimeSpan(periodDuration);
            }
            if (periodElement.Attributes.GetNamedItem("start") != null)
            {
                string periodStart = periodElement.Attributes.GetNamedItem("start").InnerText;
                period.Start = XmlConvert.ToTimeSpan(periodStart);
                this.periodStart = period.Start;
                hasPeriodStartAttribute = true;
            }
        }

        private void ParseRepresentationElement(IXmlNode representaion, out uint bitrate, out string repID)
        {
            repID = string.Empty;
            if (representaion.Attributes.GetNamedItem("id") != null)
            {
                repID = representaion.Attributes.GetNamedItem("id").InnerText;
            }

            bitrate = 0;
            if (representaion.Attributes.GetNamedItem("bandwidth") != null)
            {
                string bandwidthText = representaion.Attributes.GetNamedItem("bandwidth").InnerText;
                bitrate = XmlConvert.ToUInt32(bandwidthText);
            }
        }

        private void ParseSegmentTemplateElement(IXmlNode segmentTemplate,
            ref UInt64 timescale, ref string initializationFormat,
            ref string mediaFormat, ref UInt64 duration,
            ref uint startNumber, ref UInt64 presentationTimeOffset)
        {
            if (segmentTemplate.Attributes.GetNamedItem("duration") != null)
            {
                string durationText = segmentTemplate.Attributes.GetNamedItem("duration").InnerText;
                duration = XmlConvert.ToUInt64(durationText);
            }
            if (segmentTemplate.Attributes.GetNamedItem("initialization") != null)
            {
                initializationFormat = segmentTemplate.Attributes.GetNamedItem("initialization").InnerText;
            }
            if (segmentTemplate.Attributes.GetNamedItem("media") != null)
            {
                mediaFormat = segmentTemplate.Attributes.GetNamedItem("media").InnerText;
            }
            if (segmentTemplate.Attributes.GetNamedItem("startNumber") != null)
            {
                string startNumberText = segmentTemplate.Attributes.GetNamedItem("startNumber").InnerText;
                startNumber = XmlConvert.ToUInt32(startNumberText);
            }
            if (segmentTemplate.Attributes.GetNamedItem("timescale") != null)
            {
                string timescaleText = segmentTemplate.Attributes.GetNamedItem("timescale").InnerText;
                timescale = XmlConvert.ToUInt64(timescaleText);
            }
            if (segmentTemplate.Attributes.GetNamedItem("presentationTimeOffset") != null)
            {
                string presentationTimeOffsetText = segmentTemplate.Attributes.GetNamedItem("presentationTimeOffset").InnerText;
                presentationTimeOffset = XmlConvert.ToUInt64(presentationTimeOffsetText);
            }
        }

        static internal void ExpandDASHUrlSegmentTemplate(string format, string baseUrl, uint bitrate, string repID, ulong number, ulong time, out string result)
        {
            result = string.Empty;
            //Use the base url if the template url is not based already
            if (format.StartsWith(HttpPrefix) || format.StartsWith(HttpsPrefix))
            {
                result += format;
                
            }
            else 
            {
                result += baseUrl;

                //Ensure there's one slash (and only one) 
                //seperating the base from the template url
                if (!result.EndsWith("/") && !result.EndsWith("&")
                    && !result.EndsWith(";"))
                {
                    result += '/';
                }

                result += format;

            }
            
            result = result.Replace("$Bandwidth$", bitrate.ToString());
            result = result.Replace("$Number$", number.ToString());
            result = result.Replace("$Time$", time.ToString());
            result = result.Replace("$RepresentationID$", repID.ToString());
        }

        private static string ResolvePadding(string template, string idMatch, ulong value)
        {
            Regex paddingRegex = new Regex("(%\\d+d)+");

            var paddingMatch = paddingRegex.Match(idMatch);

            string temp = String.Format(paddingMatch.Value, value);

            temp = template.Replace(idMatch, temp);

            return temp;
        }

        private void MakeNumberBaseSegmentInfo(uint bitrate, UInt64 timescale,
            UInt64 segmentDuration, string repID,
            uint startNumber, string mediaFormat, ref MediaStream stream)
        {
            var hnsPresentationDuration = (Convert.ToUInt64(manifest.MediaPresentationDuration.TotalMilliseconds) * Convert.ToUInt64(10000));
            var hnsFragmentDuration = ((segmentDuration * 10000000) / timescale);
            UInt64 count = (uint)(hnsPresentationDuration / hnsFragmentDuration);
            UInt64 remain = (uint)(hnsPresentationDuration - (hnsFragmentDuration * count));

            if (manifest.IsLive && count == 0)
            {
                if (manifest.TimeShiftBufferDepth.TotalSeconds > 0 && hnsFragmentDuration > 0)
                {
                    var hnsTimeShiftBufferDepth = (ulong)(manifest.TimeShiftBufferDepth.TotalSeconds * 10000000);
                    count = hnsTimeShiftBufferDepth / hnsFragmentDuration;
                }
                else
                {
                    count = LiveSegmentCount;
                }
            }

            UInt64 time = 0;

            UInt64 segmentNumber = startNumber;
            if (manifest.IsLive)
            {
                CalculateNumberIdentifierForLive(hnsFragmentDuration, ref segmentNumber);
            }
            //Since we calculated the latest segment number, we will add to the front of the list
            for (UInt64 i = 0; i < count; i++)
            {
                MediaSegment segment = new MediaSegment();
                segment.Timestamp = time;
                segment.Duration = segmentDuration;
                segment.Number = segmentNumber;

                //Construct the mediaURL
                string mediaSegmentUrl;
                ExpandDASHUrlSegmentTemplate(mediaFormat, manifest.BaseUrl, bitrate,
                    repID, segmentNumber, time, out mediaSegmentUrl);
                segment.SegmentUrl = mediaSegmentUrl;

                stream.PushBackSegment(segment);
                segmentNumber++;
                time += segmentDuration;
                
            }

            if (remain > 0)
            {
                MediaSegment segment = new MediaSegment();
                segment.Timestamp = time;
                segment.Duration = remain / timescale;
                segment.Number = segmentNumber;

                string mediaSegmentUrl;
                ExpandDASHUrlSegmentTemplate(mediaFormat, manifest.BaseUrl, bitrate,
                    repID, segmentNumber, time, out mediaSegmentUrl);
                segment.SegmentUrl = mediaSegmentUrl;

                stream.PushBackSegment(segment);

#if DEBUG
                Logger.Log("Adding final segment to stream with MimeType:" + stream.MimeType + ". Segment{ Timestamp: " + time + "Duration: "
                        + segmentDuration + "Number: " + startNumber + "}"
                        + "\nURL: " + segment.SegmentUrl);
#endif
                time += segmentDuration;
            }
        }

        private void CalculateNumberIdentifierForLive(ulong hnsFragmentDuration, ref ulong segmentNumber)
        {
            //This formula is to get the latest segment number.
            DateTimeOffset availStartTime = manifest.AvailibilityStartTime;
            DateTimeOffset now = GetWallClockTime();

            if (manifest.HasPresentationDuration)
            {
                var endOfPresentation = availStartTime.DateTime.AddSeconds(manifest.MediaPresentationDuration.TotalSeconds);


                if (endOfPresentation < now.DateTime)
                {
                    manifest.TreatAsOnDemand = true;
#if DEBUG
                    Logger.Log("The time now is after the end of the presentation. Attempt to start from the beginning " + segmentNumber);
#endif
                    return;
                }
            }

            TimeSpan timeSpanElapsed = now.DateTime - availStartTime.DateTime;
            var hnsTimeElapsed = (ulong)(timeSpanElapsed.TotalMilliseconds * 10000);
            hnsTimeElapsed -= (ulong)(hnsFragmentDuration * 1.5);
            var num = hnsTimeElapsed / hnsFragmentDuration;
#if DEBUG
            Logger.Log("Number for Live Manifest " + num);
#endif
            segmentNumber += num;
        }

        private DateTimeOffset GetWallClockTime()
        {
            //CHANGE HOW TO RETRIEVE THE WALL CLOCK IN THIS LOCATION 
            //If an element is present to retrieve from a server, use
            //the time from the server.
            return DateTimeOffset.UtcNow;
        }

        private void MakeTimelineInfo(IXmlNode segmentTimeline, uint bitrate,
            ulong timescale, string repID,
            uint startNumber, ulong presentationTimeOffset,
            string mediaFormat, ref MediaStream stream)
        {
            var timeline = segmentTimeline.ChildNodes.Where(x => x.NodeName == "S");

            uint segmentNumber = startNumber;
            ulong time = 0;
            //Set time equal to the @t attribute of first element, otherwise it is 0.
            //This is to replace the value of $Time$ in the media segment url
            var tAttribute = timeline.First().Attributes.GetNamedItem("t");
            if (tAttribute != null)
            {
                time = ulong.Parse(tAttribute.InnerText);
            }
            foreach (var t in timeline)
            {
                ulong duration = ulong.Parse(t.Attributes.GetNamedItem("d").InnerText);
                var repeatAttribute = t.Attributes.GetNamedItem("r");
                long repeats = 0;
                if (repeatAttribute != null)
                {
                    repeats = long.Parse(repeatAttribute.InnerText);
                }

                for (int i = 0; i < repeats + 1; i++)
                {
                    MediaSegment segment = new MediaSegment();
                    segment.Duration = duration;
                    segment.Timestamp = time - presentationTimeOffset;
                    segment.Number = segmentNumber;

                    //Construct the mediaURL
                    string mediaSegmentUrl;
                    ExpandDASHUrlSegmentTemplate(mediaFormat, manifest.BaseUrl, bitrate,
                        repID, segmentNumber, time, out mediaSegmentUrl);
                    segment.SegmentUrl = mediaSegmentUrl;

                    stream.PushBackSegment(segment);
                    time += duration;
                    segmentNumber++;
                }
            }

            double msStreamDuration = (double)time * (1000) / timescale;
#if DEBUG
            Logger.Log("Found " + stream.Segments.Count() + " segments - Duration: " + msStreamDuration.ToString() + " miliseconds");
#endif

            if (msStreamDuration > manifest.MediaPresentationDuration.TotalMilliseconds)
            {
                manifest.MediaPresentationDuration = TimeSpan.FromMilliseconds(msStreamDuration);
            }
        }

        private void UpdateBaseUrl()
        {
            //This does not support all scenarios for the BaseUrl element.
            //It will find the first BaseUrl 
            //Element in either the MPD element or the Period Element, top down 
            //and append it to the base url of the manifest.
            var mpdBaseUrlElement = document.SelectNodes("*").Where(x => x.NodeName == "BaseURL");
            if (mpdBaseUrlElement.Count() != 0)
            {
                string baseUrlText = mpdBaseUrlElement.First().InnerText;
                if (baseUrlText.ToLower().StartsWith("http") || baseUrlText.ToLower().StartsWith("https"))
                {
                    manifest.BaseUrl = baseUrlText;
                }
                else
                {
                    manifest.AppendToBaseUrl(mpdBaseUrlElement.First().InnerText);
                }
                return;
            }
            var periodBaseUrlElement = document.SelectNodes("*").Where(x => x.NodeName == "BaseURL");
            if (periodBaseUrlElement.Count() != 0)
            {
                string baseUrlText = periodBaseUrlElement.First().InnerText;
                if (baseUrlText.ToLower().StartsWith("http") || baseUrlText.ToLower().StartsWith("https"))
                {
                    manifest.BaseUrl = baseUrlText;
                }
                else
                {
                    manifest.AppendToBaseUrl(periodBaseUrlElement.First().InnerText);
                }
                return;
            }
            
        }

    }
}
