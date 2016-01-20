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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Scenario 1 Change camera preview settings
    /// </summary>
    public sealed partial class Scenario1_PreviewSettings : Page
    {
        // Private MainPage object for status updates
        private MainPage rootPage = MainPage.Current;

        // Object to manage access to camera devices
        private MediaCapturePreviewer _previewer = null;

        /// <summary>
        /// Initializes a new instance of the <see cref="Scenario1_PreviewSettings"/> class.
        /// </summary>
        public Scenario1_PreviewSettings()
        {
            this.InitializeComponent();
            _previewer = new MediaCapturePreviewer(PreviewControl, Dispatcher);
        }

        protected override async void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            await _previewer.CleanupCameraAsync();
        }
        
        /// <summary>
        /// Initializes the camera and populates the UI
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void InitializeCameraButton_Tapped(object sender, Windows.UI.Xaml.Input.TappedRoutedEventArgs e)
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
                PopulateSettingsComboBox();
            }
        }

        /// <summary>
        ///  Event handler for Preview settings combo box. Updates stream resolution based on the selection.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ComboBoxSettings_Changed(object sender, RoutedEventArgs e)
        {
            if (_previewer.IsPreviewing)
            {
                var selectedItem = (sender as ComboBox).SelectedItem as ComboBoxItem;
                var encodingProperties = (selectedItem.Tag as StreamResolution).EncodingProperties;
                await _previewer.MediaCapture.VideoDeviceController.SetMediaStreamPropertiesAsync(MediaStreamType.VideoPreview, encodingProperties);
            }
        }

        /// <summary>
        /// Populates the combo box with all possible combinations of settings returned by the camera driver
        /// </summary>
        private void PopulateSettingsComboBox()
        {
            // Query all properties of the device
            IEnumerable<StreamResolution> allProperties = _previewer.MediaCapture.VideoDeviceController.GetAvailableMediaStreamProperties(MediaStreamType.VideoPreview).Select(x => new StreamResolution(x));

            // Order them by resolution then frame rate
            allProperties = allProperties.OrderByDescending(x => x.Height * x.Width).ThenByDescending(x => x.FrameRate);

            // Populate the combo box with the entries
            foreach (var property in allProperties)
            {
                ComboBoxItem comboBoxItem = new ComboBoxItem();
                comboBoxItem.Content = property.GetFriendlyName();
                comboBoxItem.Tag = property;
                CameraSettings.Items.Add(comboBoxItem);
            }
        }
    }
}
