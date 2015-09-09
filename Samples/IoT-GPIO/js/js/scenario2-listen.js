//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    var currentValue = Windows.Devices.Gpio.GpioPinValue.high;
    var listenPin;
    var setPin;
    var intervalId;

    var scenarioControls;
    var currentPinValue;

    var page = WinJS.UI.Pages.define("/html/scenario2-listen.html", {
        ready: function (element, options) {
            document.getElementById("startStopButton").addEventListener("click", startStopScenario);

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
        setPin = gpioController.openPin(5);

        // Establish initial value and configure pin for output.
        setPin.write(currentValue);
        setPin.setDriveMode(Windows.Devices.Gpio.GpioPinDriveMode.output);

        // Set up our GPIO pin for listening for value changes.
        listenPin = gpioController.openPin(6);

        // Configure pin for input and add ValueChanged listener.
        listenPin.setDriveMode(Windows.Devices.Gpio.GpioPinDriveMode.input);
        listenPin.addEventListener("valuechanged", valueChanged);

        // Start toggling the pin value every 500ms.
        intervalId = setInterval(flipLed, 500);
    }

    function stopScenario() {
        if (intervalId) {
            clearInterval(intervalId);
            intervalId = null;
        }
        if (listenPin) {
            listenPin.removeEventListener("valuechanged", valueChanged);
            listenPin.close();
            listenPin = null;
        }
        if (setPin) {
            setPin.close();
            setPin = null;
        }
    }

    function startStopScenario(e) {
        if (intervalId) {
            stopScenario();
            e.srcElement.innerText = "Start";
            scenarioControls.style.display = "none";
        } else {
            startScenario();
            e.srcElement.innerText = "Stop";
            scenarioControls.style.display = "block";
        }
    }

    function valueChanged(args) {
        // Report the change in pin value.
        if (listenPin.read() == Windows.Devices.Gpio.GpioPinValue.low) {
            currentPinValue.innerText = "Low";
        } else {
            currentPinValue.innerText = "High";
        }
    }

    function flipLed() {
        // Toggle the existing pin value.
        if (currentValue == Windows.Devices.Gpio.GpioPinValue.high) {
            currentValue = Windows.Devices.Gpio.GpioPinValue.low;
        } else {
            currentValue = Windows.Devices.Gpio.GpioPinValue.high;
        }

        setPin.write(currentValue);
    }
})();
