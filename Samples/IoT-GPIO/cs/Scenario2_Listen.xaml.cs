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
using Windows.Devices.Gpio;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Scenario describing how to listen for changes to a GPIO pin value
    /// </summary>
    public sealed partial class Scenario2_Listen : Page
    {
        // Use GPIO pin 5 to set values
        private const int SET_PIN = 5;
        private GpioPin setPin;

        // Use GPIO pin 6 to listen for value changes
        private const int LISTEN_PIN = 6;
        private GpioPin listenPin;

        private GpioPinValue currentValue = GpioPinValue.High;
        private DispatcherTimer timer;

        public Scenario2_Listen()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            StopScenario();
        }

        void StartScenario()
        {
            // Initialize the GPIO objects.
            var gpio = GpioController.GetDefault();

            // Set up our GPIO pin for setting values.
            // If this next line crashes with a NullReferenceException,
            // then the problem is that there is no GPIO controller on the device.
            setPin = gpio.OpenPin(SET_PIN);

            // Establish initial value and configure pin for output.
            setPin.Write(currentValue);
            setPin.SetDriveMode(GpioPinDriveMode.Output);

            // Set up our GPIO pin for listening for value changes.
            listenPin = gpio.OpenPin(LISTEN_PIN);

            // Configure pin for input and add ValueChanged listener.
            listenPin.SetDriveMode(GpioPinDriveMode.Input);
            listenPin.ValueChanged += Pin_ValueChanged;

            // Start toggling the pin value every 500ms.
            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromMilliseconds(500);
            timer.Tick += Timer_Tick;
            timer.Start();
        }

        void StopScenario()
        {
            // Stop the timer.
            if (timer != null)
            {
                timer.Stop();
                timer = null;
            }

            // Release the GPIO pins.
            if (setPin != null)
            {
                setPin.Dispose();
                setPin = null;
            }
            if (listenPin != null)
            {
                listenPin.ValueChanged -= Pin_ValueChanged;
                listenPin.Dispose();
                listenPin = null;
            }
        }

        void StartStopScenario()
        {
            if (timer != null)
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
        private void Pin_ValueChanged(GpioPin sender, GpioPinValueChangedEventArgs e)
        {
            // Report the change in pin value.
            var task = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                CurrentPinValue.Text = listenPin.Read().ToString();
            });
        }

        private void Timer_Tick(object sender, object e)
        {
            // Toggle the existing pin value.
            currentValue = (currentValue == GpioPinValue.High) ? GpioPinValue.Low : GpioPinValue.High;
            setPin.Write(currentValue);
        }
    }
}
