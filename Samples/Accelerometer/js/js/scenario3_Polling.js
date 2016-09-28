//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var reportInterval = 0;
    var intervalId = 0;
    var accelerometer;

    // DOM elements
    var scenarioEnable;
    var scenarioDisable;
    var readingOutput;

    var page = WinJS.UI.Pages.define("/html/scenario3_Polling.html", {
        ready: function (element, options) {
            scenarioEnable = document.getElementById("scenarioEnable");
            scenarioDisable = document.getElementById("scenarioDisable");
            readingOutput = document.getElementById("readingOutput");

            scenarioEnable.addEventListener("click", enableGetReadingScenario, false);
            scenarioDisable.addEventListener("click", disableGetReadingScenario, false);

            var readingType = SdkSample.accelerometerReadingType;
            accelerometer = Windows.Devices.Sensors.Accelerometer.getDefault(Windows.Devices.Sensors.AccelerometerReadingType[readingType]);
            if (accelerometer) {
                WinJS.log && WinJS.log(readingType + " accelerometer ready", "sample", "status");
                scenarioEnable.disabled = false;
            } else {
                WinJS.log && WinJS.log(readingType + " accelerometer not found", "sample", "error");
            }
        },
        unload: function () {
            if (!scenarioDisable.disabled) {
                disableGetReadingScenario();
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (!scenarioDisable.disabled) {
            if (document.msVisibilityState === "visible") {
                // Resume polling. No need to restore the desired reportInterval (it is restored for us upon app resume)
                intervalId = setInterval(getCurrentReading, reportInterval);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                clearInterval(intervalId);
                intervalId = 0;
            }
        }
    }

    function getCurrentReading() {
        var reading = accelerometer.getCurrentReading();
        if (reading) {
            SdkSample.setReadingText(readingOutput, reading);
        }
    }

    function enableGetReadingScenario() {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        accelerometer.reportInterval = Math.max(accelerometer.minimumReportInterval, 16);

        document.addEventListener("visibilitychange", visibilityChangeHandler, false);
        intervalId = setInterval(getCurrentReading, reportInterval);
        scenarioEnable.disabled = true;
        scenarioDisable.disabled = false;
    }

    function disableGetReadingScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        clearInterval(intervalId);

        // Return the report interval to its default to release resources while the sensor is not in use
        accelerometer.reportInterval = 0;

        scenarioEnable.disabled = false;
        scenarioDisable.disabled = true;
    }
})();
