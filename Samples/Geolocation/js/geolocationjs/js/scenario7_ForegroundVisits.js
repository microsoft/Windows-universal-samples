//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    // Namespace abbreviations.
    var Geolocation = Windows.Devices.Geolocation;

    var visitmonitor;

    var startMonitoringButton;
    var stopMonitoringButton;
    var latitudeElement;
    var longitudeElement;
    var accuracyElement;
    var timestampElement;
    var statechangeElement;

    var page = WinJS.UI.Pages.define("/html/scenario7_ForegroundVisits.html", {
        ready: function (element, options) {
            startMonitoringButton = document.getElementById("startMonitoringButton");
            stopMonitoringButton = document.getElementById("stopMonitoringButton");
            latitudeElement = document.getElementById("latitude");
            longitudeElement = document.getElementById("longitude");
            accuracyElement = document.getElementById("accuracy");
            timestampElement = document.getElementById("timestamp");
            statechangeElement = document.getElementById("statechange");

            startMonitoringButton.addEventListener("click", startMonitoring, false);
            stopMonitoringButton.addEventListener("click", stopMonitoring, false);
            startMonitoringButton.disabled = false;
            stopMonitoringButton.disabled = true;
        },
        unload: function () {
            if (visitmonitor) {
                visitmonitor.stop();
            }
        }
    });

    function startMonitoring() {
        Geolocation.Geolocator.requestAccessAsync().done(function (accessStatus) {
            switch (accessStatus) {
                case Geolocation.GeolocationAccessStatus.allowed:

                    visitmonitor = new Geolocation.GeovisitMonitor();

                    // Subscribe to state changed event to get visit state changes.
                    visitmonitor.addEventListener("visitstatechanged", onVisitStateChanged);

                    // Start monitoring with the desired scope.
                    // For higher granularity such as building/venue level changes, choose venue.
                    // For lower granularity more or less in the range of zipcode level changes, choose city. 
                    // Choosing Venue here as an example.
                    visitmonitor.start(Geolocation.VisitMonitoringScope.venue);

                    WinJS.log && WinJS.log("Waiting for update...", "sample", "status");
                    startMonitoringButton.disabled = true;
                    stopMonitoringButton.disabled = false;
                    break;

                case Geolocation.GeolocationAccessStatus.denied:
                    WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");
                    break;

                    default:
                case Geolocation.GeolocationAccessStatus.unspecified:
                    WinJS.log && WinJS.log("Unspecified error!", "sample", "error");
                    break;
            }
        });
    }

    function stopMonitoring() {
        visitmonitor.stop();
        visitmonitor = null;

        startMonitoringButton.disabled = false;
        stopMonitoringButton.disabled = true;

        // Clear status
        WinJS.log && WinJS.log("", "sample", "status");
    }

    function onVisitStateChanged(args) {
        WinJS.log && WinJS.log("Visit state changed", "sample", "status");

        updateVisitData(args.visit);
    }

    function updateVisitData(visit) {
        statechangeElement.innerHTML = SdkSample.toStateChangeString(visit.stateChange);
        var dateTimeFormatter = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("shortdate longtime");
        timestampElement.innerHTML = dateTimeFormatter.format(visit.timestamp);

        // Check if the visit has a valid position.
        if (!visit.position) {
            latitudeElement.innerHTML = "No data";
            longitudeElement.innerHTML = "No data";
            accuracyElement.innerHTML = "No data";
        } else {
            var coord = visit.position.coordinate;
            latitudeElement.innerHTML = coord.point.position.latitude;
            longitudeElement.innerHTML = coord.point.position.longitude;
            accuracyElement.innerHTML = coord.accuracy;
        }
    }
})();