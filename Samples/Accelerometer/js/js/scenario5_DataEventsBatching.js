//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var accelerometer;

    // DOM elements
    var scenarioEnable;
    var scenarioDisable;
    var eventOutput;

    var page = WinJS.UI.Pages.define("/html/scenario5_DataEventsBatching.html", {
        ready: function (element, options) {
            scenarioEnable = document.getElementById("scenarioEnable");
            scenarioDisable = document.getElementById("scenarioDisable");
            eventOutput = document.getElementById("eventOutput");

            scenarioEnable.addEventListener("click", enableReadingChangedScenario, false);
            scenarioDisable.addEventListener("click", disableReadingChangedScenario, false);

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
                accelerometer.addEventListener("readingchanged", onDataChanged);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                accelerometer.removeEventListener("readingchanged", onDataChanged);
            }
        }
    }

    function onDataChanged(e) {
        SdkSample.setReadingText(eventOutput, e.reading);
    }

    function enableReadingChangedScenario() {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        var reportInterval = Math.max(accelerometer.minimumReportInterval, 16);
        accelerometer.reportInterval = reportInterval;

        // MaxBatchSize will be 0 if the accelerometer does not support batching.
        var reportLatency = Math.min(accelerometer.MaxBatchSize * reportInterval, 10000);

        // Set the report latency. This is a no-op if the accelerometer does not support batching
        accelerometer.reportLatency = reportLatency;

        document.addEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometer.addEventListener("readingchanged", onDataChanged);
        scenarioEnable.disabled = true;
        scenarioDisable.disabled = false;
    }

    function disableReadingChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometer.removeEventListener("readingchanged", onDataChanged);

        // Return the report interval to its default to release resources while the sensor is not in use
        accelerometer.reportInterval = 0;

        scenarioEnable.disabled = false;
        scenarioDisable.disabled = true;
    }
})();
