//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // DOM elements
    var calibrationBarControl;

    var page = WinJS.UI.Pages.define("/html/scenario3_Calibration.html", {
        ready: function (element, options) {
            calibrationBarControl = document.getElementById("calibrationBarControl");

            document.getElementById("accuracyHighRadio").addEventListener("click", onAccuracyHigh, false);
            document.getElementById("accuracyApproximateRadio").addEventListener("click", onAccuracyApproximate, false);
            document.getElementById("accuracyUnreliableRadio").addEventListener("click", onAccuracyUnreliable, false);

            if (SdkSample.sensorReadingType == "relative") {
                document.getElementById("enabledContent").style.display = "none";
                document.getElementById("disabledContent").style.display = "block";
            }
        }
    });

    function onAccuracyHigh() {
        calibrationBarControl.winControl.hide();
    }

    function onAccuracyApproximate() {
        calibrationBarControl.winControl.hide();
    }

    function onAccuracyUnreliable() {
        calibrationBarControl.winControl.requestCalibration(Windows.Devices.Sensors.MagnetometerAccuracy.unreliable);
    }
})();
