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

using System.Collections.Generic;
using Windows.Devices.Haptics;
using Windows.Devices.Input;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

// This sample shows how to query the pen currently providing input to the system to see
// if it supports tactile feedback and, if so, what features it supports
namespace SDKTemplate
{
    public sealed partial class Scenario2_SupportedTactileFeedback : Page
    {
        SimpleHapticsController hapticsController;
        PenDevice penDevice;

        public Scenario2_SupportedTactileFeedback()
        {
            InitializeComponent();
        }

        // The PointerEnter event will get fired as soon as Pointer input is received.
        // This event handler implementation will query the device providing input to see if it's a pen and
        // then check to see the pen supports tactile feedback and, if so, what features it supports
        private void Pointer_Entered(object sender, PointerRoutedEventArgs e)
        {
            // If the current Pointer device is not a pen, exit
            if (e.Pointer.PointerDeviceType != PointerDeviceType.Pen)
            {
                supportedFeatures.Text = "";
                supportedFeedback.Text = "";
                return;
            }

            // Attempt to retrieve the PenDevice from the current PointerId
            penDevice = PenDevice.GetFromPointerId(e.Pointer.PointerId);

            // If a PenDevice cannot be retrieved based on the PointerId, it does not support
            // advanced pen features, such as tactile feedback
            if (penDevice == null)
            {
                statusText.Text = "Advanced pen features not supported";
                supportedFeatures.Text = "";
                supportedFeedback.Text = "";
                return;
            }

            // Check to see if the current PenDevice supports tactile feedback by seeing if it
            // has a SimpleHapticsController
            hapticsController = penDevice.SimpleHapticsController;
            if (hapticsController == null)
            {
                statusText.Text = "This pen does not provide tactile feedback";
                return;
            }

            // Check which tactile feedback features are supported
            supportedFeatures.Text = "Supported Haptics Features:\n";
            if (hapticsController.IsIntensitySupported)
            {
                supportedFeatures.Text += "Intensity\n";
            }
            if (hapticsController.IsPlayCountSupported)
            {
                supportedFeatures.Text += "PlayCount\n";
            }
            if (hapticsController.IsPlayDurationSupported)
            {
                supportedFeatures.Text += "PlayDuration\n";
            }
            if (hapticsController.IsReplayPauseIntervalSupported)
            {
                supportedFeatures.Text += "ReplayPauseInterval\n";
            }

            // Check which feedback waveforms are supported
            supportedFeedback.Text = "Supported Feedback:\n";
            foreach (SimpleHapticsControllerFeedback feedback in hapticsController.SupportedFeedback)
            {
                ushort waveform = feedback.Waveform;
                foreach (KeyValuePair<string, ushort> entry in MainPage.WaveformNamesMap)
                {
                    if (entry.Value == waveform)
                    {
                        supportedFeedback.Text += entry.Key + "\n";
                        break;
                    }
                }
            }
            statusText.Text = "";
        }

        // Clear the current penDevice and hapticsController on PointerExit
        private void Pointer_Exited(object sender, PointerRoutedEventArgs e)
        {
            penDevice = null;
            hapticsController = null;
        }
    }
}
