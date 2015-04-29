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
