//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

// A JavaScript background task runs a specified JavaScript file.
(function () {
    "use strict";

    // Get the background task instance's activation parameters
    var backgroundTaskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;
    var promise;

    // Associate a cancellation handler with the background task.
    function onCanceled(cancelSender, cancelReason) {
        if (promise) {
            promise.operation.cancel();
        }
    }
    backgroundTaskInstance.addEventListener("canceled", onCanceled);

    var geolocator = new Windows.Devices.Geolocation.Geolocator();

    promise = geolocator.getGeopositionAsync();
    promise.done(
        function (pos) {
            var coord = pos.coordinate;

            var settings = Windows.Storage.ApplicationData.current.localSettings;
            settings.values["Status"] = coord.timestamp;

            settings.values["latitude"] = coord.point.position.latitude;
            settings.values["longitude"] = coord.point.position.longitude;
            settings.values["accuracy"] = coord.accuracy;

            backgroundTaskInstance.succeeded = true;

            // A JavaScript background task must call close when it is done
            close();
        },
        function (err) {
            var settings = Windows.Storage.ApplicationData.current.localSettings;
            settings.values["Status"] = err.message;

            settings.values["latitude"] = "No data";
            settings.values["longitude"] = "No data";
            settings.values["accuracy"] = "No data";

            backgroundTaskInstance.succeeded = false;

            // A JavaScript background task must call close when it is done
            close();
        }
    );
})();
