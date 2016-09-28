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
using Windows.Media.MediaProperties;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{

    /// <summary>
    /// Scenario 3 keep aspect ratios the same
    /// </summary>
    public sealed partial class Scenario3_AspectRatio : Page
    {
        // Private MainPage object for status updates
        private MainPage rootPage = MainPage.Current;

        // Object to manage access to camera devices
        private MediaCapturePreviewer _previewer = null;

        // Folder in which the captures will be stored (initialized in InitializeCameraButton_Click)
        private StorageFolder _captureFolder = null;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario1_PreviewSettings"/> class.
        /// </summary>
        public Scenario3_AspectRatio()
        {
            this.InitializeComponent();
            _previewer = new MediaCapturePreviewer(PreviewControl, Dispatcher);
        }

        protected override async void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            await _previewer.CleanupCameraAsync();
        }

        /// <summary>
        /// On some devices there may not be seperate streams for preview/photo/video. In this case, changing any of them
        /// will change all of them since they are the same stream
        /// </summary>
        private void CheckIfStreamsAreIdentical()
        {
            if (_previewer.MediaCapture.MediaCaptureSettings.VideoDeviceCharacteristic == VideoDeviceCharacteristic.AllStreamsIdentical ||
                _previewer.MediaCapture.MediaCaptureSettings.VideoDeviceCharacteristic == VideoDeviceCharacteristic.PreviewRecordStreamsIdentical)
            {
                rootPage.NotifyUser("Warning: Preview and video streams for this device are identical, changing one will affect the other", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Initializes the camera and populates the UI
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void InitializeCameraButton_Click(object sender, RoutedEventArgs e)
        {
            var button = sender as Button;

            // Clear any previous message.
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            button.IsEnabled = false;
            await _previewer.InitializeCameraAsync();
            button.IsEnabled = true;

            if (_previewer.IsPreviewing)
            {
                if (string.IsNullOrEmpty(_previewer.MediaCapture.MediaCaptureSettings.AudioDeviceId))
                {
                    rootPage.NotifyUser("No audio device available. Cannot capture.", NotifyType.ErrorMessage);
                }
                else
                {
                    button.Visibility = Visibility.Collapsed;
                    PreviewControl.Visibility = Visibility.Visible;
                    CheckIfStreamsAreIdentical();
                    PopulateComboBoxes();
                    VideoButton.IsEnabled = true;
                }
            }

            var picturesLibrary = await StorageLibrary.GetLibraryAsync(KnownLibraryId.Pictures);
            // Fall back to the local app storage if the Pictures Library is not available
            _captureFolder = picturesLibrary.SaveFolder ?? ApplicationData.Current.LocalFolder;
        }

        /// <summary>
        ///  Event handler for Preview settings combo box. Updates stream resolution based on the selection.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void PreviewSettings_Changed(object sender, RoutedEventArgs e)
        {
            if (_previewer.IsPreviewing)
            {
                var selectedItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                var encodingProperties = (selectedItem.Tag as StreamResolution).EncodingProperties;
                await _previewer.MediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.VideoPreview, encodingProperties);

                // The preview just changed, update the video combo box
                MatchPreviewAspectRatio();
            }
        }

        /// <summary>
        /// Event handler for Video settings combo box. Updates stream resolution based on the selection.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void VideoSettings_Changed(object sender, RoutedEventArgs e)
        {
            if (_previewer.IsPreviewing)
            {
                if (VideoSettings.SelectedIndex > -1)
                {
                    var selectedItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                    var encodingProperties = (selectedItem.Tag as StreamResolution).EncodingProperties;
                    await _previewer.MediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.VideoRecord, encodingProperties);
                }
            }
        }

        /// <summary>
        /// Records an MP4 video to a StorageFile
        /// </summary>
        private async void VideoButton_Click(object sender, RoutedEventArgs e)
        {
            if (_previewer.IsPreviewing)
            {
                if (!_previewer.IsRecording)
                {
                    try
                    {
                        // Create storage file in Video Library
                        var videoFile = await _captureFolder.CreateFileAsync("SimpleVideo.mp4", CreationCollisionOption.GenerateUniqueName);
                        var encodingProfile = MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto);

                        await _previewer.MediaCapture.StartRecordToStorageFileAsync(encodingProfile, videoFile);

                        // Reflect changes in the UI
                        VideoButton.Content = "Stop Video";
                        _previewer.IsRecording = true;

                        rootPage.NotifyUser("Recording file, saving to: " + videoFile.Path, NotifyType.StatusMessage);
                    }
                    catch (Exception ex)
                    {
                        // File I/O errors are reported as exceptions.
                        rootPage.NotifyUser("Exception when starting video recording: " + ex.Message, NotifyType.ErrorMessage);
                    }
                }
                else
                {
                    // Reflect the changes in UI and stop recording
                    VideoButton.Content = "Record Video";
                    _previewer.IsRecording = false;

                    await _previewer.MediaCapture.StopRecordAsync();

                    rootPage.NotifyUser("Stopped recording!", NotifyType.StatusMessage);
                }
            }
        }

        /// <summary>
        /// Populates the combo boxes with preview settings and matching ratio settings for the video stream
        /// </summary>
        private void PopulateComboBoxes()
        {
            // Query all properties of the device 
            IEnumerable<StreamResolution> allPreviewProperties = _previewer.MediaCapture.VideoDeviceController.GetAvailableMediaStreamProperties(MediaStreamType.VideoPreview).Select(x => new StreamResolution(x));
            // Order them by resolution then frame rate
            allPreviewProperties = allPreviewProperties.OrderByDescending(x => x.Height * x.Width).ThenByDescending(x => x.FrameRate);

            // Populate the preview combo box with the entries
            foreach (var property in allPreviewProperties)
            {
                ComboBoxItem comboBoxItem = new ComboBoxItem();
                comboBoxItem.Content = property.GetFriendlyName() + 
                    String.Format(" A. Ratio: {0}", property.AspectRatio.ToString("#.##"));
                comboBoxItem.Tag = property;
                PreviewSettings.Items.Add(comboBoxItem);
            }

            // Keep the same aspect ratio with the video stream
            MatchPreviewAspectRatio();            
        }

        /// <summary>
        /// Finds all the available video resolutions that match the aspect ratio of the preview stream
        /// Note: This should also be done with photos as well. This same method can be modified for photos
        /// by changing the MediaStreamType from VideoPreview to Photo.
        /// </summary>
        private void MatchPreviewAspectRatio()
        {
            // Query all properties of the device 
            IEnumerable<StreamResolution> allVideoProperties = _previewer.MediaCapture.VideoDeviceController.GetAvailableMediaStreamProperties(MediaStreamType.VideoRecord).Select(x => new StreamResolution(x));

            // Query the current preview settings
            StreamResolution previewProperties = new StreamResolution(_previewer.MediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview));

            // Get all formats that have the same-ish aspect ratio as the preview
            // Allow for some tolerance in the aspect ratio comparison
            const double ASPECT_RATIO_TOLERANCE = 0.015;
            var matchingFormats = allVideoProperties.Where(x => Math.Abs(x.AspectRatio - previewProperties.AspectRatio) < ASPECT_RATIO_TOLERANCE);

            // Order them by resolution then frame rate
            allVideoProperties = matchingFormats.OrderByDescending(x => x.Height * x.Width).ThenByDescending(x => x.FrameRate);

            // Clear out old entries and populate the video combo box with new matching entries
            VideoSettings.Items.Clear();
            foreach (var property in allVideoProperties)
            {
                ComboBoxItem comboBoxItem = new ComboBoxItem();
                comboBoxItem.Content = property.GetFriendlyName();
                comboBoxItem.Tag = property;
                VideoSettings.Items.Add(comboBoxItem);
            }
            VideoSettings.SelectedIndex = -1;
        }
    }
}
