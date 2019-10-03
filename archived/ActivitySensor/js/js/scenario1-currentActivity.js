//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var scenarioOutput;

    var page = WinJS.UI.Pages.define("/html/scenario1-currentActivity.html", {
        ready: function (element, options) {
            scenarioOutput = document.getElementById("scenarioOutput");

            document.getElementById("getCurrentActivity").addEventListener("click", getCurrentActivity, false);
        }
    });

    /// <summary>
    /// This is the click handler for the 'Get current activity' button.
    /// </summary>
    function getCurrentActivity() {

        // Reset fields and status
        scenarioOutput.style.display = "none";
        WinJS.log && WinJS.log("", "sample", "status");

        Windows.Devices.Sensors.ActivitySensor.getDefaultAsync().done(function (activitySensor) {
            if (activitySensor) {
                activitySensor.getCurrentReadingAsync().done(function (reading) {
                    ActivitySensorSdkSample.formatActivity(reading, scenarioOutput, "#activity", "#confidence", "#timestamp");
                    scenarioOutput.style.display = "block";
                });
            } else {
                WinJS.log && WinJS.log("No activity sensor found", "sample", "error");
            }
        });
    }

})();
