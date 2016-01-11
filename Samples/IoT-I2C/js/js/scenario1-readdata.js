//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    var htu21dSensor;
    var intervalId;

    var scenarioControls;
    var currentHumidityLabel;
    var currentTempLabel;

    var page = WinJS.UI.Pages.define("/html/scenario1-readdata.html", {
        ready: function (element, options) {
            document.getElementById("startStopButton").addEventListener("click", startStopScenario);

            currentHumidityLabel = document.getElementById("currentHumidity");
            currentTempLabel = document.getElementById("currentTemp");
            scenarioControls = document.getElementById("scenarioControls");
        },

        unload: function () {
            stopScenario();
        }
    });

    function startScenarioAsync() {
        var i2cDeviceSelector = Windows.Devices.I2c.I2cDevice.getDeviceSelector();

        return Windows.Devices.Enumeration.DeviceInformation.findAllAsync(i2cDeviceSelector, null).then(function (infos) {

            // 0x40 was determined by looking at the datasheet for the HTU21D sensor.
            var HTU21D_settings = new Windows.Devices.I2c.I2cConnectionSettings(0x40);

            // If this next line crashes with "Unable to get property 'id'
            // of undefined or null reference", then the problem is that no
            // I2C devices were found.
            //
            // If the next line crashes with Access Denied, then the problem is
            // that access to the I2C device (HTU21D) is denied.
            //
            // The call to fromIdAsync will also crash if the settings are invalid.
            return Windows.Devices.I2c.I2cDevice.fromIdAsync(infos[0].id, HTU21D_settings);
        }).then(function (i2cDevice) {

            // i2cDevice will be null if there is a sharing violation on the device.
            // This will result in a null reference exception in readI2c below.
            htu21dSensor = i2cDevice;

            // Start the polling timer.
            intervalId = setInterval(readI2c, 500);
        });
    }

    function stopScenario() {
        // Stop polling the I2C device.
        if (intervalId) {
            clearInterval(intervalId);
            intervalId = null;
        }

        // Release the I2C device.
        if (htu21dSensor) {
            htu21dSensor.close();
            htu21dSensor = null;
        }
    }

    function startStopScenario(e) {
        if (intervalId) {
            stopScenario();
            e.srcElement.innerText = "Start";
            scenarioControls.style.display = "none";
        } else {
            e.srcElement.disabled = true;
            startScenarioAsync().done(function() {
                e.srcElement.disabled = false;
                e.srcElement.innerText = "Stop";
                scenarioControls.style.display = "block";
            });
        }
    }

    function readI2c() {
        // Read data from I2C.
        var command = new Array(1);
        var humidityData = new Array(2);
        var temperatureData = new Array(2);

        // Read the humidity from the sensor.
        command[0] = 0xE5;

        // If this next line crashes with "Unable to get property 'writeRead'
        // of undefined or null reference", then the problem is that there
        // was a sharing violation on the device. (See startScenarioAsync above.)
        //
        // If this next line crashes for some other reason, then there was
        // an error communicating with the device.
        htu21dSensor.writeRead(command, humidityData);

        // Read the temperature from the sensor.
        command[0] = 0xE3;
        // If this next line crashes, then there was an error communicating with the device.
        htu21dSensor.writeRead(command, temperatureData);

        // Calculate and report the humidity.
        var rawhumidityReading = humidityData[0] << 8 | humidityData[1];
        var ratio = rawhumidityReading / 65536.0;
        var humidity = -6 + (125 * ratio);
        currentHumidityLabel.innerText = humidity;

        // Calculate and report the temperature.
        var rawTempReading = temperatureData[0] << 8 | temperatureData[1];
        var tempRatio = rawTempReading / 65536.0;
        var temperature = (-46.85 + (175.72 * tempRatio)) * 9 / 5 + 32;
        currentTempLabel.innerText = temperature;
    }

})();
