//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var intervalId = 0;
    var sensor;

    // DOM elements
    var scenarioEnable;
    var scenarioDisable;
    var output;

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            scenarioEnable = document.getElementById("scenarioEnable");
            scenarioDisable = document.getElementById("scenarioDisable");
            output = document.getElementById("output");

            scenarioEnable.addEventListener("click", enableGetReadingScenario, false);
            scenarioDisable.addEventListener("click", disableGetReadingScenario, false);

            var readingType = Windows.Devices.Sensors.SensorReadingType[SdkSample.sensorReadingType];
            var optimizationGoal = Windows.Devices.Sensors.SensorOptimizationGoal[SdkSample.sensorOptimizationGoal];
            sensor = Windows.Devices.Sensors.OrientationSensor.getDefault(readingType, optimizationGoal);
            if (sensor) {
                WinJS.log && WinJS.log(SdkSample.sensorDescription() + " is ready", "sample", "status");
                scenarioEnable.disabled = false;
            } else {
                WinJS.log && WinJS.log(SdkSample.sensorDescription() + " not found", "sample", "error");
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
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                intervalId = setInterval(getCurrentReading, reportInterval);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                clearInterval(intervalId);
            }
        }
    }

    function getCurrentReading() {
        var reading = sensor.getCurrentReading();
        if (reading) {
            SdkSample.setReadingText(output, reading);
        }
    }

    function enableGetReadingScenario() {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        var reportInterval = Math.max(sensor.minimumReportInterval, 16);

        // Set the report interval to enable the sensor for polling
        sensor.reportInterval = reportInterval;

        document.addEventListener("visibilitychange", visibilityChangeHandler, false);
        intervalId = setInterval(getCurrentReading, reportInterval);

        scenarioEnable.disabled = true;
        scenarioDisable.disabled = false;
    }

    function disableGetReadingScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        clearInterval(intervalId);
        intervalId = 0;

        // Return the report interval to its default to release resources while the sensor is not in use
        sensor.reportInterval = 0;

        scenarioEnable.disabled = false;
        scenarioDisable.disabled = true;

    }
})();
