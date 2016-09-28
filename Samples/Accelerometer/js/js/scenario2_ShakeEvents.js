//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var accelerometer;
    var shakeCount = 0;

    // DOM elements
    var scenarioEnable;
    var scenarioDisable;
    var shakeOutput;

    var page = WinJS.UI.Pages.define("/html/scenario2_ShakeEvents.html", {
        ready: function (element, options) {
            scenarioEnable = document.getElementById("scenarioEnable");
            scenarioDisable = document.getElementById("scenarioDisable");
            shakeOutput = document.getElementById("shakeOutput");

            scenarioEnable.addEventListener("click", enableShakenScenario, false);
            scenarioDisable.addEventListener("click", disableShakenScenario, false);
 
            var readingType = SdkSample.accelerometerReadingType;
            accelerometer = Windows.Devices.Sensors.Accelerometer.getDefault(Windows.Devices.Sensors.AccelerometerReadingType[readingType]);
            if (accelerometer) {
                WinJS.log && WinJS.log(readingType + " accelerometer ready", "sample", "status");
                scenarioEnable.disabled = false;
            } else {
                WinJS.log && WinJS.log(readingType + " accelerometer not found", "sample", "error");
            }
        },
        unload: function () {
            if (!scenarioDisable.disabled) {
                disableShakenScenario();
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (!scenarioDisable.disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input
                accelerometer.addEventListener("shaken", onShaken);
            } else {
                // Disable sensor input
                accelerometer.removeEventListener("shaken", onShaken);
            }
        }
    }

    function onShaken() {
        shakeCount++;
        shakeOutput.innerHTML = shakeCount;
    }

    function enableShakenScenario() {
        document.addEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometer.addEventListener("shaken", onShaken);
        scenarioEnable.disabled = true;
        scenarioDisable.disabled = false;
    }

    function disableShakenScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometer.removeEventListener("shaken", onShaken);
        scenarioEnable.disabled = false;
        scenarioDisable.disabled = true;
    }
})();
