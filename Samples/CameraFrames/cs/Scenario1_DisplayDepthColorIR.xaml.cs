//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the Microsoft Public License.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Linq;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_DisplayDepthColorIR : Page
    {
        private MediaCapture _mediaCapture;

        private List<MediaFrameReader> _sourceReaders = new List<MediaFrameReader>();
        private IReadOnlyDictionary<MediaFrameSourceKind, FrameRenderer> _frameRenderers;
       
        private int _groupSelectionIndex;
        private readonly SimpleLogger _logger;

        public Scenario1_DisplayDepthColorIR()
        {
            InitializeComponent();
            _logger = new SimpleLogger(outputTextBlock);

            // This sample reads three kinds of frames: Color, Depth, and Infrared.
            _frameRenderers = new Dictionary<MediaFrameSourceKind, FrameRenderer>()
            {
                { MediaFrameSourceKind.Color, new FrameRenderer(colorPreviewImage) },
                { MediaFrameSourceKind.Depth, new FrameRenderer(depthPreviewImage)},
                { MediaFrameSourceKind.Infrared, new FrameRenderer(infraredPreviewImage)},
            };
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            await PickNextMediaSourceAsync();
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            await CleanupMediaCaptureAsync();
        }

        private async void NextButton_Click(object eventSender, RoutedEventArgs eventArgs)
        {
            await PickNextMediaSourceAsync();
        }

        /// <summary>
        /// Disables the "Next Group" button while we switch to the next camera
        /// source and start reading frames.
        /// </summary>
        private async Task PickNextMediaSourceAsync()
        {
            try
            {
                this.NextButton.IsEnabled = false;
                await PickNextMediaSourceWorkerAsync();
            }
            finally
            {
                this.NextButton.IsEnabled = true;
            }
        }

        /// <summary>
        /// Switches to the next camera source and starts reading frames.
        /// </summary>
        private async Task PickNextMediaSourceWorkerAsync()
        {
            await CleanupMediaCaptureAsync();

            var allGroups = await MediaFrameSourceGroup.FindAllAsync();

            if (allGroups.Count == 0)
            {
                _logger.Log("No source groups found.");
                return;
            }

            // Pick next group in the array after each time the Next button is clicked.
            _groupSelectionIndex = (_groupSelectionIndex + 1) % allGroups.Count;
            var selectedGroup = allGroups[_groupSelectionIndex];

            _logger.Log($"Found {allGroups.Count} groups and selecting index [{_groupSelectionIndex}]: {selectedGroup.DisplayName}");

            try
            {
                // Initialize MediaCapture with selected group.
                // This can raise an exception if the source no longer exists,
                // or if the source could not be initialized.
                await InitializeMediaCaptureAsync(selectedGroup);
            }
            catch (Exception exception)
            {
                _logger.Log($"MediaCapture initialization error: {exception.Message}");
                await CleanupMediaCaptureAsync();
                return;
            }

            // Set up frame readers, register event handlers and start streaming.
            var startedKinds = new HashSet<MediaFrameSourceKind>();
            foreach (MediaFrameSource source in _mediaCapture.FrameSources.Values)
            {
                MediaFrameSourceKind kind = source.Info.SourceKind;

                // Ignore this source if we already have a source of this kind.
                if (startedKinds.Contains(kind))
                {
                    continue;
                }

                // Look for a format which the FrameRenderer can render.
                string requestedSubtype = null;
                foreach (MediaFrameFormat format in source.SupportedFormats)
                {
                    requestedSubtype = FrameRenderer.GetSubtypeForFrameReader(kind, format);
                    if (requestedSubtype != null)
                    {
                        // Tell the source to use the format we can render.
                        await source.SetFormatAsync(format);
                        break;
                    }
                }
                if (requestedSubtype == null)
                {
                    // No acceptable format was found. Ignore this source.
                    continue;
                }

                MediaFrameReader frameReader = await _mediaCapture.CreateFrameReaderAsync(source, requestedSubtype);

                frameReader.FrameArrived += FrameReader_FrameArrived;
                _sourceReaders.Add(frameReader);

                MediaFrameReaderStartStatus status = await frameReader.StartAsync();
                if (status == MediaFrameReaderStartStatus.Success)
                {
                    _logger.Log($"Started {kind} reader.");
                    startedKinds.Add(kind);
                }
                else
                {
                    _logger.Log($"Unable to start {kind} reader. Error: {status}");
                }
            }

            if (startedKinds.Count == 0)
            {
                _logger.Log($"No eligible sources in {selectedGroup.DisplayName}.");
            }
        }

        /// <summary>
        /// Initializes the MediaCapture object with the given source group.
        /// </summary>
        /// <param name="sourceGroup">SourceGroup with which to initialize.</param>
        private async Task InitializeMediaCaptureAsync(MediaFrameSourceGroup sourceGroup)
        {
            if (_mediaCapture != null)
            {
                return;
            }

            // Initialize mediacapture with the source group.
            _mediaCapture = new MediaCapture();
            var settings = new MediaCaptureInitializationSettings
            {
                SourceGroup = sourceGroup,

                // This media capture can share streaming with other apps.
                SharingMode = MediaCaptureSharingMode.SharedReadOnly,

                // Only stream video and don't initialize audio capture devices.
                StreamingCaptureMode = StreamingCaptureMode.Video,

                // Set to CPU to ensure frames always contain CPU SoftwareBitmap images
                // instead of preferring GPU D3DSurface images.
                MemoryPreference = MediaCaptureMemoryPreference.Cpu
            };

            await _mediaCapture.InitializeAsync(settings);
            _logger.Log("MediaCapture is successfully initialized in shared mode.");
        }

        /// <summary>
        /// Unregisters FrameArrived event handlers, stops and disposes frame readers
        /// and disposes the MediaCapture object.
        /// </summary>
        private async Task CleanupMediaCaptureAsync()
        {
            if (_mediaCapture != null)
            {
                using (var mediaCapture = _mediaCapture)
                {
                    _mediaCapture = null;

                    foreach (var reader in _sourceReaders)
                    {
                        if (reader != null)
                        {
                            reader.FrameArrived -= FrameReader_FrameArrived;
                            await reader.StopAsync();
                            reader.Dispose();
                        }
                    }
                    _sourceReaders.Clear();
                }
            }
        }

        /// <summary>
        /// Handles a frame arrived event and renders the frame to the screen.
        /// </summary>
        private void FrameReader_FrameArrived(MediaFrameReader sender, MediaFrameArrivedEventArgs args)
        {
            // TryAcquireLatestFrame will return the latest frame that has not yet been acquired.
            // This can return null if there is no such frame, or if the reader is not in the
            // "Started" state. The latter can occur if a FrameArrived event was in flight
            // when the reader was stopped.
            using (var frame = sender.TryAcquireLatestFrame())
            {
                if (frame != null)
                {
                    var renderer = _frameRenderers[frame.SourceKind];
                    renderer.ProcessFrame(frame);
                }
            }
        }
    };
}
