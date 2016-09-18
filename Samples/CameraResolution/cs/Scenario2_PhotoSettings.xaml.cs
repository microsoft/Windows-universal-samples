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
using Windows.Graphics.Imaging;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{

    /// <summary>
    /// Scenario 2 Change camera preview and photo settings
    /// </summary>
    public sealed partial class Scenario2_PhotoSettings : Page
    {
        // Private MainPage object for status updates
        private MainPage rootPage = MainPage.Current;

        // Object to manage access to camera devices
        private MediaCapturePreviewer _previewer = null;

        // Folder in which the captures will be stored (initialized in InitializeCameraButton_Click)
        private StorageFolder _captureFolder = null;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario2_PhotoSettings"/> class.
        /// </summary>
        public Scenario2_PhotoSettings()
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
        public void CheckIfStreamsAreIdentical()
        {
            if (_previewer.MediaCapture.MediaCaptureSettings.VideoDeviceCharacteristic == VideoDeviceCharacteristic.AllStreamsIdentical ||
                _previewer.MediaCapture.MediaCaptureSettings.VideoDeviceCharacteristic == VideoDeviceCharacteristic.PreviewPhotoStreamsIdentical)
            {
                rootPage.NotifyUser("Warning: Preview and photo streams for this device are identical, changing one will affect the other", NotifyType.ErrorMessage);
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
                button.Visibility = Visibility.Collapsed;
                PreviewControl.Visibility = Visibility.Visible;
                CheckIfStreamsAreIdentical();
                PopulateComboBox(MediaStreamType.VideoPreview, PreviewSettings);
                PopulateComboBox(MediaStreamType.Photo, PhotoSettings, false);
                PhotoButton.IsEnabled = true;
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
            }
        }

        /// <summary>
        /// Event handler for Photo settings combo box. Updates stream resolution based on the selection.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void PhotoSettings_Changed(object sender, RoutedEventArgs e)
        {
            if (_previewer.IsPreviewing)
            {
                var selectedItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                var encodingProperties = (selectedItem.Tag as StreamResolution).EncodingProperties;
                await _previewer.MediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.Photo, encodingProperties);
            }
        }

        /// <summary>
        /// Takes a photo to and saves to a StorageFile
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void PhotoButton_Click(object sender, RoutedEventArgs e)
        {
            if (_previewer.IsPreviewing)
            {
                // Disable the photo button while taking a photo
                PhotoButton.IsEnabled = false;

                var stream = new InMemoryRandomAccessStream();

                try
                {
                    // Take and save the photo
                    var file = await _captureFolder.CreateFileAsync("SimplePhoto.jpg", CreationCollisionOption.GenerateUniqueName);
                    await _previewer.MediaCapture.CapturePhotoToStorageFileAsync(ImageEncodingProperties.CreateJpeg(), file);
                    rootPage.NotifyUser("Photo taken, saved to: " + file.Path, NotifyType.StatusMessage);
                }
                catch (Exception ex)
                {
                    // File I/O errors are reported as exceptions.
                    rootPage.NotifyUser("Exception when taking a photo: " + ex.Message, NotifyType.ErrorMessage);
                }

                // Done taking a photo, so re-enable the button
                PhotoButton.IsEnabled = true;
            }
        }

        /// <summary>
        /// Populates the given combo box with all possible combinations of the given stream type settings returned by the camera driver
        /// </summary>
        /// <param name="streamType"></param>
        /// <param name="comboBox"></param>
        private void PopulateComboBox(MediaStreamType streamType, ComboBox comboBox, bool showFrameRate = true)
        {
            // Query all preview properties of the device 
            IEnumerable<StreamResolution> allStreamProperties = _previewer.MediaCapture.VideoDeviceController.GetAvailableMediaStreamProperties(streamType).Select(x => new StreamResolution(x));
            // Order them by resolution then frame rate
            allStreamProperties = allStreamProperties.OrderByDescending(x => x.Height * x.Width).ThenByDescending(x => x.FrameRate);

            // Populate the combo box with the entries
            foreach (var property in allStreamProperties)
            {
                ComboBoxItem comboBoxItem = new ComboBoxItem();
                comboBoxItem.Content = property.GetFriendlyName(showFrameRate);
                comboBoxItem.Tag = property;
                comboBox.Items.Add(comboBoxItem);
            }
        }
    }
}
