using LiveDash.Models;
using LiveDash.Util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Web.Http;

namespace LiveDash
{
    /// <summary>
    /// This class extends a source buffer manager but handles cases for 
    /// live playback such as setting a live offset and the logic
    /// to pick the next segment to play
    /// </summary>
    class LiveSourceBufferManager : SourceBufferManager
    {
        private bool determinedStartingSegmentIndex = false;
        private int nextSegmentIndex = 0;
        private Manifest manifest;
        private ulong hnsDesiredLiveOffset = 300000000;

        private ulong nextSegmentNumber;
        private int totalSegmentCount;

        internal LiveSourceBufferManager(string mimeType, MediaStream stream) : base(mimeType, stream)
        {
            isFirstSegment = true;
        }

        #region properties

        internal Manifest Manifest
        {
            get { return manifest; }
            set { manifest = value; }
        }

        internal int NextSegmentIndex
        {
            get { return nextSegmentIndex; }
            set { nextSegmentIndex = value; }
        }

        internal ulong NextSegmentNumber
        {
            get { return nextSegmentNumber; }
            set { nextSegmentNumber = value; }
        }
        
        public ulong DesiredLiveOffset
        {
            get { return hnsDesiredLiveOffset; }

            set { hnsDesiredLiveOffset = value; }
        }
        #endregion

        internal override void UpdateData(TimeSpan now)
        {
            if (sb != null)
            {
                SendInitSegment(now);
                CleanBuffer(now);
                SendMediaSegment(now);
            }
            
        }
        
        protected override async void SendMediaSegment(TimeSpan now)
        {
            try
            {
                var end = now;

                if (!isInitialized || Appending || isEndOfStream)
                {
                    return;
                }

                //if the next segment is out of range from the manifest, then
                //try to redownload the manifest and pick up where we left off.
                //this is the case where a manifest reload didn't come in time, 
                //we don't have a mediaPresentationDuration present, and we cannot generate
                //segments dynamically.the new manifest does not contain the segment, 
                //then it is the end of stream.
                if (manifest.HasPresentationDuration && nextSegmentIndex > totalSegmentCount)
                {
                    //nextSegmentIndex = await GetNewSegmentIndexFromNewStream();

                    //if (nextSegmentIndex == -1)
                    //{
                        isEndOfStream = true;
                        return;
                    //}
                }

                foreach (var range in sb.Buffered)
                {
                    if (range.Start <= end + MaxGap && range.End > end)
                    {
                        end = range.End;
                    }
                    else if (range.Start > end)
                    {
                        break;
                    }
                }

                var stream = base.stream;

                MediaSegment segment = GetNextSegment();
                
                if (now + MaxBuffering <= end)
                {
                    return;
                }
                
                string url = segment.SegmentUrl;
                internalAppending = true;
#if DEBUG
                Logger.Log("Loading segment url " + url);
#endif
                Uri mediaSegmentUrl = null;
                try
                {
                    mediaSegmentUrl = new Uri(url);
                }
                catch (Exception e)
                {
#if DEBUG
                    Logger.Log("malformed media segment: " + url + Logger.Display(e));
#endif
                }

                bool firstDownloadSucceded = await TryDownloadBuffer(mediaSegmentUrl);
                if (firstDownloadSucceded && isFirstSegment)
                {
                    var bufferedRange = sb.Buffered;

                    TimeSpan startingTimestamp = bufferedRange.First().Start;
                    TimeSpan endingTimestamp = bufferedRange.Last().End;

                    if (sb.Buffered.Count != 0)
                    {
                        sb.Remove(TimeSpan.Zero, endingTimestamp);
#if DEBUG
                        Logger.Log("Removing buffered Range from 0 to " + endingTimestamp);
#endif
                    }
                    sb.TimestampOffset = -startingTimestamp;
#if DEBUG
                    Logger.Log("Setting Timestamp to " + sb.TimestampOffset);
#endif

                    sb.AppendBuffer(activeDownload.Result);
                    isFirstSegment = false;
                }
                else if (!firstDownloadSucceded)
                {
                    bool searchSucceeded = await SearchForSegment();
                    if (!searchSucceeded)
                    {
#if DEBUG
                        Logger.Log("Calling end of stream");
#endif
                        // Failed - should send EndOfStream
                        sb.Abort();
                        isEndOfStream = true;

                    }

                }
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Error when sending media segment: " + e.Message);
#endif
            }

        }

