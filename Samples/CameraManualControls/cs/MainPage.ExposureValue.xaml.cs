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
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml;

namespace CameraManualControls
{
    public sealed partial class MainPage : Page
    {

        private void UpdateEvControlCapabilities()
        {
            var exposureCompensationControl = _mediaCapture.VideoDeviceController.ExposureCompensationControl;

            if (exposureCompensationControl.Supported)
            {
                EvButton.Tag = Visibility.Visible;

                var value = exposureCompensationControl.Value;

                // The slider will track the number of steps
                var stepSize = exposureCompensationControl.Step;
                EvSlider.Minimum = exposureCompensationControl.Min / stepSize;
                EvSlider.Maximum = exposureCompensationControl.Max / stepSize;
                EvSlider.StepFrequency = 1;
                EvSlider.Value = value / stepSize;

                EvTextBlock.Text = EvStepCountToString((int)Math.Round(value), stepSize);
            }
            else
            {
                EvButton.Visibility = Visibility.Collapsed;
                EvButton.Tag = Visibility.Collapsed;
            }
        }

        private async void EvSlider_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (_settingUpUi) return;

            float stepSize = _mediaCapture.VideoDeviceController.ExposureCompensationControl.Step;

            var value = (sender as Slider).Value;
            await _mediaCapture.VideoDeviceController.ExposureCompensationControl.SetValueAsync((float)value * stepSize);

            EvTextBlock.Text = EvStepCountToString((int)Math.Round(value), stepSize);
        }

        /// <summary>
        /// Converts the number of steps into a (mixed) fraction based on the step size, when the value used is steps * stepSize
        /// </summary>
        /// <param name="steps">Number of steps</param>
        /// <param name="stepSize">The size of one step</param>
        /// <returns></returns>
        private static string EvStepCountToString(int steps, float stepSize)
        {
            if (stepSize <= 0) throw new ArgumentOutOfRangeException(nameof(stepSize), "Parameter must be a positive value");

            int stepsPerUnit = (int)Math.Round(1 / stepSize);
            if (steps == 0) return "0";

            string sign = steps < 0 ? "-" : "+";
            int whole = (int)Math.Abs(steps * stepSize);
            int remainder = (stepsPerUnit >= 1) ? Math.Abs(steps) % stepsPerUnit : 0;

            if (remainder == 0) return sign + whole.ToString();

            string fraction = remainder.ToString() + "/" + stepsPerUnit.ToString();

            if (whole == 0) return sign + fraction;
            return sign + whole.ToString() + " " + fraction;
        }
    }
}
