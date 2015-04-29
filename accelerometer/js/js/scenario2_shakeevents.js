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
    var accelerometer;

    var page = WinJS.UI.Pages.define("/html/scenario2_ShakeEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Open").addEventListener("click", enableShakenScenario, false);
            document.getElementById("scenario2Revoke").addEventListener("click", disableShakenScenario, false);
            document.getElementById("scenario2Open").disabled = false;
            document.getElementById("scenario2Revoke").disabled = true;

            accelerometer = Windows.Devices.Sensors.Accelerometer.getDefault();
            if (accelerometer === null) {
                WinJS.log && WinJS.log("No accelerometer found", "sample", "error");
            }
        },
        unload: function () {
            if (document.getElementById("scenario2Open").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                accelerometer.removeEventListener("shaken", onShaken);
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (document.getElementById("scenario2Open").disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input
                accelerometer.addEventListener("shaken", onShaken);
            } else {
                // Disable sensor input
                accelerometer.removeEventListener("shaken", onShaken);
            }
        }
    }

    var onShaken = (function () {
        var shakeCount = 0;

        return function (e) {
            shakeCount++;
            document.getElementById("shakeOutput").innerHTML = shakeCount;
        };
    })();

    function enableShakenScenario() {
        if (accelerometer) {
            document.addEventListener("visibilitychange", visibilityChangeHandler, false);
            accelerometer.addEventListener("shaken", onShaken);
            document.getElementById("scenario2Open").disabled = true;
            document.getElementById("scenario2Revoke").disabled = false;
        } else {
            WinJS.log && WinJS.log("No accelerometer found", "sample", "error");
        }
    }

    function disableShakenScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometer.removeEventListener("shaken", onShaken);
        document.getElementById("scenario2Open").disabled = false;
        document.getElementById("scenario2Revoke").disabled = true;
    }
})();
