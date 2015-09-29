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
using System.Threading.Tasks;
using Windows.Devices.Spi;
using Windows.Devices.Enumeration;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace SDKTemplate
{
    public sealed partial class Scenario1_ReadData : Page
    {
        private SpiDevice adxl345Sensor;
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
            String spiDeviceSelector = SpiDevice.GetDeviceSelector();
            IReadOnlyList<DeviceInformation> devices = await DeviceInformation.FindAllAsync(spiDeviceSelector);

            // 0 = Chip select line to use.
            var ADXL345_Settings = new SpiConnectionSettings(0);

            // 5MHz is the rated speed of the ADXL345 accelerometer.
            ADXL345_Settings.ClockFrequency = 5000000;

            // The accelerometer expects an idle-high clock polarity.
            // We use Mode3 to set the clock polarity and phase to: CPOL = 1, CPHA = 1.
            ADXL345_Settings.Mode = SpiMode.Mode3;

            // If this next line crashes with an ArgumentOutOfRangeException,
            // then the problem is that no SPI devices were found.
            //
            // If the next line crashes with Access Denied, then the problem is
            // that access to the SPI device (ADXL345) is denied.
            //
            // The call to FromIdAsync will also crash if the settings are invalid.
            //
            // FromIdAsync produces null if there is a sharing violation on the device.
            // This will result in a NullReferenceException a few lines later.
            adxl345Sensor = await SpiDevice.FromIdAsync(devices[0].Id, ADXL345_Settings);

            // 
            // Initialize the accelerometer:
            //
            // For this device, we create 2-byte write buffers:
            // The first byte is the register address we want to write to.
            // The second byte is the contents that we want to write to the register. 
            //

            // 0x31 is address of data format register, 0x01 sets range to +- 4Gs.
            byte[] WriteBuf_DataFormat = new byte[] { 0x31, 0x01 };

            // 0x2D is address of power control register, 0x08 puts the accelerometer into measurement mode.
            byte[] WriteBuf_PowerControl = new byte[] { 0x2D, 0x08 };

            // Write the register settings.
            //
            // If this next line crashes with a NullReferenceException, then
            // there was a sharing violation on the device.
            // (See comment earlier in this function.)
            //
            // If this next line crashes for some other reason, then there was
            // an error communicating with the device.

            adxl345Sensor.Write(WriteBuf_DataFormat);
            adxl345Sensor.Write(WriteBuf_PowerControl);

            // Start the polling timer.
            timer = new DispatcherTimer() { Interval = TimeSpan.FromMilliseconds(100) };
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

            // Release the SPI sensor.
            if (adxl345Sensor != null)
            {
                adxl345Sensor.Dispose();
                adxl345Sensor = null;
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

        void Timer_Tick(object sender, object e)
        {
            const byte ACCEL_SPI_RW_BIT = 0x80; // Bit used in SPI transactions to indicate read/write
            const byte ACCEL_SPI_MB_BIT = 0x40; // Bit used to indicate multi-byte SPI transactions
            const byte ACCEL_REG_X = 0x32;      // Address of the X Axis data register
            const int ACCEL_RES = 1024;         // The ADXL345 has 10 bit resolution giving 1024 unique values
            const int ACCEL_DYN_RANGE_G = 8;    // The ADXL345 had a total dynamic range of 8G, since we're configuring it to +-4G 
            const int UNITS_PER_G = ACCEL_RES / ACCEL_DYN_RANGE_G;  // Ratio of raw int values to G units

            byte[] ReadBuf = new byte[6 + 1];    // Read buffer of size 6 bytes (2 bytes * 3 axes) + 1 byte padding
            byte[] RegAddrBuf = new byte[1 + 6]; // Register address buffer of size 1 byte + 6 bytes padding

            // Register address we want to read from with read and multi-byte bit set
            RegAddrBuf[0] = ACCEL_REG_X | ACCEL_SPI_RW_BIT | ACCEL_SPI_MB_BIT;

            // If this next line crashes, then there was an error communicating with the device.
            adxl345Sensor.TransferFullDuplex(RegAddrBuf, ReadBuf);

            // In order to get the raw 16-bit data values, we need to concatenate two 8-bit bytes for each axis
            short AccelerationRawX = BitConverter.ToInt16(ReadBuf, 1);
            short AccelerationRawY = BitConverter.ToInt16(ReadBuf, 3);
            short AccelerationRawZ = BitConverter.ToInt16(ReadBuf, 5);

            // Convert raw values to G's and display them.
            X.Text = ((double)AccelerationRawX / UNITS_PER_G).ToString();
            Y.Text = ((double)AccelerationRawY / UNITS_PER_G).ToString();
            Z.Text = ((double)AccelerationRawZ / UNITS_PER_G).ToString();
        }
    }
}
