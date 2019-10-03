//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    var pin;

    var scenarioControls;
    var currentPinValue;

    var page = WinJS.UI.Pages.define("/html/scenario1-getandsetpin.html", {
        ready: function (element, options) {
            document.getElementById("startStopButton").addEventListener("click", startStopScenario);
            document.getElementById("setHighButton").addEventListener("click", setPinHigh);
            document.getElementById("setLowButton").addEventListener("click", setPinLow);
            document.getElementById("getValueButton").addEventListener("click", getPinValue);

            scenarioControls = document.getElementById("scenarioControls");
            currentPinValue = document.getElementById("currentPinValue");
        },

        unload: function () {
            stopScenario();
        }
    });

    function startScenario() {
        var gpioController = Windows.Devices.Gpio.GpioController.getDefault();

        // Set up our GPIO pin for setting values.
        // If this next line crashes with "Unable to get property 'openPin'
        // of undefined or null reference", then the problem is that there
        // is no GPIO controller on the device.
        pin = gpioController.openPin(5);

        // Configure pin for output.
        pin.setDriveMode(Windows.Devices.Gpio.GpioPinDriveMode.output);
    }

    function stopScenario() {
        // Release the GPIO pin.
        if (pin) {
            pin.close();
            pin = null;
        }
    }

    function startStopScenario(e) {
        if (pin) {
            stopScenario();
            e.srcElement.innerText = "Start";
            scenarioControls.style.display = "none";
        } else {
            startScenario();
            e.srcElement.innerText = "Stop";
            scenarioControls.style.display = "block";
        }
    }

    function setPinHigh() {
        // Set the pin value to High.
        pin.write(Windows.Devices.Gpio.GpioPinValue.high);
    }

    function setPinLow() {
        // Set the pin value to Low.
        pin.write(Windows.Devices.Gpio.GpioPinValue.low);
    }

    function getPinValue() {
        // Change the GUI to reflect the current pin value.
        if (pin.read() == Windows.Devices.Gpio.GpioPinValue.low) {
            currentPinValue.innerText = "Low";
        } else {
            currentPinValue.innerText = "High";
        }
    }

})();
