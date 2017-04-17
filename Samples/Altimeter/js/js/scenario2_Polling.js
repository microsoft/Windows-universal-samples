//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var altimeter;

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Open").addEventListener("click", enableGetReadingScenario, false);
            document.getElementById("scenario2Open").disabled = false;
        }
    });

    function enableGetReadingScenario() {
        SdkSample.getDefaultAltimeterAsync().then(function (result) {
            altimeter = result;
            if (altimeter) {
                var reading = altimeter.getCurrentReading();
                if (reading) {
                    document.getElementById("readingOutputChangeInMeters").innerText = reading.altitudeChangeInMeters.toFixed(2);
                }
            } else {
                WinJS.log && WinJS.log("No altimeter found", "sample", "error");
            }
        });
    }
})();
