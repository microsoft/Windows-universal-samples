//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var inclinometer;

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("getData").addEventListener("click", getData, false);
            document.getElementById("getData").disabled = false;

            inclinometer = Windows.Devices.Sensors.Inclinometer.getDefaultForRelativeReadings();
            if (!inclinometer) {
                document.getElementById("getData").disabled = true;
                WinJS.log && WinJS.log("No relative inclinometer found", "sample", "error");
            }
        }
    });

    function getData() {
        var reading = inclinometer.getCurrentReading();
        if (reading) {
            document.getElementById("readingOutputX").innerHTML = reading.pitchDegrees.toFixed(2);
            document.getElementById("readingOutputY").innerHTML = reading.rollDegrees.toFixed(2);
            document.getElementById("readingOutputZ").innerHTML = reading.yawDegrees.toFixed(2);
        }
    }
})();
