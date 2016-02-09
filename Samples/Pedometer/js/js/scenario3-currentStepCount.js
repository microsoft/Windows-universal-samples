//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var getStepCount;
    var readingTimeStamp;
    var totalStepsCount;

    var page = WinJS.UI.Pages.define("/html/scenario3-currentStepCount.html", {
        ready: function (element, options) {
            getStepCount = document.getElementById("getStepCount");
            readingTimeStamp = document.getElementById("readingTimeStamp");
            totalStepsCount = document.getElementById("totalStepsCount");

            getStepCount.addEventListener("click", getCurrentStepCount, false);
        }
    });

    /// <summary>
    /// Invoked when 'getStepCount' button is clicked.
    /// 'ReadingChanged' will not be fired when there is no activity on the pedometer 
    /// and hence can't be reliably used to get the current step count. This handler makes
    /// use of 'getCurrentReadings' API to read the step count
    /// </summary>
    function getCurrentStepCount() {        
        getStepCount.disabled = true;
        Windows.Devices.Sensors.Pedometer.getDefaultAsync().then(function (pedometer) {
            if (pedometer) {
                var readings = pedometer.getCurrentReadings();
                var timestamp;
                var totalSteps = 0;
                for (var iter = readings.first(); iter.hasCurrent; iter.moveNext()) {
                    var reading = iter.current.value;
                    totalSteps += reading.cumulativeSteps;
                    timestamp = reading.timestamp;
                }
                readingTimeStamp.innerText = timestamp ? SdkSample.formatTimestamp(timestamp) : "no data";
                totalStepsCount.innerText = totalSteps;
                WinJS.log && WinJS.log("Retrieved the step count from the default pedometer", "sample", "status");
            } else {
                WinJS.log && WinJS.log("No Pedometer found", "sample", "error");
            }
        }, function error(e) {
            WinJS.log && WinJS.log("Error when opening default pedometer. " + e.message, "sample", "error");
        }).done(function () {
            // enable the button for subsequent retrievals
            getStepCount.disabled = false;
        });
    }

})();
