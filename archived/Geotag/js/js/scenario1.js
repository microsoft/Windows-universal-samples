//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // namespaces and classes
    var GeotagHelper = Windows.Storage.FileProperties.GeotagHelper;
    var Geolocator = Windows.Devices.Geolocation.Geolocator;
    var GeolocationAccessStatus = Windows.Devices.Geolocation.GeolocationAccessStatus;
    var Geopoint = Windows.Devices.Geolocation.Geopoint;

    // Local variables
    var file;

    // DOM elements
    var fileOperationsPanel;
    var fileDisplayName;

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            fileOperationsPanel = document.getElementById("fileOperationsPanel");
            fileDisplayName = document.getElementById("fileDisplayName");

            document.getElementById("chooseFileButton").addEventListener("click", chooseFile, false);
            document.getElementById("getGeotagButton").addEventListener("click", getGeotag, false);
            document.getElementById("setGeotagFromGeolocatorButton").addEventListener("click", setGeotagFromGeolocator, false);
            document.getElementById("setGeotagButton").addEventListener("click", setGeotag, false);
        }
    });

    function chooseFile() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.push(".jpg", ".jpeg", ".mp4");
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        picker.pickSingleFileAsync().done(function (pickedFile) {
            file = pickedFile;
            if (file) {
                fileDisplayName.innerText = file.displayName;
                fileOperationsPanel.style.display = "block";
            } else {
                fileOperationsPanel.style.display = "none";
            }
        });
    }

    function getGeotag() {
        GeotagHelper.getGeotagAsync(file).done(function (geopoint) {
            if (geopoint) {
                WinJS.log && WinJS.log("Latitude = " + geopoint.position.latitude
                    + ", Longitude = " + geopoint.position.longitude, "sample", "status");
            } else {
                // File I/O errors are converted to "No information".
                WinJS.log && WinJS.log("No location information available", "sample", "error");
            }
        });
    }

    function setGeotagFromGeolocator() {
        // Call RequestAccessAsync to get user permission on location usage.
        // Otherwise SetGeotagFromGeolocator will fail.
        Geolocator.requestAccessAsync().done(function (status) {
            if (status != GeolocationAccessStatus.allowed) {
                WinJS.log && WinJS.log("Location access is not allowed", "sample", "error");
            } else {
                var geolocator = new Geolocator();
                geolocator.desiredAccuracy = Windows.Devices.Geolocation.PositionAccuracy.high;
                GeotagHelper.setGeotagFromGeolocatorAsync(file, geolocator).done(function () {
                    WinJS.log && WinJS.log("Geolocation set to current location", "sample", "status");
                }, function (e) {
                    // File I/O errors are reported as exceptions.
                    // AccessDeniedExcetion can be raised if the user revoked location access
                    // after RequestAccessAsync completed.
                    WinJS.log && WinJS.log("Exception: " + e.message, "sample", "error");
                });
            }
        });
    }

    function setGeotag() {
        // Set the approximate position of the observation deck of the Seattle Space Needle.
        var geopoint = new Geopoint({ latitude: 47.620491, longitude: -122.349319, altitude: 158.12 });
        
        GeotagHelper.setGeotagAsync(file, geopoint).done(function() {
            WinJS.log && WinJS.log("Geolocation set to Seattle Space Needle", "sample", "status");
        }, function (e) {
            // File I/O errors are reported as exceptions
            WinJS.log && WinJS.log("Exception: " + e.message, "sample", "error");
        });
    }
})();

