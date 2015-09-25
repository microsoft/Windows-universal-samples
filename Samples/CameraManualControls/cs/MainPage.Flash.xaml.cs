//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Devices;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CameraManualControls
{
    public sealed partial class MainPage : Page
    {

        private void UpdateFlashControlCapabilities()
        {
            var flashControl = _mediaCapture.VideoDeviceController.FlashControl;

            if (flashControl.Supported)
            {
                FlashButton.Tag = Visibility.Visible;

                FlashAutoRadioButton.IsChecked = true;

                if (flashControl.RedEyeReductionSupported)
                {
                    RedEyeFlashCheckBox.Visibility = Visibility.Visible;
                }

                // Video light is not strictly part of flash, but users might expect to find it there
                if (_mediaCapture.VideoDeviceController.TorchControl.Supported)
                {
                    TorchCheckBox.Visibility = Visibility.Visible;
                }
            }
            else
            {
                FlashButton.Visibility = Visibility.Collapsed;
                FlashButton.Tag = Visibility.Collapsed;
            }
        }

        private void FlashOnRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            // Note that the FlashControl only applies to photos. To turn on the light for video recording, use the _mediaCapture.VideoDeviceController.TorchControl

            _mediaCapture.VideoDeviceController.FlashControl.Enabled = true;
            _mediaCapture.VideoDeviceController.FlashControl.Auto = false;
        }

        private void FlashAutoRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            _mediaCapture.VideoDeviceController.FlashControl.Enabled = true;
            _mediaCapture.VideoDeviceController.FlashControl.Auto = true;
        }

        private void FlashOffRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            _mediaCapture.VideoDeviceController.FlashControl.Enabled = false;
        }

        private void RedEyeFlashCheckBox_CheckedChanged(object sender, RoutedEventArgs e)
        {
            _mediaCapture.VideoDeviceController.FlashControl.RedEyeReduction = (RedEyeFlashCheckBox.IsChecked == true);
        }

        private void TorchCheckBox_CheckedChanged(object sender, RoutedEventArgs e)
        {
            _mediaCapture.VideoDeviceController.TorchControl.Enabled = (TorchCheckBox.IsChecked == true);
        }

    }
}
