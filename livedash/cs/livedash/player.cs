using LiveDash.Models;
using LiveDash.Util;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.Web.Http;
using Windows.Storage;

namespace LiveDash
{
    /// <summary>
    /// This class manages the source buffers (audio and video) and takes
    /// care of setting the mse stream source on the media element
    /// </summary>
    public class Player
    {
        private MediaElement mediaElement;
        private MseStreamSource mse;
        private Manifest manifest;
        private List<SourceBufferManager> sourceBuffers;
        private DispatcherTimer playbackTimer;
        private DispatcherTimer manifestReloadTimer;
        private bool opened = false;
        private bool isPaused = false;
        private bool hasLiveOffsetValue;
        private TimeSpan liveOffset;

        public Player(
        MediaElement vid,
        MseStreamSource mse,
        Manifest manifest)
        {
            sourceBuffers = new List<SourceBufferManager>();
            playbackTimer = new DispatcherTimer();
            mediaElement = vid;
            this.mse = mse;
            this.manifest = manifest;

            //Add an audio and video source buffer
            AddAudioAndVideoSourceBuffers(this.manifest.GetSelectedPeriod());
            
            if (this.manifest.IsLive && this.manifest.HasMinimumUpdatePeriod)
            {
                StartManifestReloadTimer();
            }

        }


        #region properties

        public bool HasLiveOffsetValue
        {
            get { return hasLiveOffsetValue; }

            set { hasLiveOffsetValue = value; }
        }

        public TimeSpan LiveOffset
        {
            get { return liveOffset; }

            set { liveOffset = value; }
        }

        #endregion
        private void AddAudioAndVideoSourceBuffers(Period currentPeriod)
        {

            var audioStream = currentPeriod.GetAudioStream();
            if (audioStream != null)
            {
                if (manifest.IsLive)
                {
                    sourceBuffers.Add(new LiveSourceBufferManager(Manifest.AudioMimeType, audioStream));
                    mediaElement.TransportControls.IsSeekEnabled = false;
                }
                else
                {
                    sourceBuffers.Add(new SourceBufferManager(Manifest.AudioMimeType, audioStream));
                }
            }

            var vidStream = currentPeriod.GetVideoStream();
            if (vidStream != null)
            {
                if (manifest.IsLive)
                {
                    sourceBuffers.Add(new LiveSourceBufferManager(Manifest.VideoMimeType, vidStream));
                }
                else
                {
                    sourceBuffers.Add(new SourceBufferManager(Manifest.VideoMimeType, vidStream));
                }
            }
            
        }

        private void StartManifestReloadTimer()
        {
            UpdateLiveSourceBuffers();
            manifestReloadTimer = new DispatcherTimer();

            double minUpdatePeriod = manifest.MinimumUpdatePeriod.TotalSeconds;
            if (minUpdatePeriod < Manifest.MinMinimumUpdatePeriod)
            {
                manifestReloadTimer.Interval = TimeSpan.FromSeconds(Manifest.MinMinimumUpdatePeriod);
            }
            else
            {
                manifestReloadTimer.Interval = manifest.MinimumUpdatePeriod;
            }

            manifestReloadTimer.Tick +=
            async (sender, e) =>
                {
                    await ReloadManifest();
                    CheckForMediaPresentationDuration();
                    UpdateLiveSourceBuffers();
                };
            manifestReloadTimer.Start();
        }

        private void CheckForMediaPresentationDuration()
        {
            //Stop reloading the manifest because we know the presentation is about 
            //to end
            if (manifest.HasPresentationDuration 
                && manifestReloadTimer != null && manifestReloadTimer.IsEnabled)
            {
                manifestReloadTimer.Stop();
            }
        }

        private void UpdateLiveSourceBuffers()
        {
            foreach (var sourceBuffer in sourceBuffers)
            {
                ((LiveSourceBufferManager)sourceBuffer).Manifest = manifest;
                ((LiveSourceBufferManager)sourceBuffer).HandleManifestReload();
            }
        }

        private void UpdateLiveSourceBuffersToLiveEdge()
        {
            foreach (var sourceBuffer in sourceBuffers)
            {
                ((LiveSourceBufferManager)sourceBuffer).Manifest = manifest;
                ((LiveSourceBufferManager)sourceBuffer).GoToLiveEdge();
            }
        }

        private async Task ReloadManifest()
        {
            try
            {
                var sourceUrl = manifest.SourceUrl;
                manifest = new Manifest(sourceUrl);
                var document = await manifest.LoadManifestAsync(sourceUrl);

                DashManifestParser mpdParser = new DashManifestParser(document, ref manifest);
                if (!mpdParser.Parse())
                {
#if DEBUG
                    Logger.Log("The Parser failed to parse this mpd");
#endif
                    return;
                }
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Error occured reloading manifest: " + e.Message);
#endif
            }
        }

