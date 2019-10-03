//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    var getGeopositionPromise;
    var desiredAccuracyInMeters = 0;

    var disposed;
    var page = WinJS.UI.Pages.define("/html/scenario2_GetPosition.html", {
        ready: function (element, options) {
            disposed = false;
            document.getElementById("getGeopositionButton").addEventListener("click", getGeoposition, false);
            document.getElementById("cancelGetGeopositionButton").addEventListener("click", cancelGetGeoposition, false);
            document.getElementById("getGeopositionButton").disabled = false;
            document.getElementById("cancelGetGeopositionButton").disabled = true;
            document.getElementById("desiredAccuracyInMeters").addEventListener("keyup", getDesiredAccuracy);
        },
        dispose: function () {
            if (!disposed) {
                disposed = true;
                if (document.getElementById("getGeopositionButton").disabled) {
                    if (getGeopositionPromise) {
                        getGeopositionPromise.operation.cancel();
                    }
                }
            }
        }
    });

    function getGeoposition() {
        document.getElementById("getGeopositionButton").disabled = true;
        document.getElementById("cancelGetGeopositionButton").disabled = false;

        Windows.Devices.Geolocation.Geolocator.requestAccessAsync().done(
            function (accessStatus) {
                switch (accessStatus) {
                    case Windows.Devices.Geolocation.GeolocationAccessStatus.allowed:

                        // If DesiredAccuracy or DesiredAccuracyInMeters are not set (or value is 0), DesiredAccuracy default is used.
                        var geolocator = new Windows.Devices.Geolocation.Geolocator();
                        geolocator.desiredAccuracyInMeters = desiredAccuracyInMeters;

                        getGeopositionPromise = geolocator.getGeopositionAsync();
                        getGeopositionPromise.done(
                            function (pos) {
                                if (!disposed) {
                                    WinJS.log && WinJS.log("Location updated", "sample", "status");

                                    updateLocationData(pos);
                                    document.getElementById("getGeopositionButton").disabled = false;
                                    document.getElementById("cancelGetGeopositionButton").disabled = true;
                                }
                            },
                            function (err) {
                                if (!disposed) {
                                    WinJS.log && WinJS.log(err.message, "sample", "error");

                                    updateLocationData(null);
                                    document.getElementById("getGeopositionButton").disabled = false;
                                    document.getElementById("cancelGetGeopositionButton").disabled = true;
                                }
                            }
                        );

                        WinJS.log && WinJS.log("Waiting for update...", "sample", "status");
                        document.getElementById("getGeopositionButton").disabled = true;
                        document.getElementById("cancelGetGeopositionButton").disabled = false;
                        break;

                    case Windows.Devices.Geolocation.GeolocationAccessStatus.denied:
                        WinJS.log && WinJS.log("Access to location is denied.", "sample", "error");
                        break;

                    case Windows.Devices.Geolocation.GeolocationAccessStatus.unspecified:
                        WinJS.log && WinJS.log("Unspecified error!", "sample", "error");
                        break;
                }
            },
            function (err) {
                WinJS.log && WinJS.log(err, "sample", "error");
            });
    }

    function cancelGetGeoposition() {
        if (getGeopositionPromise) {
            getGeopositionPromise.operation.cancel();
        }
        document.getElementById("getGeopositionButton").disabled = false;
        document.getElementById("cancelGetGeopositionButton").disabled = true;
    }

    function getDesiredAccuracy(evt) {
        var e = document.getElementById("desiredAccuracyInMeters");
        var stringValue = e.value;
        var value = parseInt(stringValue);
        var nullAllowed = true;
        var elementName = "Desired accuracy";

        if (!value) {
            var msg;
            // value is either empty or alphabetic
            if (0 !== stringValue.length) {
                WinJS.log && WinJS.log("Desired accuracy must be a number.", "sample", "error");
            }
        } else {
            // update value
            desiredAccuracyInMeters = value;

            // clear out status message
            WinJS.log && WinJS.log("", "sample", "status");
        }
    }

    function updateLocationData(pos) {
        if (pos == null) {
            document.getElementById("latitude").innerHTML = "No data";
            document.getElementById("longitude").innerHTML = "No data";
            document.getElementById("accuracy").innerHTML = "No data";
            document.getElementById("source").innerHTML = "No data";

            // hide labels and satellite data
            document.getElementById("positionLabel").style.opacity = 0;
            document.getElementById("position").style.opacity = 0;
            document.getElementById("horizontalLabel").style.opacity = 0;
            document.getElementById("horizontal").style.opacity = 0;
            document.getElementById("verticalLabel").style.opacity = 0;
            document.getElementById("vertical").style.opacity = 0;
        } else {
            var coord = pos.coordinate;
            document.getElementById("latitude").innerHTML = coord.point.position.latitude;
            document.getElementById("longitude").innerHTML = coord.point.position.longitude;
            document.getElementById("accuracy").innerHTML = coord.accuracy;
            document.getElementById("source").innerHTML = toPositionSource(coord.positionSource);

            if (coord.positionSource === Windows.Devices.Geolocation.PositionSource.satellite) {
                // show labels and satellite data
                document.getElementById("positionLabel").style.opacity = 1;
                document.getElementById("position").style.opacity = 1;
                document.getElementById("horizontalLabel").style.opacity = 1;
                document.getElementById("horizontal").style.opacity = 1;
                document.getElementById("verticalLabel").style.opacity = 1;
                document.getElementById("vertical").style.opacity = 1;
                document.getElementById("position").innerHTML = coord.satelliteData.positionDilutionOfPrecision;
                document.getElementById("horizontal").innerHTML = coord.satelliteData.horizontalDilutionOfPrecision;
                document.getElementById("vertical").innerHTML = coord.satelliteData.verticalDilutionOfPrecision;
            } else {
                // hide labels and satellite data
                document.getElementById("positionLabel").style.opacity = 0;
                document.getElementById("position").style.opacity = 0;
                document.getElementById("horizontalLabel").style.opacity = 0;
                document.getElementById("horizontal").style.opacity = 0;
                document.getElementById("verticalLabel").style.opacity = 0;
                document.getElementById("vertical").style.opacity = 0;
            }
        }
    }

    function toPositionSource(val) {
        switch (val) {
            case Windows.Devices.Geolocation.PositionSource.satellite:
                return "Satellite";
            case Windows.Devices.Geolocation.PositionSource.wiFi:
                return "Wifi";
            case Windows.Devices.Geolocation.PositionSource.cellular:
                return "cellular";
            case Windows.Devices.Geolocation.PositionSource.ipAddress:
                return "IPAddress";
            case Windows.Devices.Geolocation.PositionSource.unknown:
                return "Unknown";
        }
    }
})();