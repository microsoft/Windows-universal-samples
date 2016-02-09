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
using System.Threading.Tasks;
using System.Collections.Generic;
using Windows.Devices.I2c;
using Windows.Devices.Enumeration;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_ReadData : Page
    {
        private I2cDevice htu21dSensor;
        private DispatcherTimer timer;

        public Scenario1_ReadData()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            StopScenario();
        }

        private async Task StartScenarioAsync()
        {
            string i2cDeviceSelector = I2cDevice.GetDeviceSelector();
            IReadOnlyList<DeviceInformation> devices = await DeviceInformation.FindAllAsync(i2cDeviceSelector);

            // 0x40 was determined by looking at the datasheet for the HTU21D sensor.
            var HTU21D_settings = new I2cConnectionSettings(0x40);

            // If this next line crashes with an ArgumentOutOfRangeException,
            // then the problem is that no I2C devices were found.
            //
            // If the next line crashes with Access Denied, then the problem is
            // that access to the I2C device (HTU21D) is denied.
            //
            // The call to FromIdAsync will also crash if the settings are invalid.
            //
            // FromIdAsync produces null if there is a sharing violation on the device.
            // This will result in a NullReferenceException in Timer_Tick below.
            htu21dSensor = await I2cDevice.FromIdAsync(devices[0].Id, HTU21D_settings);

            // Start the polling timer.
            timer = new DispatcherTimer() { Interval = TimeSpan.FromMilliseconds(500) };
            timer.Tick += Timer_Tick;
            timer.Start();
        }

        void StopScenario()
        {
            if (timer != null)
            {
                timer.Tick -= Timer_Tick;
                timer.Stop();
                timer = null;
            }

            // Release the I2C sensor.
            if (htu21dSensor != null)
            {
                htu21dSensor.Dispose();
                htu21dSensor = null;
            }
        }

        async void StartStopScenario()
        {
            if (timer != null)
            {
                StopScenario();
                StartStopButton.Content = "Start";
                ScenarioControls.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
            else
            {
                StartStopButton.IsEnabled = false;
                await StartScenarioAsync();
                StartStopButton.IsEnabled = true;
                StartStopButton.Content = "Stop";
                ScenarioControls.Visibility = Windows.UI.Xaml.Visibility.Visible;
            }
        }

        private void Timer_Tick(object sender, object e)
        {
            // Read data from I2C.
            var command = new byte[1];
            var humidityData = new byte[2];
            var temperatureData = new byte[2];

            // Read humidity.
            command[0] = 0xE5;

            // If this next line crashes with a NullReferenceException, then
            // there was a sharing violation on the device. (See StartScenarioAsync above.)
            //
            // If this next line crashes for some other reason, then there was
            // an error accessing the device.
            htu21dSensor.WriteRead(command, humidityData);

            // Read temperature.
            command[0] = 0xE3;
            // If this next line crashes, then there was an error accessing the sensor.
            htu21dSensor.WriteRead(command, temperatureData);

            // Calculate and report the humidity.
            var rawHumidityReading = humidityData[0] << 8 | humidityData[1];
            var humidityRatio = rawHumidityReading / (float)65536;
            double humidity = -6 + (125 * humidityRatio);
            CurrentHumidity.Text = humidity.ToString();

            // Calculate and report the temperature.
            var rawTempReading = temperatureData[0] << 8 | temperatureData[1];
            var tempRatio = rawTempReading / (float)65536;
            double temperature = (-46.85 + (175.72 * tempRatio)) * 9 / 5 + 32;
            CurrentTemp.Text = temperature.ToString();
        }
    }
}
