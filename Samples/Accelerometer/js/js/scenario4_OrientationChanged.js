//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var accelerometerOriginal;
    var accelerometerReadingTransform;
    var displayInformation;

    // DOM elements
    var scenarioEnable;
    var scenarioDisable;
    var eventOutputOriginal;
    var eventOutputDataTransform;

    var page = WinJS.UI.Pages.define("/html/scenario4_OrientationChanged.html", {
        ready: function (element, options) {
            scenarioEnable = document.getElementById("scenarioEnable");
            scenarioDisable = document.getElementById("scenarioDisable");
            eventOutputOriginal = document.getElementById("eventOutputOriginal");
            eventOutputDataTransform = document.getElementById("eventOutputDataTransform");

            scenarioEnable.addEventListener("click", enableOrientationChangedScenario, false);
            scenarioDisable.addEventListener("click", disableOrientationChangedScenario, false);

            // Get two instances of the accelerometer:
            // One that returns the raw accelerometer data
            var readingType = SdkSample.accelerometerReadingType;
            accelerometerOriginal = Windows.Devices.Sensors.Accelerometer.getDefault(Windows.Devices.Sensors.AccelerometerReadingType[readingType]);
            // Other on which the 'ReadingTransform' is updated so that data returned aligns with the request transformation.
            accelerometerReadingTransform = Windows.Devices.Sensors.Accelerometer.getDefault(Windows.Devices.Sensors.AccelerometerReadingType[readingType]);

            if (!accelerometerOriginal || !accelerometerReadingTransform) {
                WinJS.log && WinJS.log(readingType + " accelerometer not found", "sample", "error");
            } else {
                WinJS.log && WinJS.log(readingType + " accelerometer ready", "sample", "status");
                scenarioEnable.disabled = false;
            }

            displayInformation = Windows.Graphics.Display.DisplayInformation.getForCurrentView();
            displayInformation.addEventListener("orientationchanged", onOrientationChanged);
        },
        unload: function () {
            if (!scenarioDisable.disabled) {
                disableOrientationChangedScenario();
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (!scenarioDisable.disabled) {
            if (document.msVisibilityState === "visible") {
                // Re-enable sensor input. No need to restore the desired reportInterval (it is restored for us upon app resume)
                accelerometerOriginal.addEventListener("readingchanged", onDataChangedOriginal);
                accelerometerReadingTransform.addEventListener("readingchanged", onDataChangedReadingTransform);
            } else {
                // Disable sensor input. No need to restore the default reportInterval (resources will be released upon app suspension)
                accelerometerOriginal.removeEventListener("readingchanged", onDataChangedOriginal);
                accelerometerReadingTransform.removeEventListener("readingchanged", onDataChangedReadingTransform);
            }
        }
    }

    function onDataChangedOriginal(e) {
        SdkSample.setReadingText(eventOutputOriginal, e.reading);
    }

    function onDataChangedReadingTransform(e) {
        SdkSample.setReadingText(eventOutputDataTransform, e.reading);
    }

    function enableOrientationChangedScenario() {
        // Select a report interval that is both suitable for the purposes of the app and supported by the sensor.
        accelerometerOriginal.reportInterval = Math.max(accelerometerOriginal.minimumReportInterval, 16);
        accelerometerReadingTransform.reportInterval = Math.max(accelerometerReadingTransform.minimumReportInterval, 16);

        // Set the readingTransform to align with the current display orientation
        accelerometerReadingTransform.readingTransform = displayInformation.currentOrientation;

        accelerometerOriginal.addEventListener("readingchanged", onDataChangedOriginal);
        accelerometerReadingTransform.addEventListener("readingchanged", onDataChangedReadingTransform);

        document.addEventListener("visibilitychange", visibilityChangeHandler, false);

        scenarioEnable.disabled = true;
        scenarioDisable.disabled = false;
    }

    function disableOrientationChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometerOriginal.removeEventListener("readingchanged", onDataChangedOriginal);
        accelerometerReadingTransform.removeEventListener("readingchanged", onDataChangedReadingTransform);

        // Return the report interval to its default to release resources while the sensor is not in use
        accelerometerOriginal.reportInterval = 0;
        accelerometerReadingTransform.reportInterval = 0;

        scenarioEnable.disabled = false;
        scenarioDisable.disabled = true;
    }

    function onOrientationChanged(eventArgs) {
        if (accelerometerReadingTransform) {
            accelerometerReadingTransform.readingTransform = eventArgs.target.currentOrientation;
        }
    }
})();