        internal void HandleManifestReload()
        {
            if (!stream.CanGenerateSegmentsDynamically)
            {
                for (int x = 0; x < stream.Segments.Count(); x++)
                {
                    if (stream.Segments.ElementAt(x).Number == nextSegmentNumber)
                    {
                        nextSegmentIndex = x;
                        return;
                    }
                }
                determinedStartingSegmentIndex = false;
                DetermineStartingSegmentIndex();
            }
        }

        internal MediaSegment GetNextSegment()
        {
            DetermineStartingSegmentIndex();
            bool isEndOfStream = false;
            if (manifest.HasPresentationDuration)
            {
                if (isFirstSegment)
                {
                    var segmentInfo = stream.SegmentInformation;
                    var presentationDuration = (ulong)manifest.MediaPresentationDuration.TotalMilliseconds * segmentInfo.Timescale / 1000;

                    var segmentDuration = segmentInfo.Duration;

                    totalSegmentCount = (int)presentationDuration / (int)segmentDuration;
                }
            }

            MediaSegment segment;
            if (stream.CanGenerateSegmentsDynamically && !isEndOfStream)
            {
                //create new segment based on media segment info given
                segment = new MediaSegment();
                var segmentInfo = stream.SegmentInformation;

                var format = segmentInfo.UrlTemplate;
                var bitrate = segmentInfo.Bitrate;
                var repID = segmentInfo.RepresentationID;
                var number = segmentInfo.StartNumber + (ulong)nextSegmentIndex;
                var time = segmentInfo.StartTimestamp + (ulong)nextSegmentIndex;
                
                string mediaUrl;
                DashManifestParser.ExpandDASHUrlSegmentTemplate(format, manifest.BaseUrl, bitrate, repID, number, time, out mediaUrl);

                segment.Duration = segmentInfo.Duration;
                segment.Number = number;
                segment.Timestamp = time;
                segment.SegmentUrl = mediaUrl;
                
            }
            else //populated by a list of segments
            {
                segment = stream.Segments.ElementAt(nextSegmentIndex);
            }
            nextSegmentNumber = segment.Number + 1;

            return segment;
        }

        private async Task<bool> ReloadManifest()
        {
            try
            {
                //Redownload manifest to pick up where it left off
                var mpdUrl = manifest.SourceUrl;
                manifest = new Manifest(mpdUrl);
                var document = await manifest.LoadManifestAsync(mpdUrl);

                DashManifestParser mpdParser = new DashManifestParser(document, ref manifest);

                return mpdParser.Parse();
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Error reloading manifest in the Live Source Buffer: " + e.Message);
#endif
                return false;
            }
        }

        private async Task<int> GetNewSegmentIndexFromNewStream()
        {
            //Get latest segment info and find out the next segment number
            if (!stream.CanGenerateSegmentsDynamically)
            {
                var nextSegmentnumber = stream.Segments.Last().Number + 1;

                bool reloadAndParseSucceded = await ReloadManifest();

                if (reloadAndParseSucceded)
                {

                    Period selectedPeriod = manifest.GetSelectedPeriod();
                    //CMediaStream newStream = null;
                    if (IsVideo)
                    {
                        stream = selectedPeriod.GetVideoStream();
                    }
                    else if (IsAudio)
                    {
                        stream = selectedPeriod.GetAudioStream();
                    }
                    else
                    {
                        //Is not an audio or video stream...return -1
                        return -1;
                    }
                    for (int i = stream.Segments.Count() - 1; i >= 0; i--)
                    {
                        if (stream.Segments.ElementAt(i).Number == nextSegmentnumber)
                        {
                            return i;
                        }
                    }
                }
            }
            //The segment where we left off at is not on the server
            return -1;
        }

