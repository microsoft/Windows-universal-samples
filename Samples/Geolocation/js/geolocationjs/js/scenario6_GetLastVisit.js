//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

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

    var getLastVisitButton;
    var latitudeElement;
    var longitudeElement;
    var accuracyElement;
    var timestampElement;
    var statechangeElement;

    var page = WinJS.UI.Pages.define("/html/scenario6_GetLastVisit.html", {
        ready: function (element, options) {
            getLastVisitButton = document.getElementById("getLastVisitButton");
            latitudeElement = document.getElementById("latitude");
            longitudeElement = document.getElementById("longitude");
            accuracyElement = document.getElementById("accuracy");
            timestampElement = document.getElementById("timestamp");
            statechangeElement = document.getElementById("statechange");

            getLastVisitButton.addEventListener("click", getLastVisit, false);
            getLastVisitButton.disabled = false;
        }
    });

    function getLastVisit() {
        getLastVisitButton.disabled = true;

        Geolocation.Geolocator.requestAccessAsync().then(function (accessStatus) {
            switch (accessStatus) {
                case Geolocation.GeolocationAccessStatus.allowed:

                    WinJS.log && WinJS.log("Waiting for update...", "sample", "status");
                    getLastVisitButton.disabled = true;

                    // Get the last visit report, if any. This should be a quick operation
                    return Geolocation.GeovisitMonitor.getLastReportAsync().then(function (visit) {
                        WinJS.log && WinJS.log("Visit info updated", "sample", "status");
                        return visit;
                    });

                case Geolocation.GeolocationAccessStatus.denied:
                    WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");
                    break;

                case Geolocation.GeolocationAccessStatus.unspecified:
                    WinJS.log && WinJS.log("Unspecified error!", "sample", "error");
                    break;
            }
        }).done(function (visit) {
            updateLastVisit(visit);
            getLastVisitButton.disabled = false;
        });
    }

    function updateLastVisit(visit) {
        if (!visit) {
            latitudeElement.innerHTML = "No data";
            longitudeElement.innerHTML = "No data";
            accuracyElement.innerHTML = "No data";
            timestampElement.innerHTML = "No data";
            statechangeElement.innerHTML = "No data";
        } else {
            statechangeElement.innerHTML = SdkSample.toStateChangeString(visit.stateChange);

            var dateTimeFormatter = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("shortdate longtime");
            timestampElement.innerHTML = dateTimeFormatter.format(visit.timestamp);

            // Check if there is a valid position in the report.
            if (visit.position === null) {
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
    }

})();