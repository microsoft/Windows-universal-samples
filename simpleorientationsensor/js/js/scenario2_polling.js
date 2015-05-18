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
    var sensor;

    var page = WinJS.UI.Pages.define("/html/scenario2_Polling.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Get").addEventListener("click", invokeGetReadingScenario, false);

            sensor = Windows.Devices.Sensors.SimpleOrientationSensor.getDefault();
            if (sensor === null) {
                WinJS.log && WinJS.log("No simple orientation sensor found", "sample", "error");
            }
        }
    });

    function invokeGetReadingScenario() {
        if (sensor) {
            var orientation = sensor.getCurrentOrientation();

            switch (orientation) {
                case Windows.Devices.Sensors.SimpleOrientation.notRotated:
                    document.getElementById("readingOutputOrientation").innerHTML = "Not Rotated";
                    break;
                case Windows.Devices.Sensors.SimpleOrientation.rotated90DegreesCounterclockwise:
                    document.getElementById("readingOutputOrientation").innerHTML = "Rotated 90";
                    break;
                case Windows.Devices.Sensors.SimpleOrientation.rotated180DegreesCounterclockwise:
                    document.getElementById("readingOutputOrientation").innerHTML = "Rotated 180";
                    break;
                case Windows.Devices.Sensors.SimpleOrientation.rotated270DegreesCounterclockwise:
                    document.getElementById("readingOutputOrientation").innerHTML = "Rotated 270";
                    break;
                case Windows.Devices.Sensors.SimpleOrientation.faceup:
                    document.getElementById("readingOutputOrientation").innerHTML = "Face Up";
                    break;
                case Windows.Devices.Sensors.SimpleOrientation.facedown:
                    document.getElementById("readingOutputOrientation").innerHTML = "Face Down";
                    break;
                default:
                    document.getElementById("readingOutputOrientation").innerHTML = "Undefined orientation " + orientation;
                    break;
            }
        } else {
            WinJS.log && WinJS.log("No simple orientation sensor found", "sample", "error");
        }
    }
})();
