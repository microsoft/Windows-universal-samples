//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    function logError(msg) {
        WinJS.log && WinJS.log(msg, "", "error");
    }

    function logStatus(msg) {
        WinJS.log && WinJS.log(msg, "", "status");
    }

    // Apps need to call RequestAccessAsync to get user permission on location usage,
    // otherwise SetGeotagFromGeolocator will fail. Also RequestAccessAsync needs 
    // to be called from a UI thread.
    function requestLocationAccess() {
        try {
            Windows.Devices.Geolocation.Geolocator.requestAccessAsync().then(
                function (status) {
                    if (status != Windows.Devices.Geolocation.GeolocationAccessStatus.allowed) {
                        logStatus("Location access is NOT allowed");
                    }
                },
                function (error) {
                    logError("requestAccessAsync failed: " + error.message);
                }
            );
        } catch (error) {
            logError("Expection: " + error.message);
        }
    }

    function getGeotag() {
        try {
            var inputFilename = document.getElementById("filename").value;
            Windows.Storage.KnownFolders.picturesLibrary.getFileAsync(inputFilename).then(
                function (file) {
                    try {
                        Windows.Storage.FileProperties.GeotagHelper.getGeotagAsync(file).then(
                            function (result) {
                                if (result != null) {
                                    logStatus("getGeotagAsync complete - latitude: " + result.position.latitude + " longitude: " + result.position.longitude);
                                } else {
                                    logStatus("getGeotagAsync complete - location info not available")
                                }
                            },
                            function (error) {
                                logError("getGeotagAsync failed: " + error.message);
                            }
                        );
                    } catch (error) {
                        logError("Expection: " + error.message);
                    }
                },
                function (error) {
                    logError("getGeotagAsync failed: " + error.message);
                }
            );
        } catch (error) {
            logError("Expection: " + error.message);
        }
    }

    function setGeotagFromGeolocator() {
        try {
            var inputFilename = document.getElementById("filename").value;
            Windows.Storage.KnownFolders.picturesLibrary.getFileAsync(inputFilename).then(
                function (file) {
                    var geolocator = new Windows.Devices.Geolocation.Geolocator();
                    geolocator.DesiredAccuracy = Windows.Devices.Geolocation.PositionAccuracy.High;

                    Windows.Storage.FileProperties.GeotagHelper.setGeotagFromGeolocatorAsync(file, geolocator).then(
                        function () {
                            logStatus("setGeotagFromGeolocatorAsync complete");
                        },
                        function (error) {
                            logError("setGeotagFromGeolocatorAsync failed: " + error.message);
                        }
                    );
                },
                function (error) {
                    logError("getFileAsync failed: " + error.message);
                }
            );
        } catch (error) {
            logError("Expection: " + error.message);
        }
    }

    function setGeotag() {
        try {
            var inputFilename = document.getElementById("filename").value;
            Windows.Storage.KnownFolders.picturesLibrary.getFileAsync(inputFilename).then(
                function (file) {
                    try {
                        // Use latitude 10.0, longitude 20.0 as an example
                        var geopoint = new Windows.Devices.Geolocation.Geopoint({ latitude: 10.0, longitude: 20.0, altitude: 0.0 });
                        Windows.Storage.FileProperties.GeotagHelper.setGeotagAsync(file, geopoint).then(
                            function () {
                                logStatus("setGeotagAsync complete");
                            },
                            function (error) {
                                logError("setGeotagAsync failed: " + error.message);
                            }
                        );
                    } catch (error) {
                        logError("Expection: " + error.message);
                    }
                },
                function (error) {
                    logError("getFileAsync failed: " + error.message);
                }
            );
        } catch (error) {
            logError("Expection: " + error.message);
        }
    }

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("GetGeotagButton").addEventListener("click", getGeotag, false);
            document.getElementById("SetGeotagFromGeolocatorButton").addEventListener("click", setGeotagFromGeolocator, false);
            document.getElementById("SetGeotagButton").addEventListener("click", setGeotag, false);
            requestLocationAccess();
        }
    });
})();

