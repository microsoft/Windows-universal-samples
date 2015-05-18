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
    var accelerometerOriginal;
    var accelerometerReadingTransform;
    var displayInformation;

    var page = WinJS.UI.Pages.define("/html/scenario4_OrientationChanged.html", {
        ready: function (element, options) {
            var disableOpen = true;
            document.getElementById("scenario4Open").addEventListener("click", enableOrientationChangedScenario, false);
            document.getElementById("scenario4Revoke").addEventListener("click", disableOrientationChangedScenario, false);
            document.getElementById("scenario4Revoke").disabled = true;

            displayInformation = Windows.Graphics.Display.DisplayInformation.getForCurrentView();

            // Get two instances of the accelerometer:
            // One that returns the raw accelerometer data
            accelerometerOriginal = Windows.Devices.Sensors.Accelerometer.getDefault();
            // Other on which the 'ReadingTransform' is updated so that data returned aligns with the request transformation.
            accelerometerReadingTransform = Windows.Devices.Sensors.Accelerometer.getDefault();

            if (!accelerometerOriginal || !accelerometerReadingTransform) {
                WinJS.log && WinJS.log("No accelerometer found", "sample", "error");
            } else {
                disableOpen = false;
                displayInformation.addEventListener("orientationchanged", onOrientationChanged);
            }
            document.getElementById("scenario4Open").disabled = disableOpen;
        },
        unload: function () {
            if (!document.getElementById("scenario4Revoke").disabled) {
                document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
                displayInformation.removeEventListener("orientationchanged", onOrientationChanged);
                accelerometerOriginal.removeEventListener("readingchanged", onDataChangedOriginal);
                accelerometerReadingTransform.removeEventListener("readingchanged", onDataChangedReadingTransform);

                // Return the report interval to its default to release resources while the sensor is not in use
                accelerometerOriginal.reportInterval = 0;
                accelerometerReadingTransform.reportInterval = 0;
            }
        }
    });

    function visibilityChangeHandler() {
        // This is the event handler for VisibilityChanged events. You would register for these notifications
        // if handling sensor data when the app is not visible could cause unintended actions in the app.
        if (!document.getElementById("scenario4Revoke").disabled) {
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
        var reading = e.reading;

        // event can still be in queue after unload is called
        // so check if elements are still loaded

        if (document.getElementById("eventOutputXOriginal")) {
            document.getElementById("eventOutputXOriginal").innerHTML = reading.accelerationX.toFixed(2);
        }
        if (document.getElementById("eventOutputYOriginal")) {
            document.getElementById("eventOutputYOriginal").innerHTML = reading.accelerationY.toFixed(2);
        }
        if (document.getElementById("eventOutputZOriginal")) {
            document.getElementById("eventOutputZOriginal").innerHTML = reading.accelerationZ.toFixed(2);
        }
    }

    function onDataChangedReadingTransform(e) {
        var reading = e.reading;

        // event can still be in queue after unload is called
        // so check if elements are still loaded

        if (document.getElementById("eventOutputXDataTransform")) {
            document.getElementById("eventOutputXDataTransform").innerHTML = reading.accelerationX.toFixed(2);
        }
        if (document.getElementById("eventOutputYDataTransform")) {
            document.getElementById("eventOutputYDataTransform").innerHTML = reading.accelerationY.toFixed(2);
        }
        if (document.getElementById("eventOutputZDataTransform")) {
            document.getElementById("eventOutputZDataTransform").innerHTML = reading.accelerationZ.toFixed(2);
        }
    }

    function enableOrientationChangedScenario() {
        // Set the reportInterval to enable the sensor events
        accelerometerOriginal.reportInterval = accelerometerOriginal.minimumReportInterval;
        accelerometerReadingTransform.reportInterval = accelerometerReadingTransform.minimumReportInterval;

        // Set the readingTransform to align with the current display orientation
        accelerometerReadingTransform.readingTransform = displayInformation.currentOrientation;

        accelerometerOriginal.addEventListener("readingchanged", onDataChangedOriginal);
        accelerometerReadingTransform.addEventListener("readingchanged", onDataChangedReadingTransform);

        document.addEventListener("visibilitychange", visibilityChangeHandler, false);

        document.getElementById("scenario4Open").disabled = true;
        document.getElementById("scenario4Revoke").disabled = false;
    }

    function disableOrientationChangedScenario() {
        document.removeEventListener("visibilitychange", visibilityChangeHandler, false);
        accelerometerOriginal.removeEventListener("readingchanged", onDataChangedOriginal);
        accelerometerReadingTransform.removeEventListener("readingchanged", onDataChangedReadingTransform);

        // Return the report interval to its default to release resources while the sensor is not in use
        accelerometerOriginal.reportInterval = 0;
        accelerometerReadingTransform.reportInterval = 0;

        document.getElementById("scenario4Open").disabled = false;
        document.getElementById("scenario4Revoke").disabled = true;
    }

    function onOrientationChanged(eventArgs) {
        if (accelerometerReadingTransform) {
            accelerometerReadingTransform.readingTransform = eventArgs.target.currentOrientation;
        }
    }
})();
