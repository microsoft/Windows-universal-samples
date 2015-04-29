//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario2_history.html", {
        ready: function (element, options) {
            document.getElementById("scenario2GetActivityHistory").addEventListener("click", getActivityHistoryScenario, false);
        }
    });

    /// <summary>
    /// This is the click handler for the 'Get Activity History' button.
    /// </summary>
    function getActivityHistoryScenario() {

        // Reset fields and status
        document.getElementById("countOutput").innerText = "no data";
        document.getElementById("activity1Output").innerText = "no data";
        document.getElementById("confidence1Output").innerText = "no data";
        document.getElementById("timestamp1Output").innerText = "no data";
        document.getElementById("activityNOutput").innerText = "no data";
        document.getElementById("confidenceNOutput").innerText = "no data";
        document.getElementById("timestampNOutput").innerText = "no data";
        WinJS.log && WinJS.log("", "sample", "status");

        var calendar = new Windows.Globalization.Calendar();
        calendar.setToNow();
        calendar.addDays(-1);
        var yesterday = calendar.getDateTime();

        // Get history from yesterday onwards
        var history = Windows.Devices.Sensors.ActivitySensor.getSystemHistoryAsync(yesterday).done(function (history) {
            document.getElementById("countOutput").innerText = history.length;
            if (history.length > 0) {
                var reading1 = history[0];
                document.getElementById("activity1Output").innerText = ActivitySensorSdkSample.activityTypeToString(reading1.activity);
                document.getElementById("confidence1Output").innerText = ActivitySensorSdkSample.readingConfidenceToString(reading1.confidence);
                document.getElementById("timestamp1Output").innerText = reading1.timestamp;

                var readingN = history[0];
                document.getElementById("activityNOutput").innerText = ActivitySensorSdkSample.activityTypeToString(readingN.activity);
                document.getElementById("confidenceNOutput").innerText = ActivitySensorSdkSample.readingConfidenceToString(readingN.confidence);
                document.getElementById("timestampNOutput").innerText = readingN.timestamp;
            }
        }, function (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        });
    }

})();
