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

using Windows.Devices.Haptics;
using Windows.Devices.Input;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

// This sample shows how to play Interaction feedback tactile signals, how to
// adjust the intensity of the tactile signal, and how to fall back to a default
// waveform if the desired waveform is not supported by the current PenDevice
namespace SDKTemplate
{
    public sealed partial class Scenario4_InteractionFeedback : Page
    {
        SimpleHapticsController hapticsController;
        PenDevice penDevice;
        SimpleHapticsControllerFeedback currentFeedback;

        public Scenario4_InteractionFeedback()
        {
            InitializeComponent();
        }

        // The PointerEnter event will get fired as soon as Pointer input is received.
        // This event handler implementation will query the device providing input to see if it's a pen and
        // then check to see the pen supports tactile feedback
        private void MainGrid_Entered(object sender, PointerRoutedEventArgs e)
        {
            // If the current Pointer device is not a pen, exit
            if (e.Pointer.PointerDeviceType != PointerDeviceType.Pen)
            {
                return;
            }

            // Attempt to retrieve the PenDevice from the current PointerId
            penDevice = PenDevice.GetFromPointerId(e.Pointer.PointerId);

            // If a PenDevice cannot be retrieved based on the PointerId, it does not support
            // advanced pen features, such as tactile feedback
            if (penDevice == null)
            {
                statusText.Text = "Advanced pen features not supported";
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
        }

        // Stop sending the tactile feedback and clear the current penDevice and hapticsController on PointerExit.
        // Stopping the feedback is important as it clears the tactile signal from the PenDevice's
        // SimpleHapticsController, ensuring that it has a clean state once it next enters range.
        private void MainGrid_Exited(object sender, PointerRoutedEventArgs e)
        {
            penDevice = null;
            statusText.Text = "";
            if (hapticsController != null)
            {
                hapticsController.StopFeedback();
                hapticsController = null;
                currentFeedback = null;
            }
        }

        // Send haptic feedback that matches the selected waveform upon the button being clicked
        private void SendFeedback_Clicked(object sender, RoutedEventArgs e)
        {
            if (hapticsController == null)
            {
                return;
            }

            // Get feedback based on the user's selected waveform
            currentFeedback = GetSelectedFeedbackOrFallback(out string message);

            // Send the current feedback to the PenDevice's SimpleHapticsController.
            // Once sent, unlike inking feedback, this feedback will be played immediately.
            // Also, check to see if the current PenDevice's SimpleHapticsController supports
            // setting the intensity value of the tactile feedback.  If so, set it based
            // on the slider.  If not, send the feedback without custom intensity.
            if (hapticsController.IsIntensitySupported)
            {
                hapticsController.SendHapticFeedback(currentFeedback, intensitySlider.Value / 100);
                message += "\nIntensity set to " + intensitySlider.Value + "%";
            }
            else
            {
                hapticsController.SendHapticFeedback(currentFeedback);
                message += "\nSetting intensity is not supported by this pen";
            }
            statusText.Text = message;
        }

        // Get feedback which matches the currently selected waveform in the dropdown.
        // To get the feedback, the SimpleHapticsController's SupportedFeedback list must be traversed
        // to see if a matching feedback is present.
        // This is important because Click is the only Interaction waveform required to be
        // supported by every pen with haptics.
        // If no matching feedback is found in the SupportedFeedback list, this method returns
        // Click as a guaranteed fallback.
        private SimpleHapticsControllerFeedback GetSelectedFeedbackOrFallback(out string message)
        {
            // Look up the waveform the user selected.
            string name = (string)waveformComboBox.SelectionBoxItem;
            ushort waveform = MainPage.WaveformNamesMap[name];

            // See if the haptics controller supports the selected waveform.
            SimpleHapticsControllerFeedback feedback = MainPage.FindSupportedFeedback(hapticsController, waveform);
            if (feedback != null)
            {
                message = "Waveform set to " + name;
                return feedback;
            }

            // It does not. Use Click as a fallback.
            message = name + " is not supported by this pen, so falling back to Click";
            return MainPage.FindSupportedFeedback(hapticsController, KnownSimpleHapticsControllerWaveforms.Click);
        }
    }
}