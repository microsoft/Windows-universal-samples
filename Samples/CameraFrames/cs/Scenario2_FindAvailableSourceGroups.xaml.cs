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
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario2_FindAvailableSourceGroups : Page
    {
        private MediaCapture _mediaCapture;
        private MediaFrameSource _source;
        private MediaFrameReader _reader;
        private FrameRenderer _frameRenderer;
        private bool _streaming = false;

        private SourceGroupCollection _groupCollection;
        private readonly SimpleLogger _logger;

        public Scenario2_FindAvailableSourceGroups()
        {
            InitializeComponent();
            _logger = new SimpleLogger(outputTextBlock);
            _frameRenderer = new FrameRenderer(PreviewImage);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            /// SourceGroupCollection will setup device watcher to monitor
            /// SourceGroup devices enabled or disabled from the system.
            _groupCollection = new SourceGroupCollection(this.Dispatcher);
            GroupComboBox.ItemsSource = _groupCollection.FrameSourceGroups;
        }

        protected override async void OnNavigatedFrom(NavigationEventArgs e)
        {
            _groupCollection?.Dispose();
            await StopReaderAsync();
            DisposeMediaCapture();
        }

        private async Task UpdateButtonStateAsync()
        {
            await Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                StartButton.IsEnabled = _source != null && !_streaming;
                StopButton.IsEnabled = _source != null && _streaming;
            });
        }

        /// <summary>
        /// Disposes of the MediaCapture object and clears the items from the Format and Source ComboBoxes.
        /// </summary>
        private void DisposeMediaCapture()
        {
            FormatComboBox.ItemsSource = null;
            SourceComboBox.ItemsSource = null;

            _source = null;

            _mediaCapture?.Dispose();
            _mediaCapture = null;
        }

        /// <summary>
        /// Stops reading from the previous selection and starts reading frames from the newly selected source group.
        /// </summary>
        private async void GroupComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            await StopReaderAsync();
            DisposeMediaCapture();

            var group = GroupComboBox.SelectedItem as FrameSourceGroupModel;
            if (group != null)
            {
                await InitializeCaptureAsync();

                SourceComboBox.ItemsSource = group.SourceInfos;
                SourceComboBox.SelectedIndex = 0;
            }
        }

        /// <summary>
        /// Starts reading from the newly selected source.
        /// </summary>
        private async void SourceComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            await StopReaderAsync();

            if (SourceComboBox.SelectedItem != null)
            {
                await StartReaderAsync();

                // Get the formats supported by the selected source into the FormatComboBox.
                IEnumerable<FrameFormatModel> formats = null;
                if (_mediaCapture != null && _source != null)
                {
                    formats = _source.SupportedFormats.Select(format => new FrameFormatModel(format));
                }

                FormatComboBox.ItemsSource = formats;
            }
        }

        /// <summary>
        /// Sets the video format for the current frame source.
        /// </summary>
        private async void FormatComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var format = FormatComboBox.SelectedItem as FrameFormatModel;
            await ChangeMediaFormatAsync(format);
        }

        /// <summary>
        /// Starts reading frames from the current reader.
        /// </summary>
        private async Task StartReaderAsync()
        {
            await CreateReaderAsync();

            if (_reader != null && !_streaming)
            {
                MediaFrameReaderStartStatus result = await _reader.StartAsync();
                _logger.Log($"Start reader with result: {result}");

                if (result == MediaFrameReaderStartStatus.Success)
                {
                    _streaming = true;
                    await UpdateButtonStateAsync();
                }
            }
        }

        /// <summary>
        /// Creates a frame reader from the current frame source and registers to handle its frame events.
        /// </summary>
        private async Task CreateReaderAsync()
        {
            await InitializeCaptureAsync();

            UpdateFrameSource();

            if (_source != null)
            {
                _reader = await _mediaCapture.CreateFrameReaderAsync(_source);
                _reader.FrameArrived += Reader_FrameArrived;
                
                _logger.Log($"Reader created on source: {_source.Info.Id}");
            }
        }

        /// <summary>
        /// Updates the current frame source to the one corresponding to the user's selection.
        /// </summary>
        private void UpdateFrameSource()
        {
            var info = SourceComboBox.SelectedItem as FrameSourceInfoModel;
            if (_mediaCapture != null && info != null && info.SourceGroup != null)
            {
                var groupModel = GroupComboBox.SelectedItem as FrameSourceGroupModel;
                if (groupModel == null || groupModel.Id != info.SourceGroup.Id)
                {
                    SourceComboBox.SelectedItem = null;
                    return;
                }

                if (_source == null || _source.Info.Id != info.SourceInfo.Id)
                {
                    _mediaCapture.FrameSources.TryGetValue(info.SourceInfo.Id, out _source);
                }
            }
            else
            {
                _source = null;
            }
        }

        /// <summary>
        /// Initializes the MediaCapture object with the current source group.
        /// </summary>
        private async Task InitializeCaptureAsync()
        {
            var groupModel = GroupComboBox.SelectedItem as FrameSourceGroupModel;
            if (_mediaCapture == null && groupModel != null)
            {
                // Create a new media capture object.
                _mediaCapture = new MediaCapture();

                var settings = new MediaCaptureInitializationSettings()
                {
                    // Select the source we will be reading from.
                    SourceGroup = groupModel.SourceGroup,

                    // This media capture has exclusive control of the source.
                    SharingMode = MediaCaptureSharingMode.ExclusiveControl,

                    // Set to CPU to ensure frames always contain CPU SoftwareBitmap images,
                    // instead of preferring GPU D3DSurface images.
                    MemoryPreference = MediaCaptureMemoryPreference.Cpu,

                    // Capture only video. Audio device will not be initialized.
                    StreamingCaptureMode = StreamingCaptureMode.Video,
                };

                try
                {
                    // Initialize MediaCapture with the specified group.
                    // This can raise an exception if the source no longer exists,
                    // or if the source could not be initialized.
                    await _mediaCapture.InitializeAsync(settings);
                    _logger.Log($"Successfully initialized MediaCapture for {groupModel.DisplayName}");
                }
                catch (Exception exception)
                {
                    _logger.Log(exception.Message);
                    DisposeMediaCapture();
                }
            }
        }

        /// <summary>
        /// Sets the frame format of the current frame source.
        /// </summary>
        private async Task ChangeMediaFormatAsync(FrameFormatModel format)
        {
            if (_source == null)
            {
                _logger.Log("Unable to set format when source is not set.");
                return;
            }

            // Set the source format if the selected one is different from the current one.
            if (format != null && !format.HasSameFormat(_source.CurrentFormat))
            {
                await _source.SetFormatAsync(format.Format);
                _logger.Log($"Format set to {format.DisplayName}");
            }
        }

        /// <summary>
        /// Stops reading from the frame reader, disposes of the reader and updates the button state.
        /// </summary>
        private async Task StopReaderAsync()
        {
            _streaming = false;

            if (_reader != null)
            {
                await _reader.StopAsync();
                _reader.FrameArrived -= Reader_FrameArrived;
                _reader.Dispose();
                _reader = null;

                _logger.Log("Reader stopped.");
            }

            await UpdateButtonStateAsync();
        }

        /// <summary>
        /// Bound to the click of the Start button.
        /// </summary>
        private async void StartButton_Click(object sender, RoutedEventArgs e)
        {
            await StartReaderAsync();
        }

        /// <summary>
        /// Bound to the click of the Stop button.
        /// </summary>
        private async void StopButton_Click(object sender, RoutedEventArgs e)
        {
            await StopReaderAsync();
        }

        /// <summary>
        /// Handles the frame arrived event by converting the frame to a displayable
        /// format and rendering it to the screen.
        /// </summary>
        private void Reader_FrameArrived(MediaFrameReader sender, MediaFrameArrivedEventArgs args)
        {
            // TryAcquireLatestFrame will return the latest frame that has not yet been acquired.
            // This can return null if there is no such frame, or if the reader is not in the
            // "Started" state. The latter can occur if a FrameArrived event was in flight
            // when the reader was stopped.
            using (var frame = sender.TryAcquireLatestFrame())
            {
                _frameRenderer.ProcessFrame(frame);
            }
        }
    }
}