        private async Task<bool> SearchForSegment()
        {
            try
            {

                if (stream.CanGenerateSegmentsDynamically)
                {
                    //Does not search for now.
                    return false;
                }
                else
                {
                    var segmentList = stream.Segments;
                    int min = 0;
                    int max = (segmentList.Count()) - 1;

                    //five tries forward and backward? 
                    //need to do it in steps of availibilityWindowDuration / 2.
                    ulong increment = ((ulong)max / 2) / 5;

                    int foundIndex = -1;
                    //Forward Search
                    foundIndex = await IterativeSearch(segmentList, nextSegmentIndex, max, increment, true);
                    //Backward Search
                    if (foundIndex == -1)
                    {
                        foundIndex = await IterativeSearch(segmentList, min, nextSegmentIndex, increment, false);
                    }

                    if (foundIndex == -1)
                    {
#if DEBUG
                        Logger.Log("No segment from the manifest was found for stream " + stream.MimeType);
#endif
                        return false;
                    }
                    else
                    {
                        nextSegmentIndex = foundIndex;
                        return true;
                    }

                }
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Exception while searching for available segment: " + Logger.Display(e));
#endif
                return false;
            }
        }

        private async Task<int> IterativeSearch(List<MediaSegment> segmentList, int minIndex, int maxIndex, ulong increment, bool isForwardSearch)
        {
            while (minIndex <= maxIndex)
            {
                int next;
                if (isForwardSearch)
                {
                    next = minIndex + (int)increment;
                }
                else
                {
                    next = maxIndex - (int)increment;
                }

                if (next < minIndex || next > maxIndex)
                {
                    return -1;
                }

                Uri segmentUri = new Uri(segmentList.ElementAt(next).SegmentUrl);
                HttpResponseMessage response = await Downloader.SendHeadRequestAsync(segmentUri);
                
                if (response.IsSuccessStatusCode)
                {
                    bool downloadSucceeded = await TryDownloadBuffer(segmentUri);
                    return next;
                }
                else
                {
                    if (isForwardSearch)
                    {
                        minIndex = next;
                    }
                    else
                    {
                        maxIndex = next;
                    }
                }
            }
            return -1;
        }

        private async Task<bool> TryDownloadBuffer(Uri url)
        {
            bool success = false;
            var activeDownload = Downloader.DownloadBufferAsync(url);
            base.activeDownload = activeDownload;
            await activeDownload.ContinueWith(
            task =>
                 {
                     try
                     {
                         internalAppending = false;
                         bool resultIsPresent = task.Result != null;

                         if (resultIsPresent)
                         {
                             isEndOfStream = false;
                             try
                             {
                                 sb.AppendBuffer(task.Result);
#if DEBUG
                                 Logger.Log("Appended media segment");
#endif
                                 nextSegmentIndex++;
                             }
                             catch (Exception e)
                             {
#if DEBUG
                                 Logger.Log("Append segment exception HRESULT 0x" + e.HResult.ToString("x") + " for the " + mimeType + " source buffer");
#endif
                             }

                             success = true;
                         }
                         else
                         {
                             if (task.Status != TaskStatus.Canceled)
                             {
                                 success = false;
                             }
                         }

                         //m_activeDownload = null;
                     }
                     catch (Exception e)
                     {
#if DEBUG
                         Logger.Log("Error in Trying download HRESULT 0x" + e.HResult.ToString("x") + " for the " + mimeType + " source buffer");
#endif
                     }
                 });
            return success;
        }

        private void DetermineStartingSegmentIndex()
        {
            if (!determinedStartingSegmentIndex && !manifest.TreatAsOnDemand && manifest.IsLive)
            {
                //Can't have a live offset lower than 30 seconds
                if (hnsDesiredLiveOffset < 300000000)
                {
                    hnsDesiredLiveOffset = 300000000;
                }
                nextSegmentIndex = stream.GetStartingSegmentIndexForLive(hnsDesiredLiveOffset);
                determinedStartingSegmentIndex = true;
            }
        }
        

        internal void GoToLiveEdge()
        {
            //m_isFirstSegment = true;
            determinedStartingSegmentIndex = false;
            DetermineStartingSegmentIndex();
        }
        
    }
}
