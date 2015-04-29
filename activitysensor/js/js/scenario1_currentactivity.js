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

    var page = WinJS.UI.Pages.define("/html/scenario1_currentActivity.html", {
        ready: function (element, options) {
            document.getElementById("scenario1GetCurrentActivity").addEventListener("click", getCurrentActivityScenario, false);
        }
    });

    /// <summary>
    /// This is the click handler for the 'Get Current Activity' button.
    /// </summary>
    function getCurrentActivityScenario() {

        // Reset fields and status
        document.getElementById("activityOutput").innerText = "no data";
        document.getElementById("confidenceOutput").innerText = "no data";
        document.getElementById("timestampOutput").innerText = "no data";
        WinJS.log && WinJS.log("", "sample", "status");

        Windows.Devices.Sensors.ActivitySensor.getDefaultAsync().done(function (activitySensor) {
            if (activitySensor) {
                activitySensor.getCurrentReadingAsync().done(function (reading) {
                    document.getElementById("activityOutput").innerText = ActivitySensorSdkSample.activityTypeToString(reading.activity);
                    document.getElementById("confidenceOutput").innerText = ActivitySensorSdkSample.readingConfidenceToString(reading.confidence);
                    document.getElementById("timestampOutput").innerText = reading.timestamp;
                },
                function (e) {
                    WinJS.log && WinJS.log(e.message, "sample", "error");
                });
            } else {
                WinJS.log && WinJS.log("No activity sensor found", "sample", "error");
            }
        },
        function (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        });
    }

})();
