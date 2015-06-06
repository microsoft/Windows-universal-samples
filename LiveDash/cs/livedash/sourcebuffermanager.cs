using LiveDash.Models;
using LiveDash.Util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Media.Core;
using Windows.Storage.Streams;
using Windows.Web.Http;

namespace LiveDash
{
    /// <summary>
    /// Handles the source buffer for each stream of data
    /// </summary>
    public class SourceBufferManager
    {
        protected string mimeType;
        protected MediaStream stream;
        protected MseStreamSource mse;
        protected MseSourceBuffer sb;
        protected Task<IBuffer> activeDownload = null;
        protected bool isEndOfStream = false;
        protected bool isInitialized = false;
        protected bool internalAppending = false;
        protected int segmentCursor = 0;
        protected bool isFirstSegment;

        protected TimeSpan MaxGap = TimeSpan.FromSeconds(0.1); // seconds
        protected TimeSpan MaxBuffering = TimeSpan.FromSeconds(5.0); // seconds

        internal SourceBufferManager(string mimeType, MediaStream stream)
        {
            this.mimeType = mimeType;
            this.stream = stream;
            
        }

        #region properties
        public bool IsEndOfStream
        {
            get { return isEndOfStream; }

            set { isEndOfStream = value; }
        }

        public bool Appending
        {
            get { return sb != null ? (sb.IsUpdating || internalAppending) : false; }
        }
        

        public bool IsVideo
        {
            get { return mimeType == Manifest.VideoMimeType; }
        }

        public bool IsAudio
        {
            get { return mimeType == Manifest.AudioMimeType; }
        }

        public MseSourceBuffer MseSourceBuffer
        {
            get { return sb; }
        }

        #endregion

        internal void Initialize(MseStreamSource mse)
        {
            this.mse = mse;
            sb = this.mse.AddSourceBuffer(mimeType);
        }

        internal virtual void UpdateData(TimeSpan now)
        {
            if (sb != null)
            {
                SendInitSegment(now);
                CleanBuffer(now);
                SendMediaSegment(now);
            }
        }

        internal void SendInitSegment(TimeSpan now)
        {
            if (isInitialized || Appending)
            {
                return;
            }
            
            if (stream.InitSegmentData == null)
            {
                if (!stream.isInitSegmentDownloading)
                {
                    stream.isInitSegmentDownloading = true;
#if DEBUG
                    Logger.Log("Requesting init url " + stream.InitSegmentUrl);
#endif
                    ((Task<IBuffer>)Downloader.DownloadBufferAsync(new Uri(stream.InitSegmentUrl))).ContinueWith(
                        task =>
                        {
                            stream.InitSegmentData = task.Result;
                            stream.isInitSegmentDownloading = false;
                            if (task.Result != null)
                            {
#if DEBUG
                                Logger.Log("Init URL downloaded");
#endif
                                UpdateData(now);
                            }
                        });
                }
                return;
            }
            isEndOfStream = false;
#if DEBUG
            Logger.Log("Appending init segment");
#endif
            try
            {
                sb.AppendBuffer(stream.InitSegmentData);
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Append init segment exception HRESULT 0x" + e.HResult.ToString("x"));
#endif
            }

            isInitialized = true;
        }

        internal void CleanBuffer(TimeSpan now)
        {
            try
            {
                if (isEndOfStream || Appending)
                {
                    return;
                }
                var buffering = sb.Buffered;

                if (buffering.Count > 0)
                {
                    // Keep 5 seconds
                    if (now - TimeSpan.FromSeconds(5) > buffering[0].Start)
                    {
                        sb.Remove(buffering[0].Start, now - TimeSpan.FromSeconds(5));
                    }
                }
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Exception occured when cleaning buffer: " + e.Message + Logger.Display(e));
#endif
            }
        }

        protected virtual void SendMediaSegment(TimeSpan now)
        {
            var end = now;
            if (!isInitialized || Appending || isEndOfStream)
            {
                return;
            }

            if (segmentCursor >= this.stream.Segments.Count)
            {
                isEndOfStream = true;
                return;
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

            var stream = this.stream;
            var segment = stream.Segments.ElementAt(segmentCursor);
            ulong endAdjustedToTimescale = (ulong)end.TotalMilliseconds * stream.SegmentInformation.Timescale / 1000;
            if (segment.Timestamp + segment.Duration <= endAdjustedToTimescale)
            {
                segmentCursor++;
                SendMediaSegment(now);
            }
            if (now + MaxBuffering <= end)
            {
                return;
            }

            internalAppending = true;
            string url = stream.Segments.ElementAt(segmentCursor).SegmentUrl;
#if DEBUG
            Logger.Log("Loading segment url " + url);
#endif

            var activeDownload = Downloader.DownloadBufferAsync(new Uri(url));
            if (activeDownload == null)
            {
                sb.Abort();
                isEndOfStream = true;
                return;
            }
            this.activeDownload = activeDownload;
            activeDownload.ContinueWith(
                 task =>
                 {
                     internalAppending = false;
                     if (task.Result != null)
                     {
                         isEndOfStream = false;
                         try
                         {
                             sb.AppendBuffer(task.Result);
                         }
                         catch (Exception e)
                         {
#if DEBUG
                             Logger.Log("Append segment exception HRESULT 0x" + e.HResult.ToString("x"));
#endif
                         }
                         segmentCursor++;
                     }
                     else
                     {
                         if (task.Status != TaskStatus.Canceled)
                         {
                             // Failed - should send EndOfStream
                             sb.Abort();
                             isEndOfStream = true;
                         }
                     }
                     this.activeDownload = null;
                 });
        }

        internal void Close()
        {
            AbortDownload();
            isEndOfStream = true;
            sb = null;
        }

        private void AbortDownload()
        {
            if (activeDownload != null)
            {
                if (activeDownload.Status == TaskStatus.RanToCompletion)
                {
                    ((IAsyncOperationWithProgress<IBuffer, HttpProgress>)activeDownload).Cancel();
                }
            }
        }
    }
}
