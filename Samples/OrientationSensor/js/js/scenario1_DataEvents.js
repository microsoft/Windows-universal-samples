//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var sensor;

    // DOM elements
    var scenarioEnable;
    var scenarioDisable;
    var output;

    var page = WinJS.UI.Pages.define("/html/scenario1_DataEvents.html", {
        ready: function (element, options) {
            scenarioEnable = document.getElementById("scenarioEnable");
            scenarioDisable = document.getElementById("scenarioDisable");
            output = document.getElementById("output");

            scenarioEnable.addEventListener("click", enableReadingChangedScenario, false);
            scenarioDisable.addEventListener("click", disableReadingChangedScenario, false);

            var readingType = Windows.Devices.Sensors.SensorReadingType[SdkSample.sensorReadingType];
            var optimizationGoal = Windows.Devices.Sensors.SensorOptimizationGoal[SdkSample.sensorOptimizationGoal];
            sensor = Windows.Devices.Sensors.OrientationSensor.getDefault(readingType, optimizationGoal);
            if (sensor) {
                WinJS.log && WinJS.log(SdkSample.sensorDescription() + " is ready", "sample", "status");
                scenarioEnable.disabled = false;
            } else {
                WinJS.log && WinJS.log(SdkSample.sensorDescription() +" not found", "sample", "error");
            }
        },
        unload: function () {
            if (!scenarioDisable.disabled) {
                disableReadingChangedScenario();
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (!scenarioDisable.disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                sensor.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                sensor.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        SdkSample.setReadingText(output, e.reading);
    }

    function enableReadingChangedScenario() {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        sensor.reportInterval = Math.max(sensor.minimumReportInterval, 16);

        document.addEventListener("visibilitychange", visibilityChangeHandler, false);
        sensor.addEventListener("readingchanged", onDataChanged);
        scenarioEnable.disabled = true;
        scenarioDisable.disabled = false;
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        sensor.removeEventListener("readingchanged", onDataChanged);

        // Return the report interval to its default to release resources while the sensor is not in use
        sensor.reportInterval = 0;

        scenarioEnable.disabled = false;
        scenarioDisable.disabled = true;
    }
})();
