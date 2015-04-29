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
            document.getElementById("getData").addEventListener("click", getData, false);
            document.getElementById("getData").disabled = false;

            sensor = Windows.Devices.Sensors.OrientationSensor.getDefaultForRelativeReadings();
            if (!sensor) {
                WinJS.log && WinJS.log("No relative orientation sensor found", "sample", "error");
            }
        }
    });

    function getData() {
        var reading = sensor.getCurrentReading();
        if (reading) {
            // Quaternion values
            var quaternion = reading.quaternion;    // get a reference to the object to avoid re-creating it for each access
            document.getElementById("readingOutputQuaternion").innerHTML =
                   "W: " + quaternion.w.toFixed(6)
                + " X: " + quaternion.x.toFixed(6)
                + " Y: " + quaternion.y.toFixed(6)
                + " Z: " + quaternion.z.toFixed(6);

            // Rotation Matrix values
            var rotationMatrix = reading.rotationMatrix;
            document.getElementById("readingOutputRotationMatrix").innerHTML =
                   "M11: " + rotationMatrix.m11.toFixed(6)
                + " M12: " + rotationMatrix.m12.toFixed(6)
                + " M13: " + rotationMatrix.m13.toFixed(6)
                + " M21: " + rotationMatrix.m21.toFixed(6)
                + " M22: " + rotationMatrix.m22.toFixed(6)
                + " M23: " + rotationMatrix.m23.toFixed(6)
                + " M31: " + rotationMatrix.m31.toFixed(6)
                + " M32: " + rotationMatrix.m32.toFixed(6)
                + " M33: " + rotationMatrix.m33.toFixed(6);
        }
    }
})();
