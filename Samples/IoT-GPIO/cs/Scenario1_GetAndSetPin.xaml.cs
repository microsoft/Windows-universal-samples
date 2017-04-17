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

using Windows.Devices.Gpio;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_GetAndSetPin : Page
    {
        // Use GPIO pin 5 to set values
        private const int LED_PIN = 5;
        private GpioPin pin;

        public Scenario1_GetAndSetPin()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            StopScenario();
        }

        void StartScenario()
        {
            var gpio = GpioController.GetDefault();

            // Set up our GPIO pin for setting values.
            // If this next line crashes with a NullReferenceException,
            // then the problem is that there is no GPIO controller on the device.
            pin = gpio.OpenPin(LED_PIN);

            // Configure pin for output.
            pin.SetDriveMode(GpioPinDriveMode.Output);
        }

        void StopScenario()
        {
            // Release the GPIO pin.
            if (pin != null)
            {
                pin.Dispose();
                pin = null;
            }
        }

        void StartStopScenario()
        {
            if (pin != null)
            {
                StopScenario();
                StartStopButton.Content = "Start";
                ScenarioControls.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
            else
            {
                StartScenario();
                StartStopButton.Content = "Stop";
                ScenarioControls.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
        }

        private void SetPinHigh()
        {
            // Set the pin value to High.
            pin.Write(GpioPinValue.High);
        }

        private void SetPinLow()
        {
            // Set the pin value to Low.
            pin.Write(GpioPinValue.Low);
        }

        private void GetPinValue()
        {
            // Change the GUI to reflect the current pin value.
            CurrentPinValue.Text = pin.Read().ToString();
        }
    }
}
