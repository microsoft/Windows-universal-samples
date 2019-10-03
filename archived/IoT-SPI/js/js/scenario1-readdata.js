//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    var adxl345Sensor;
    var intervalId;

    var scenarioControls;
    var xText;
    var yText;
    var zText;

    var page = WinJS.UI.Pages.define("/html/scenario1-readdata.html", {
        ready: function (element, options) {
            document.getElementById("startStopButton").addEventListener("click", startStopScenario);

            scenarioControls = document.getElementById("scenarioControls");

            xText = document.getElementById("X");
            yText = document.getElementById("Y");
            zText = document.getElementById("Z");
        },

        unload: function () {
            stopScenario();
        }
    });

    function startScenarioAsync() {
        var spiDeviceSelector = Windows.Devices.Spi.SpiDevice.getDeviceSelector();
        return Windows.Devices.Enumeration.DeviceInformation.findAllAsync(spiDeviceSelector, null).then(function (devices) {

            // 0 = Chip select line to use
            var adxl345_Settings = new Windows.Devices.Spi.SpiConnectionSettings(0);

            // 5MHz is the rated speed of the ADXL345 accelerometer.
            adxl345_Settings.clockFrequency = 5000000;

            // The accelerometer expects an idle-high clock polarity.
            // We use Mode3 to set the clock polarity and phase to: CPOL = 1, CPHA = 1.
            adxl345_Settings.mode = Windows.Devices.Spi.SpiMode.mode3;

            // If this next line crashes with "Unable to get property 'id'
            // of undefined or null reference", then the problem is that no
            // SPI devices were found.
            //
            // If the next line crashes with Access Denied, then the problem is
            // that access to the SPI device is denied.
            //
            // The call to fromIdAsync will also crash if the settings are invalid.
            return Windows.Devices.Spi.SpiDevice.fromIdAsync(devices[0].id, adxl345_Settings);
        }).then(function (spiDevice) {

            // spiDevice will be null if there is a sharing violation on the device.
            // This will result in a null reference exception a few lines later.

            adxl345Sensor = spiDevice;

            //
            // Initialize the accelerometer:
            //
            // For this device, we create 2-byte write buffers:
            // The first byte is the register address we want to write to.
            // The second byte is the contents that we want to write to the register. 
            //
            var WriteBuf_DataFormat = new Uint8Array(2);
            WriteBuf_DataFormat[0] = 0x31; // 0x31 is address of data format register
            WriteBuf_DataFormat[1] = 0x01; // 0x01 sets range to +- 4Gs

            var WriteBuf_PowerControl = new Uint8Array(2);
            WriteBuf_PowerControl[0] = 0x2D; // 0x2D is address of power control register
            WriteBuf_PowerControl[1] = 0x08; // 0x08 puts the accelerometer into measurement mode

            // Write the register settings.
            //
            // If this next line crashes with "Unable to get property 'write'
            // of undefined or null reference", then the problem is that there
            // was a sharing violation on the device. (See comment earlier in this function.)
            //
            // If this next line crashes for some other reason, then there was
            // an error communicating with the device.
            adxl345Sensor.write(WriteBuf_DataFormat);
            adxl345Sensor.write(WriteBuf_PowerControl);

            // Start the polling timer.
            intervalId = setInterval(readSpi, 100);
        });
    }

    function stopScenario() {
        if (intervalId) {
            clearInterval(intervalId);
            intervalId = null;
        }

        if (adxl345Sensor) {
            // Release the SPI device
            adxl345Sensor.close();
            adxl345Sensor = null;
        }
    }

    function startStopScenario(e) {
        if (intervalId) {
            stopScenario();
            e.srcElement.innerText = "Start";
            scenarioControls.style.display = "none";
        } else {
            e.srcElement.disabled = true;
            startScenarioAsync().done(function () {
                e.srcElement.disabled = false;
                e.srcElement.innerText = "Stop";
                scenarioControls.style.display = "block";
            });
        }
    }

    function toInt16(buffer, startIndex) {
        // First calculate the unsigned 16-bit value.
        var unsigned = (buffer[startIndex + 1] << 8) | buffer[startIndex];
        // Now reinterpret it as a signed 16-bit value.
        var signed = (unsigned << 16) >> 16;
    }

    function readSpi() {
        var ACCEL_SPI_RW_BIT = 0x80; // Bit used in SPI transactions to indicate read/write
        var ACCEL_SPI_MB_BIT = 0x40; // Bit used to indicate multi-byte SPI transactions
        var ACCEL_REG_X = 0x32;      // Address of the X Axis data register
        var ACCEL_RES = 1024;         // The ADXL345 has 10 bit resolution giving 1024 unique values
        var ACCEL_DYN_RANGE_G = 8;    // The ADXL345 had a total dynamic range of 8G, since we're configuring it to +-4G 
        var UNITS_PER_G = ACCEL_RES / ACCEL_DYN_RANGE_G;  // Ratio of raw int values to G units

        var ReadBuf = new Uint8Array(6 + 1); // Read buffer of size 6 bytes (2 bytes * 3 axes) + 1 byte padding
        var RegAddrBuf = new Uint8Array(1 + 6); // Register address buffer of size 1 byte + 6 bytes padding

        // Register address we want to read from with read and multi-byte bit set
        RegAddrBuf[0] = ACCEL_REG_X | ACCEL_SPI_RW_BIT | ACCEL_SPI_MB_BIT;

        // If this next line crashes, then there was an error communicating with the device.
        adxl345Sensor.transferFullDuplex(RegAddrBuf, ReadBuf);

        // In order to get the raw 16-bit data values, we need to concatenate two 8-bit bytes for each axis
        var AccelerationRawX = (ReadBuf[2] << 8) | ReadBuf[1];
        var AccelerationRawY = (ReadBuf[4] << 8) | ReadBuf[3];
        var AccelerationRawZ = (ReadBuf[6] << 8) | ReadBuf[5];

        var AccelerationRawX = toInt16(ReadBuf, 1);
        var AccelerationRawY = toInt16(ReadBuf, 3);
        var AccelerationRawZ = toInt16(ReadBuf, 5);

        // Convert raw values to G's and display them.
        xText.innerText = "X Axis: " + (AccelerationRawX / UNITS_PER_G);
        yText.innerText = "Y Axis: " + (AccelerationRawY / UNITS_PER_G);
        zText.innerText = "Z Axis: " + (AccelerationRawZ / UNITS_PER_G);
    }
})();
