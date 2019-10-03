//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var activitySensor;

    var enableReadingChanged;
    var disableReadingChanged;
    var scenarioOutput;

    var page = WinJS.UI.Pages.define("/html/scenario3-changeEvents.html", {
        ready: function (element, options) {
            enableReadingChanged = document.getElementById("enableReadingChanged");
            disableReadingChanged = document.getElementById("disableReadingChanged");
            scenarioOutput = document.getElementById("scenarioOutput");

            enableReadingChanged.addEventListener("click", enableReadingChangedScenario, false);
            disableReadingChanged.addEventListener("click", disableReadingChangedScenario, false);
        },
        unload: function () {
            if (activitySensor) {
                activitySensor.removeEventListener("readingchanged", onReadingChanged);
            }
        }
    });

    /// <summary>
    /// This is the event handler for ReadingChanged events.
    /// </summary>
    /// <param name="e"></param>
    function onReadingChanged(e) {
        var reading = e.reading;
        ActivitySensorSdkSample.formatActivity(reading, scenarioOutput, "#activity", "#confidence", "#timestamp");
    }

    /// <summary>
    /// This is the click handler for the 'Reading Changed On' button.
    /// </summary>
    function enableReadingChangedScenario() {
        enableReadingChanged.disabled = true;
        Windows.Devices.Sensors.ActivitySensor.getDefaultAsync().done(function (sensor) {
            if (sensor) {
                activitySensor = sensor;

                activitySensor.subscribedActivities.push(
                    Windows.Devices.Sensors.ActivityType.walking,
                    Windows.Devices.Sensors.ActivityType.running,
                    Windows.Devices.Sensors.ActivityType.inVehicle,
                    Windows.Devices.Sensors.ActivityType.biking,
                    Windows.Devices.Sensors.ActivityType.fidgeting);

                activitySensor.addEventListener("readingchanged", onReadingChanged);
                disableReadingChanged.disabled = false;
            } else {
                WinJS.log && WinJS.log("No activity sensor found", "sample", "error");
                enableReadingChanged.disabled = false;
            }
        });
    }

    /// <summary>
    /// This is the click handler for the 'Reading Changed Off' button.
    /// </summary>
    function disableReadingChangedScenario() {
        activitySensor.removeEventListener("readingchanged", onReadingChanged);
        activitySensor = null;
        enableReadingChanged.disabled = false;
        disableReadingChanged.disabled = true;
    }
})();
