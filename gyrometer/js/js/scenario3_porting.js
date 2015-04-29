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
    var gyrometerWP;
    var gyrometerWindows;

    var page = WinJS.UI.Pages.define("/html/scenario3_Porting.html", {
        ready: function (element, options) {
            document.getElementById("scenario3GetSample").addEventListener("click", getGyrometerSample, false);

            // Get two instances of the gyrometer:
            // One that returns the raw gyrometer data
            gyrometerWP = Windows.Devices.Sensors.Gyrometer.getDefault();
            // Other on which the 'ReadingTransform' is updated so that data returned aligns with the native WP orientation (portrait)
            gyrometerWindows = Windows.Devices.Sensors.Gyrometer.getDefault();

            if (gyrometerWP && gyrometerWindows) {
                // Enable 'Get Sample' button
                document.getElementById("scenario3GetSample").disabled = false;

                // Assumption is that this app has been developed for Windows Phone 8.1 (or earlier)
                // and hence assumes that the sensor returns readings in Portrait Mode, which may 
                // not be true when the app or sensor logic is being ported over to a 
                // Landscape-First Windows device
                // While we encourage you to re-design your app as a universal app to gain access 
                // to many other advantages of developing a universal app, this scenario demonstrates
                // a simple approach to let the runtime honor your assumption on the  
                // "native orientation" of the sensor.
                gyrometerWP.readingTransform = Windows.Graphics.Display.DisplayOrientations.portrait;
                // If you were to go the route of universal app, make no assumptions about the 
                // native orientation of the device. Instead rely on using a display orientation 
                // (absolute or current) to enforce the reference frame for the sensor readings. 
                // (which is done by updating 'ReadingTransform' property with the appropriate orientation)
            } else {
                // Disable 'Get Sample' button
                document.getElementById("scenario3GetSample").disabled = true;
                WinJS.log && WinJS.log("No gyrometer found", "sample", "error");
            }
        },
        unload: function () {
            // Return the report interval to its default to release resources while the sensor is not in use
            gyrometerWP.reportInterval = 0;
            gyrometerWindows.reportInterval = 0;
        }
    });

    function getGyrometerSample() {

        document.getElementById("scenario3GetSample").disabled = true;

        gyrometerWP.reportInterval = gyrometerWP.minimumReportInterval;
        gyrometerWindows.reportInterval = gyrometerWindows.minimumReportInterval;

        var readingWP = gyrometerWP.getCurrentReading();
        var readingWindows = gyrometerWindows.getCurrentReading();

        if (readingWP) {
            document.getElementById("eventOutputXWP").innerHTML = readingWP.angularVelocityX.toFixed(2);
            document.getElementById("eventOutputYWP").innerHTML = readingWP.angularVelocityY.toFixed(2);
            document.getElementById("eventOutputZWP").innerHTML = readingWP.angularVelocityZ.toFixed(2);
        }
        if (readingWindows) {
            document.getElementById("eventOutputXWindows").innerHTML = readingWindows.angularVelocityX.toFixed(2);
            document.getElementById("eventOutputYWindows").innerHTML = readingWindows.angularVelocityY.toFixed(2);
            document.getElementById("eventOutputZWindows").innerHTML = readingWindows.angularVelocityZ.toFixed(2);
        }

        gyrometerWP.reportInterval = 0;
        gyrometerWindows.reportInterval = 0;

        document.getElementById("scenario3GetSample").disabled = false;
    }
})();
