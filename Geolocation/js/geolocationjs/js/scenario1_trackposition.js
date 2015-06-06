//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    //"use strict";

    var geolocator = null;
    var disposed;
    var page = WinJS.UI.Pages.define("/html/scenario1_TrackPosition.html", {
        ready: function (element, options) {
            disposed = false;
            document.getElementById("startTrackingButton").addEventListener("click", starttracking, false);
            document.getElementById("stopTrackingButton").addEventListener("click", stoptracking, false);
            document.getElementById("startTrackingButton").disabled = false;
            document.getElementById("stopTrackingButton").disabled = true;
        },
        dispose: function () {
            if (!disposed) {
                disposed = true;
                if (document.getElementById("startTrackingButton").disabled) {
                    geolocator.removeEventListener("positionchanged", onPositionChanged);
                    geolocator.removeEventListener("statuschanged", onStatusChanged);
                }
            }
        }
    });

    function starttracking() {
        Windows.Devices.Geolocation.Geolocator.requestAccessAsync().done(
            function(accessStatus){
                switch(accessStatus)
                {
                    case Windows.Devices.Geolocation.GeolocationAccessStatus.allowed:
                        // You should set MovementThreshold for distance-based tracking
                        // or ReportInterval for periodic-based tracking before adding event
                        // handlers. If none is set, a ReportInterval of 1 second is used
                        // as a default and a position will be returned every 1 second.
                        //
                        // Value of 2000 milliseconds (2 seconds) 
                        // isn't a requirement, it is just an example.
                        geolocator = new Windows.Devices.Geolocation.Geolocator();
                        geolocator.ReportInterval = 2000;

                        // Subscribe to PositionChanged event to get updated tracking positions
                        geolocator.addEventListener("positionchanged", onPositionChanged);

                        // Subscribe to StatusChanged event to get updates of location status changes
                        geolocator.addEventListener("statuschanged", onStatusChanged);

                        WinJS.log && WinJS.log("Waiting for update...", "sample", "status");
                        document.getElementById("startTrackingButton").disabled = true;
                        document.getElementById("stopTrackingButton").disabled = false;
                        break;
                        
                    case Windows.Devices.Geolocation.GeolocationAccessStatus.denied:
                        WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");
                        break;

                    case Windows.Devices.Geolocation.GeolocationAccessStatus.unspecified:
                        WinJS.log && WinJS.log("Unspecified error!", "sample", "error");
                        break;
                }
            },
            function(err){
                WinJS.log && WinJS.log(err, "sample", "error");
            }
        );
    }

    function stoptracking() {
        geolocator.removeEventListener("positionchanged", onPositionChanged);
        geolocator.removeEventListener("statuschanged", onStatusChanged);
        geolocator = null;

        document.getElementById("startTrackingButton").disabled = false;
        document.getElementById("stopTrackingButton").disabled = true;

        // Clear status
        WinJS.log && WinJS.log("", "sample", "status");
    }

    function onPositionChanged(e) {
        var coord = e.position.coordinate;

        WinJS.log && WinJS.log("Location updated", "sample", "status");

        document.getElementById("latitude").innerText = coord.point.position.latitude;
        document.getElementById("longitude").innerText = coord.point.position.longitude;
        document.getElementById("accuracy").innerText = coord.accuracy;
    }

    function onStatusChanged(e) {
        switch (e.status) {
            case Windows.Devices.Geolocation.PositionStatus.ready:
                // Location platform is providing valid data.
                document.getElementById("status").innerText = "Ready";
                WinJS.log && WinJS.log("Location platform is ready.", "sample", "status");
                break;
            case Windows.Devices.Geolocation.PositionStatus.initializing:
                // Location platform is attempting to acquire a fix.
                document.getElementById("status").innerText = "Initializing";
                WinJS.log && WinJS.log("Location platform is attempting to obtain a position.", "sample", "status");
                break;
            case Windows.Devices.Geolocation.PositionStatus.noData:
                // Location platform could not obtain location data.
                document.getElementById("status").innerText = "No data";
                WinJS.log && WinJS.log("Not able to determine the location.", "sample", "error");
                break;
            case Windows.Devices.Geolocation.PositionStatus.disabled:
                // The permission to access location data is denied by the user or other policies.
                document.getElementById("status").innerText = "Disabled";
                WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");

                // Clear cached location data if any
                document.getElementById("latitude").innerText = "No data";
                document.getElementById("longitude").innerText = "No data";
                document.getElementById("accuracy").innerText = "No data";
                break;
            case Windows.Devices.Geolocation.PositionStatus.notInitialized:
                // The location platform is not initialized. This indicates that the application has
                // not made a request for location data.
                document.getElementById("status").innerText = "Not initialized";
                WinJS.log && WinJS.log("No request for location is made yet.", "sample", "status");
                break;
            case Windows.Devices.Geolocation.PositionStatus.notAvailable:
                // The location platform is not available on this version of the OS.
                document.getElementById("status").innerText = "Not available";
                WinJS.log && WinJS.log("Location is not available on this version of the OS.", "sample", "error");
                break;
            default:
                document.getElementById("status").innerText = "Unknown";
                WinJS.log && WinJS.log("", "sample", "status");
                break;
        }
    }
})();