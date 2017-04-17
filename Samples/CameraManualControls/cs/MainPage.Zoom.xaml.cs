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

        private void UpdateZoomControlCapabilities()
        {
            var zoomControl = _mediaCapture.VideoDeviceController.ZoomControl;

            if (zoomControl.Supported)
            {
                ZoomButton.Tag = Visibility.Visible;

                // Unhook the event handler, so that changing properties on the slider won't trigger an API call
                ZoomSlider.ValueChanged -= ZoomSlider_ValueChanged;

                var value = zoomControl.Value;
                ZoomSlider.Minimum = zoomControl.Min;
                ZoomSlider.Maximum = zoomControl.Max;
                ZoomSlider.StepFrequency = zoomControl.Step;
                ZoomSlider.Value = value;

                ZoomSlider.ValueChanged += ZoomSlider_ValueChanged;
            }
            else
            {
                ZoomButton.Visibility = Visibility.Collapsed;
                ZoomButton.Tag = Visibility.Collapsed;
            }
        }

        private void ZoomSlider_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (_settingUpUi) return;

            SetZoomLevel((float)ZoomSlider.Value);
        }

        private void SetZoomLevel(float level)
        {
            var zoomControl = _mediaCapture.VideoDeviceController.ZoomControl;

            // Make sure zoomFactor is within the valid range
            level = Math.Max(Math.Min(level, zoomControl.Max), zoomControl.Min);

            // Make sure zoomFactor is a multiple of Step, snap to the next lower step
            level -= (level % zoomControl.Step);

            var settings = new ZoomSettings { Value = level };

            if (zoomControl.SupportedModes.Contains(ZoomTransitionMode.Smooth))
            {
                // Favor smooth zoom for this sample
                settings.Mode = ZoomTransitionMode.Smooth;
            }
            else
            {
                settings.Mode = zoomControl.SupportedModes.First();
            }

            zoomControl.Configure(settings);
        }

    }
}
