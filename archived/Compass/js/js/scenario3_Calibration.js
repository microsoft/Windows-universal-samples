//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario3_Calibration.html", {
        ready: function (element, options) {
            document.getElementById("accuracyHighRadio").addEventListener("click", onAccuracyHigh, false);
            document.getElementById("accuracyApproximateRadio").addEventListener("click", onAccuracyApproximate, false);
            document.getElementById("accuracyUnreliableRadio").addEventListener("click", onAccuracyUnreliable, false);
            document.getElementById("accuracyHighRadio").checked = "true";
        },
        unload: function () {
        }
    });

    function onAccuracyHigh() {
        document.getElementById("calibrationBarControl").winControl.hide();
    }

    function onAccuracyApproximate() {
        document.getElementById("calibrationBarControl").winControl.hide();
    }

    function onAccuracyUnreliable() {
        document.getElementById("calibrationBarControl").winControl.requestCalibration(Windows.Devices.Sensors.MagnetometerAccuracy.unreliable);
    }

})();
