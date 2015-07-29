//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Microsoft.Samples.SimpleCommunication;
using System;
using System.Linq;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;

namespace SimpleCommunication
{
    internal class CaptureDevice
    {
        // Media capture object
        private MediaCapture mediaCapture;
        // Custom media sink
        private StspMediaSinkProxy mediaSink;
        // Flag indicating if recording to custom sink has started
        private bool recordingStarted = false;
        private bool forwardEvents = false;

        // Wraps the capture failed and media sink incoming connection events
        public event EventHandler<IncomingConnectionEventArgs> IncomingConnectionArrived;
        public event EventHandler<MediaCaptureFailedEventArgs> CaptureFailed;

        public CaptureDevice() { }

        /// <summary>
        ///  Handler for the wrapped MediaCapture object's Failed event. It just wraps and forward's MediaCapture's 
        ///  Failed event as own CaptureFailed event
        /// </summary>
        private void mediaCapture_Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            if (CaptureFailed != null && forwardEvents) CaptureFailed(this, errorEventArgs);
        }

        /// <summary>
        /// Handler for the wrapped media sink object's IncomingConnectionEvent. Expose the wrapped event to
        /// listeners as own IncomingConnectionArrived event.
        /// </summary>
        private void mediaSink_IncomingConnectionEvent(object sender, object args)
        {
            if (IncomingConnectionArrived != null && forwardEvents) IncomingConnectionArrived((StspMediaSinkProxy)sender, (IncomingConnectionEventArgs)args);
        }

        /// <summary>
        /// Cleans up the resources.
        /// </summary>
        private void CleanupSink()
        {
            if (mediaSink != null)
            {
                mediaSink.IncomingConnectionEvent -= mediaSink_IncomingConnectionEvent;
                mediaSink.Dispose();
                mediaSink = null;
                recordingStarted = false;
            }
        }

        private void DoCleanup()
        {
            if (mediaCapture != null)
            {
                mediaCapture.Failed -= mediaCapture_Failed;
                mediaCapture = null;
            }

            CleanupSink();
        }

        public async Task InitializeAsync()
        {
            try
            {
                forwardEvents = true;

                if (mediaCapture != null)
                {
                    throw new InvalidOperationException("Camera is already initialized");
                }

                mediaCapture = new MediaCapture();
                mediaCapture.Failed += mediaCapture_Failed;

                await mediaCapture.InitializeAsync();
            }
            catch (Exception e)
            {
                DoCleanup();
                throw e;
            }
        }

        /// <summary>
        /// Asynchronous method cleaning up resources and stopping recording if necessary.
        /// </summary>
        public async Task CleanUpAsync()
        {
            try
            {
                forwardEvents = true;

                if (mediaCapture == null && mediaSink == null) return;

                if (recordingStarted)
                {
                    await mediaCapture.StopRecordAsync();
                }

                DoCleanup();
            }
            catch (Exception)
            {
                DoCleanup();
            }
        }

        /// <summary>
        /// Creates url object from MediaCapture
        /// </summary>
        public MediaCapture CaptureSource
        {
            get { return mediaCapture; }
        }

        /// <summary>
        /// Allow selection of camera settings.
        /// </summary>
        /// <param name="mediaStreamType" type="Windows.Media.Capture.MediaStreamType">
        /// Type of a the media stream.
        /// </param>
        /// <param name="filterSettings" type="Func<Windows.Media.MediaProperties.IMediaEncodingProperties, bool>">
        /// A predicate function, which will be called to filter the correct settings.
        /// </param>
        public async Task<IMediaEncodingProperties> SelectPreferredCameraStreamSettingAsync(MediaStreamType mediaStreamType, Func<IMediaEncodingProperties, bool> filterSettings)
        {
            IMediaEncodingProperties previewEncodingProperties = null;

            if (mediaStreamType == MediaStreamType.Audio || mediaStreamType == MediaStreamType.Photo)
            {
                throw new ArgumentException("mediaStreamType value of MediaStreamType.Audio or MediaStreamType.Photo is not supported", "mediaStreamType");
            }
            if (filterSettings == null)
            {
                throw new ArgumentNullException("filterSettings");
            }

            var properties = mediaCapture.VideoDeviceController.GetAvailableMediaStreamProperties(mediaStreamType);
            var filterredProperties = properties.Where(filterSettings);
            var preferredSettings = filterredProperties.ToArray();

            Array.Sort<IMediaEncodingProperties>(preferredSettings, (x, y) =>
            {
                return (int)(((x as VideoEncodingProperties).Width) -
                    (y as VideoEncodingProperties).Width);
            });

            if (preferredSettings.Length > 0)
            {
                previewEncodingProperties = preferredSettings[0];
                await mediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(mediaStreamType, preferredSettings[0]);
            }

            return previewEncodingProperties;
        }

        /// <summary>
        /// Starts media recording asynchronously
        /// </summary>
        /// <param name="encodingProfile">
        /// Encoding profile used for the recording session
        /// </param>
        public async Task StartRecordingAsync(MediaEncodingProfile encodingProfile)
        {
            try
            {
                // We cannot start recording twice.
                if (mediaSink != null && recordingStarted)
                {
                    throw new InvalidOperationException("Recording already started.");
                }

                // Release sink if there is one already.
                CleanupSink();

                // Create new sink
                mediaSink = new StspMediaSinkProxy();
                mediaSink.IncomingConnectionEvent += mediaSink_IncomingConnectionEvent;

                var mfExtension = await mediaSink.InitializeAsync(encodingProfile.Audio, encodingProfile.Video);
                await mediaCapture.StartRecordToCustomSinkAsync(encodingProfile, mfExtension);

                recordingStarted = true;
            }
            catch (Exception e)
            {
                CleanupSink();
                throw e;
            }
        }

        /// <summary>
        /// Stops recording asynchronously
        /// </summary>
        public async Task StopRecordingAsync()
        {
            if (recordingStarted)
            {
                try
                {
                    await mediaCapture.StopRecordAsync();
                    CleanupSink();
                }
                catch (Exception)
                {
                    CleanupSink();
                }
            }
        }

        public static async Task<bool> CheckForRecordingDeviceAsync()
        {
            var cameraFound = false;

            var devices = await DeviceInformation.FindAllAsync(DeviceClass.VideoCapture);
            if (devices.Count > 0)
            {
                cameraFound = true;
            }

            return cameraFound;
        }
    }
}
