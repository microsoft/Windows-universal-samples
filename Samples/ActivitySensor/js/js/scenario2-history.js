//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var scenarioOutput;

    var page = WinJS.UI.Pages.define("/html/scenario2-history.html", {
        ready: function (element, options) {
            scenarioOutput = document.getElementById("scenarioOutput");

            document.getElementById("getActivityHistory").addEventListener("click", getActivityHistory, false);
        }
    });

    /// <summary>
    /// This is the click handler for the 'Get activity history' button.
    /// </summary>
    function getActivityHistory() {

        // Reset fields and status
        scenarioOutput.style.display = "none";
        WinJS.log && WinJS.log("", "sample", "status");

        var calendar = new Windows.Globalization.Calendar();
        calendar.setToNow();
        calendar.addDays(-1);
        var yesterday = calendar.getDateTime();

        // Get history from yesterday onwards
        var history = Windows.Devices.Sensors.ActivitySensor.getSystemHistoryAsync(yesterday).done(function (history) {
            if (history.length > 0) {
                var reading1 = history[0];
                ActivitySensorSdkSample.formatActivity(reading1, scenarioOutput, "#activity1", "#confidence1", "#timestamp1");

                var readingN = history[history.length - 1];
                ActivitySensorSdkSample.formatActivity(readingN, scenarioOutput, "#activityN", "#confidenceN", "#timestampN");

                scenarioOutput.style.display = "block";

            }
            WinJS.log && WinJS.log("Readings retrieved: " + history.length, "sample", "status");
        });
    }

})();