        internal void Initialize()
        {
            mediaElement.MediaOpened +=
                (sender1, e1) =>
                {
                    UpdateData(mediaElement.Position);
                };

            ConfigureMse();

            SetLiveOffsetValue();
            
            //  Create our timer - for the web we could use the timerupdate
            //  method on the mediaelement but that's not available in XAML
            playbackTimer.Interval = TimeSpan.FromSeconds(0.2);
            playbackTimer.Tick +=
                (sender, e) =>
                {
                    UpdateData(mediaElement.Position);
                };
            playbackTimer.Start();

            //  Hook up the stream source
            mediaElement.SetMediaStreamSource(mse);

            mediaElement.CurrentStateChanged += M_mediaElement_CurrentStateChanged;
            mediaElement.Play();
        }

        private void M_mediaElement_CurrentStateChanged(object sender, RoutedEventArgs e)
        {
            if (mediaElement.CurrentState == MediaElementState.Paused &&
                playbackTimer.IsEnabled)
            {
                playbackTimer.Stop();
                isPaused = true;

            }
            else if (mediaElement.CurrentState == MediaElementState.Playing &&
                isPaused)
            {
                if (manifest.IsLive)
                {
                    //Do a head request on the current segment to see if it is on the server
                    //if not we will skip to live edge
                    SearchSegmentIfOutsideBuffer();
                }
                playbackTimer.Start();
                isPaused = false;
            }


        }

        private async void SearchSegmentIfOutsideBuffer()
        {
            MediaSegment segment = ((LiveSourceBufferManager)GetVideoBuffer()).GetNextSegment();
            HttpResponseMessage response = await Downloader.SendHeadRequestAsync(new Uri(segment.SegmentUrl));

            if (response.StatusCode == HttpStatusCode.NotFound)
            {
#if DEBUG
                Logger.Log("Trying to go to Live Edge");
#endif
                GoToLiveEdge();
            }
        }

        internal void ConfigureMse()
        {
            mse.Opened += M_mse_Opened;

            mse.Closed += M_mse_Closed;
        }

        private void M_mse_Closed(MseStreamSource sender, object args)
        {
#if DEBUG
            Logger.Log("Closing media source");
#endif
            foreach (var sourceBuffer in sourceBuffers)
            {
                sourceBuffer.Close();
            }

        }

        private void M_mse_Opened(MseStreamSource sender, object args)
        {
#if DEBUG
            Logger.Log("Opened Stream Source");
#endif
            if (opened)
            {
                return;
            }
            opened = true;
            foreach (var sourceBuffer in sourceBuffers)
            {
                if (manifest.IsLive)
                {
                    mse.Duration = null;
                }
                sourceBuffer.Initialize(mse);
            }
    }

        internal void UpdateData(TimeSpan now)
        {
            var eos = true;
            try
            {
                var vidBuffer = GetVideoBuffer();
                var audioBuffer = GetAudioBuffer();

                if (vidBuffer == null && audioBuffer == null)
                {
                    throw new InvalidOperationException("Video or Audio buffer is null");
                }
                if (manifest.IsLive)
                {
                    UpdateLiveSourceBuffers();
                }

                if (vidBuffer != null && !vidBuffer.IsEndOfStream)
                {
                    vidBuffer.UpdateData(now);
                    eos = false;
                }

                if (audioBuffer != null && !audioBuffer.IsEndOfStream)
                {
                    audioBuffer.UpdateData(now);
                    eos = false;
                }
                if (eos)
                {
                    if (mse.ReadyState == MseReadyState.Open)
                    {
#if DEBUG
                        Logger.Log("Calling EndOfStream");
#endif
                        mse.EndOfStream(MseEndOfStreamStatus.Success);
                    }
                }
            }
            catch (Exception)
            {

            }
        }

        private SourceBufferManager GetAudioBuffer()
        {
            foreach (var sb in sourceBuffers)
            {
                if (sb.IsAudio)
                {
                    return sb;
                }
            }
            return null;
        }

        private SourceBufferManager GetVideoBuffer()
        {
            foreach (var sb in sourceBuffers)
            {
                if (sb.IsVideo)
                {
                    return sb;
                }
            }
            return null;
        }

        internal void Close()
        {
            try
            {
                if (manifestReloadTimer != null)
                {
                    manifestReloadTimer.Stop();
                }
                sourceBuffers.Clear();
                mse = null;
                playbackTimer.Stop();
            }
            catch (Exception e)
            {
#if DEBUG
                Logger.Log("Exception has been thrown: " + e.Message);
#endif
            }
        }

        internal async void GoToLiveEdge()
        {
            await ReloadManifest();

            sourceBuffers.Clear();
            mse.EndOfStream(MseEndOfStreamStatus.Success);

            opened = false;
            mse = new MseStreamSource();
            AddAudioAndVideoSourceBuffers(manifest.GetSelectedPeriod());

            SetLiveOffsetValue();
            
            ConfigureMse();
            mediaElement.SetMediaStreamSource(mse);
            
            CheckForMediaPresentationDuration();
            UpdateLiveSourceBuffersToLiveEdge();
            mediaElement.Play();
        }

        internal void SetLiveOffsetValue()
        {
            if (HasLiveOffsetValue)
            {
                var hnsdesiredLiveOffset = (ulong)(liveOffset.TotalMilliseconds * 10000); //hns
                foreach (var sourcebuffer in sourceBuffers)
                {
                    ((LiveSourceBufferManager)sourcebuffer).DesiredLiveOffset = hnsdesiredLiveOffset;
                }
            }
        }
        
    }
}
