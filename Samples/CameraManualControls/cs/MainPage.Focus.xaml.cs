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
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Graphics.Display;
using Windows.Media.Capture;
using Windows.Media.Devices;
using Windows.Media.MediaProperties;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace CameraManualControls
{
    public sealed partial class MainPage : Page
    {

        private void UpdateFocusControlCapabilities()
        {
            var focusControl = _mediaCapture.VideoDeviceController.FocusControl;

            if (focusControl.Supported)
            {
                FocusButton.Tag = Visibility.Visible;

                // Unhook the event handler, so that changing properties on the slider won't trigger an API call
                FocusSlider.ValueChanged -= FocusSlider_ValueChanged;

                var value = focusControl.Value;
                FocusSlider.Minimum = focusControl.Min;
                FocusSlider.Maximum = focusControl.Max;
                FocusSlider.StepFrequency = focusControl.Step;
                FocusSlider.Value = value;

                FocusSlider.ValueChanged += FocusSlider_ValueChanged;

                CafFocusRadioButton.Visibility = focusControl.SupportedFocusModes.Contains(FocusMode.Continuous) ? Visibility.Visible : Visibility.Collapsed;

                // Tap to focus requires support for RegionsOfInterest
                TapFocusRadioButton.Visibility = (_mediaCapture.VideoDeviceController.RegionsOfInterestControl.AutoFocusSupported &&
                                                  _mediaCapture.VideoDeviceController.RegionsOfInterestControl.MaxRegions > 0) ? Visibility.Visible : Visibility.Collapsed;

                // Show the focus assist light only if the device supports it. Note that it exists under the FlashControl (not the FocusControl), so check for support there first
                FocusLightCheckBox.Visibility = (_mediaCapture.VideoDeviceController.FlashControl.Supported &&
                                                 _mediaCapture.VideoDeviceController.FlashControl.AssistantLightSupported) ? Visibility.Visible : Visibility.Collapsed;

                ManualFocusRadioButton.IsChecked = true;
            }
            else
            {
                FocusButton.Visibility = Visibility.Collapsed;
                FocusButton.Tag = Visibility.Collapsed;
            }
        }

        private void FocusLightCheckBox_CheckedChanged(object sender, RoutedEventArgs e)
        {
            var flashControl = _mediaCapture.VideoDeviceController.FlashControl;

            flashControl.AssistantLightEnabled = (FocusLightCheckBox.IsChecked == true);
        }

        private async void FocusSlider_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (_settingUpUi) return;

            var value = (sender as Slider).Value;

            await _mediaCapture.VideoDeviceController.FocusControl.SetValueAsync((uint)value);
        }

        private async void ManualFocusRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            // The below check is necessary because this event triggers during initialization of the page, before the MediaCapture is up
            if (!_isPreviewing) return;

            // Reset tap-to-focus status
            _isFocused = false;
            FocusRectangle.Visibility = Visibility.Collapsed;

            // Lock focus in case Continuous Autofocus was active when switching to Manual
            var focusControl = _mediaCapture.VideoDeviceController.FocusControl;
            await focusControl.LockAsync();
        }

        private async void CafFocusRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            // Reset tap-to-focus status
            _isFocused = false;
            FocusRectangle.Visibility = Visibility.Collapsed;

            var focusControl = _mediaCapture.VideoDeviceController.FocusControl;

            await focusControl.UnlockAsync();

            var settings = new FocusSettings { Mode = FocusMode.Continuous, AutoFocusRange = AutoFocusRange.FullRange };
            focusControl.Configure(settings);
            await focusControl.FocusAsync();
        }

        private async void TapFocusRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            // Lock focus in case Continuous Autofocus was active when switching to Tap-to-focus
            var focusControl = _mediaCapture.VideoDeviceController.FocusControl;
            await focusControl.LockAsync();

            // Wait for user tap, which will be handled in PreviewControl_Tapped
        }

        /// <summary>
        /// Focus the camera on the given rectangle of the preview, defined by the position and size parameters, in UI coordinates within the CaptureElement
        /// </summary>
        /// <param name="position">The position of the tap, to become the center of the focus rectangle</param>
        /// <param name="size">the size of the rectangle around the tap</param>
        /// <returns></returns>
        public async Task TapToFocus(Point position, Size size)
        {
            // Transition to the "focused" state
            _isFocused = true;

            var previewEncodingProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;
            var previewRect = GetPreviewStreamRectInControl(previewEncodingProperties, PreviewControl, _displayOrientation);

            // Build UI rect that will highlight the tapped area
            FocusRectangle.Width = size.Width;
            FocusRectangle.Height = size.Height;

            // Adjust for the rect to be centered around the tap position 
            var left = position.X - FocusRectangle.Width / 2;
            var top = position.Y - FocusRectangle.Height / 2;

            // Move it so it's contained within the preview stream in the UI and doesn't reach into letterboxing area or outside of window bounds

            // The left and right edges should not be outside the active preview area within the CaptureElement
            left = Math.Max(previewRect.Left, left);
            left = Math.Min(previewRect.Width - FocusRectangle.Width + previewRect.Left, left);

            // The top and bottom edges should not be outside the active preview area within the CaptureElement
            top = Math.Max(previewRect.Top, top);
            top = Math.Min(previewRect.Height - FocusRectangle.Height + previewRect.Top, top);

            // Apply the adjusted position to the FocusRectangle
            Canvas.SetLeft(FocusRectangle, left);
            Canvas.SetTop(FocusRectangle, top);

            FocusRectangle.Stroke = new SolidColorBrush(Colors.White);
            FocusRectangle.Visibility = Visibility.Visible;

            // FocusRectangle exists in UI coordinates, need to convert to preview coordinates and adjust for rotation if necessary
            var focusPreview = ConvertUiTapToPreviewRect(position, size, previewRect);

            // Note that this Region Of Interest could be configured to also calculate exposure and white balance within the region
            var regionOfInterest = new RegionOfInterest
            {
                AutoFocusEnabled = true,
                BoundsNormalized = true,
                Bounds = focusPreview,
                Type = RegionOfInterestType.Unknown,
                Weight = 100,
            };

            var result = await FocusCamera(regionOfInterest);

            // Update the UI based on the result of the focusing operation
            FocusRectangle.Stroke = (result == MediaCaptureFocusState.Focused ? new SolidColorBrush(Colors.Lime) : new SolidColorBrush(Colors.Red));
        }

        private async Task TapUnfocus()
        {
            _isFocused = false;
            FocusRectangle.Visibility = Visibility.Collapsed;
            await FocusCamera(null);
        }

        /// <summary>
        /// Sets camera to focus on the passed in region of interest
        /// </summary>
        /// <param name="region">The region to focus on, or null to focus on the default region</param>
        /// <returns></returns>
        private async Task<MediaCaptureFocusState> FocusCamera(RegionOfInterest region)
        {
            var roiControl = _mediaCapture.VideoDeviceController.RegionsOfInterestControl;
            var focusControl = _mediaCapture.VideoDeviceController.FocusControl;

            if (region != null)
            {
                // If the call provided a region, then set it
                await roiControl.SetRegionsAsync(new[] { region }, true);

                var focusRange = focusControl.SupportedFocusRanges.Contains(AutoFocusRange.FullRange) ? AutoFocusRange.FullRange : focusControl.SupportedFocusRanges.FirstOrDefault();
                var focusMode = focusControl.SupportedFocusModes.Contains(FocusMode.Single) ? FocusMode.Single : focusControl.SupportedFocusModes.FirstOrDefault();

                var settings = new FocusSettings { Mode = focusMode, AutoFocusRange = focusRange };

                focusControl.Configure(settings);
            }
            else
            {
                // If no region provided, clear any regions and reset focus
                await roiControl.ClearRegionsAsync();
            }

            await focusControl.FocusAsync();

            return focusControl.FocusState;
        }

        /// <summary>
        /// Applies the necessary rotation to a tap on a CaptureElement (with Stretch mode set to Uniform) to account for device orientation
        /// </summary>
        /// <param name="tap">The location, in UI coordinates, of the user tap</param>
        /// <param name="size">The size, in UI coordinates, of the desired focus rectangle</param>
        /// <param name="previewRect">The area within the CaptureElement that is actively showing the preview, and is not part of the letterboxed area</param>
        /// <returns>A Rect that can be passed to the MediaCapture Focus and RegionsOfInterest APIs, with normalized bounds in the orientation of the native stream</returns>
        private Rect ConvertUiTapToPreviewRect(Point tap, Size size, Rect previewRect)
        {
            // Adjust for the resulting focus rectangle to be centered around the position
            double left = tap.X - size.Width / 2, top = tap.Y - size.Height / 2;

            // Get the information about the active preview area within the CaptureElement (in case it's letterboxed)
            double previewWidth = previewRect.Width, previewHeight = previewRect.Height;
            double previewLeft = previewRect.Left, previewTop = previewRect.Top;

            // Transform the left and top of the tap to account for rotation
            switch (_displayOrientation)
            {
                case DisplayOrientations.Portrait:
                    var tempLeft = left;

                    left = top;
                    top = previewRect.Width - tempLeft;
                    break;
                case DisplayOrientations.LandscapeFlipped:
                    left = previewRect.Width - left;
                    top = previewRect.Height - top;
                    break;
                case DisplayOrientations.PortraitFlipped:
                    var tempTop = top;

                    top = left;
                    left = previewRect.Width - tempTop;
                    break;
            }

            // For portrait orientations, the information about the active preview area needs to be rotated
            if (_displayOrientation == DisplayOrientations.Portrait || _displayOrientation == DisplayOrientations.PortraitFlipped)
            {
                previewWidth = previewRect.Height;
                previewHeight = previewRect.Width;
                previewLeft = previewRect.Top;
                previewTop = previewRect.Left;
            }

            // Normalize width and height of the focus rectangle
            var width = size.Width / previewWidth;
            var height = size.Height / previewHeight;

            // Shift rect left and top to be relative to just the active preview area
            left -= previewLeft;
            top -= previewTop;

            // Normalize left and top
            left /= previewWidth;
            top /= previewHeight;

            // Ensure rectangle is fully contained within the active preview area horizontally
            left = Math.Max(left, 0);
            left = Math.Min(1 - width, left);

            // Ensure rectangle is fully contained within the active preview area vertically
            top = Math.Max(top, 0);
            top = Math.Min(1 - height, top);

            // Create and return resulting rectangle
            return new Rect(left, top, width, height);
        }

    }
}
