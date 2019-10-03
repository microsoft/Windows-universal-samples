//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Exception number constants. These constants are defined using values from winerror.h,
    // and are compared against error.number in the exception handlers in this scenario.

    // Either the requested value to set on the Windows property is not valid, or the Windows property
    // is not supported by the file format.
    var E_INVALIDARG = Helpers.convertHResultToNumber(0x80070057);

    // Keep objects in-scope across the lifetime of the scenario.
    var ImageProperties = {};
    var FileToken = "";

    // Define namespace and API aliases.
    var FutureAccess = Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList;
    var LocalSettings = Windows.Storage.ApplicationData.current.localSettings.values;

    function id(elementId) {
        return document.getElementById(elementId);
    }

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            id("buttonOpen").addEventListener("click", openHandler, false);
            id("buttonApply").addEventListener("click", saveHandler, false);
            id("buttonClose").addEventListener("click", closeHandler, false);
            WinJS.Application.addEventListener("checkpoint", saveToPersistedState, false);

            resetSessionState();

            // If the scenario has previously persisted state, restore it.
            if (LocalSettings.hasKey("scenario1FileToken")) {
                restoreImageFromPersistedState();
            }
        },

        unload: function () {
            // Only store the state of the currently open scenario.
            WinJS.Application.removeEventListener("checkpoint", saveToPersistedState, false);
        }
    });

    // Clear all of the state that is stored in application data.
    function resetPersistedState() {
        LocalSettings.remove("scenario1FileToken");
        LocalSettings.remove("scenario1PropertiesText");
    }

    // Clear all of the state that is stored in memory and in the UI.
    function resetSessionState() {
        ImageProperties = {};
        FileToken = "";

        id("buttonApply").disabled = true;
        id("buttonClose").disabled = true;
        id("outputImage").src = "";
        id("outputImage").alt = "image holder";
        id("propertiesTitle").value = "";
        id("propertiesKeywords").innerText = "";
        id("propertiesRating").selectedIndex = 0;
        id("propertiesDateTaken").innerText = "";
        id("propertiesMake").innerText = "";
        id("propertiesModel").innerText = "";
        id("propertiesOrientation").innerText = "";
        id("propertiesLatDeg").value = "";
        id("propertiesLatMin").value = "";
        id("propertiesLatSec").value = "";
        id("propertiesLatRef").value = "";
        id("propertiesLongDeg").value = "";
        id("propertiesLongMin").value = "";
        id("propertiesLongSec").value = "";
        id("propertiesLongRef").value = "";
        id("propertiesExposure").innerText = "";
        id("propertiesFNumber").innerText = "";
    }

    function closeHandler() {
        resetPersistedState();
        resetSessionState();
        WinJS.log && WinJS.log("Closed file", "sample", "status");
    }

    // Invoked when the user clicks on the Open button.
    function openHandler() {
        resetSessionState();
        resetPersistedState();

        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        Helpers.fillDecoderExtensions(picker.fileTypeFilter);

        picker.pickSingleFileAsync().done(function (file) {
            // The returned file is null if the user cancelled the picker.
            file && loadFileAsync(file);
        });
    }

    // Load an image from a file and display some basic imaging properties.
    function loadFileAsync(file) {
        // Request persisted access permissions to the file the user selected.
        // This allows the app to directly load the file in the future without relying on a
        // broker such as the file picker.
        FileToken = FutureAccess.add(file);

        // Windows.Storage.FileProperties.ImageProperties provides convenience access to
        // commonly-used properties such as GPS and keywords. It also accepts
        // queries for Windows property system keys such as "System.Photo.ExposureTime".
        return file.properties.getImagePropertiesAsync().then(function (imageProps) {
            ImageProperties = imageProps;

            var requests = [
                "System.Photo.ExposureTime",        // In seconds
                "System.Photo.FNumber"              // F-stop values defined by EXIF spec
            ];

            return ImageProperties.retrievePropertiesAsync(requests);
        }).done(function (retrievedProps) {
            // Format the properties into text to display in the UI.
            displayImageUI(file, getImagePropertiesForDisplay(retrievedProps));

            WinJS.log && WinJS.log("Loaded file from picker: " + file.name, "sample", "status");
            id("buttonApply").disabled = false;
            id("buttonClose").disabled = false;
        }, function (error) {
            WinJS.log && WinJS.log("Failed to load file: " + error.message, "sample", "error");
            resetSessionState();
        });
    }

    // Takes a property collection returned by GetPropertiesAsync and formats the values
    // into an object suitable for displayImageUI().
    function getImagePropertiesForDisplay(retrievedProps) {
        // If the specified property doesn't exist, its value will be null.
        var orientationText = Helpers.getOrientationString(ImageProperties.orientation);

        var exposureText = retrievedProps["System.Photo.ExposureTime"] ?
            retrievedProps["System.Photo.ExposureTime"] * 1000 + " ms" : "";

        var fNumberText = retrievedProps["System.Photo.FNumber"] ?
            retrievedProps["System.Photo.FNumber"].toFixed(1) : "";

        var latRefText = "";
        var longRefText = "";
        var latDegrees = 0;
        var latMinutes = 0;
        var latSeconds = 0;
        var longDegrees = 0;
        var longMinutes = 0;
        var longSeconds = 0;
        var lat = ImageProperties.latitude;
        var long = ImageProperties.longitude;

        // Do a simple check if GPS data exists.
        if (lat && long) {
            // Latitude and longitude are returned as double precision numbers,
            // but we want to convert to degrees/minutes/seconds format.
            // Note that you can also get GPS data using the "System.GPS.Latitude"
            // and "System.GPS.Longitude" property keys, which returns geolocation
            // in degrees/minutes/seconds format. See Scenario 2 for more information.
            latRefText = (lat >= 0) ? "N" : "S";
            longRefText = (long >= 0) ? "E" : "W";
            latDegrees = Math.floor(Math.abs(lat));
            latMinutes = Math.floor((Math.abs(lat) - latDegrees) * 60);
            latSeconds = (Math.abs(lat) - latDegrees - latMinutes / 60) * 3600;
            longDegrees = Math.floor(Math.abs(long));
            longMinutes = Math.floor((Math.abs(long) - longDegrees) * 60);
            longSeconds = (Math.abs(long) - longDegrees - longMinutes / 60) * 3600;
        }

        return {
            "title": ImageProperties.title,
            "keywords": ImageProperties.keywords, // array of strings
            "rating": ImageProperties.rating, // number
            "dateTaken": ImageProperties.dateTaken,
            "make": ImageProperties.cameraManufacturer,
            "model": ImageProperties.cameraModel,
            "orientation": orientationText,
            "latRef": latRefText,
            "longRef": longRefText,
            "latDegrees": latDegrees ? latDegrees.toString() : "",
            "latMinutes": latMinutes ? latMinutes.toString() : "",
            "latSeconds": latSeconds ? latSeconds.toString() : "",
            "longDegrees": longDegrees ? longDegrees.toString() : "",
            "longMinutes": longMinutes ? longMinutes.toString() : "",
            "longSeconds": longSeconds ? longSeconds.toString() : "",
            "exposure": exposureText,
            "fNumber": fNumberText
        };
    }

    // When the app resumes from a suspended state, restore the persisted file token and UI text/image.
    function restoreImageFromPersistedState() {
        FileToken = LocalSettings["scenario1FileToken"];

        // Keep data in-scope across multiple asynchronous methods.
        var file = {};
        FutureAccess.getFileAsync(FileToken).then(function (_file) {
            file = _file;
            return file.properties.getImagePropertiesAsync();
        }).done(function (imageProps) {
            ImageProperties = imageProps;
            displayImageUI(file, JSON.parse(LocalSettings["scenario1PropertiesText"]));

            WinJS.log && WinJS.log("Loaded file from persisted state: " + file.name, "sample", "status");
            id("buttonApply").disabled = false;
            id("buttonClose").disabled = false;
        }, function (error) {
            WinJS.log && WinJS.log("Failed to load file: " + error.message, "sample", "error");
            resetSessionState();
        });
    }

    // Serialize file and properties data to local storage.
    function saveToPersistedState() {
        // Only save state if we have valid data.
        if (FileToken !== "") {
            // Keywords is an array of strings. Convert from the textbox which separates
            // each keyword by a newline.
            var keywordsText = [];
            if (id("propertiesKeywords").value !== "") {
                keywordsText = id("propertiesKeywords").value.split("\n");
            }

            var propertyTextObject = {
                "title": id("propertiesTitle").value,
                "keywords": keywordsText, // array of strings
                "rating": Helpers.convertStarsToSystemRating(
                    id("propertiesRating").selectedIndex
                    ), // number

                "dateTaken": id("propertiesDateTaken").innerText,
                "make": id("propertiesMake").innerText,
                "model": id("propertiesModel").innerText,
                "orientation": id("propertiesOrientation").innerText,
                "latRef": id("propertiesLatRef").value,
                "longRef": id("propertiesLongRef").value,
                "latDegrees": id("propertiesLatDeg").value,
                "latMinutes": id("propertiesLatMin").value,
                "latSeconds": id("propertiesLatSec").value,
                "longDegrees": id("propertiesLongDeg").value,
                "longMinutes": id("propertiesLongMin").value,
                "longSeconds": id("propertiesLongSec").value,
                "exposure": id("propertiesExposure").innerText,
                "fNumber": id("propertiesFNumber").innerText
            };

            LocalSettings["scenario1PropertiesText"] = JSON.stringify(propertyTextObject);
            LocalSettings["scenario1FileToken"] = FileToken;
        }
    }

    // Display the image file and properties text in the UI. This method is called
    // when the user loads a new file, or when the app resumes from a suspended state.
    // This method takes two parameters:
    // 1. StorageFile containing the image to be displayed
    // 2. Object with the property text to be displayed:
    //    title, rating, dateTaken, keywords, make, model, latRef, longRef,
    //    latDegrees, latMinutes, latSeconds, longDegrees, longMinutes, longSeconds.
    //    All members are strings except for keywords (string array) and rating (number).
    function displayImageUI(file, propertyText) {
        // Create a oneshot object URL that is not reusable.
        id("outputImage").src = window.URL.createObjectURL(file, { oneTimeOnly: true });
        id("outputImage").alt = file.name;

        id("propertiesTitle").value = propertyText.title;
        id("propertiesRating").selectedIndex =
            Helpers.convertSystemRatingToStars(propertyText.rating);

        id("propertiesDateTaken").innerText = propertyText.dateTaken;
        id("propertiesMake").innerText = propertyText.make;
        id("propertiesModel").innerText = propertyText.model;
        id("propertiesOrientation").innerText = propertyText.orientation;
        id("propertiesLatDeg").value = propertyText.latDegrees;
        id("propertiesLatMin").value = propertyText.latMinutes;

        id("propertiesLatRef").value = propertyText.latRef;
        id("propertiesLongDeg").value = propertyText.longDegrees;
        id("propertiesLongMin").value = propertyText.longMinutes;
        id("propertiesLongRef").value = propertyText.longRef;
        id("propertiesExposure").innerText = propertyText.exposure;
        id("propertiesFNumber").innerText = propertyText.fNumber;

        // Truncate the latitude and longitude seconds data to 4 decimal places (precision of 1/10000th).
        id("propertiesLatSec").value = propertyText.latSeconds.substring(0, 7);
        id("propertiesLongSec").value = propertyText.longSeconds.substring(0, 7);

        // Keywords is an array of strings. For simplicity place all of the keywords in a
        // single text box, separated by newlines.
        id("propertiesKeywords").value = propertyText.keywords.join("\n");
    }

    // Invoked when the user clicks the Save button. Save the edited properties to the image file.
    function saveHandler() {
        WinJS.log && WinJS.log("Updating properties...", "sample", "status");

        ImageProperties.title = id("propertiesTitle").value;

        // Keywords are stored as an array of strings. Split the textarea text by newlines.
        ImageProperties.keywords.clear();
        if (id("propertiesKeywords").value !== "") {
            var keywordsArray = id("propertiesKeywords").value.split("\n");

            keywordsArray.forEach(function (keyword) {
                ImageProperties.keywords.append(keyword);
            });
        }

        var properties = new Windows.Foundation.Collections.PropertySet();

        // When writing the rating, use the "System.Rating" property key.
        // ImageProperties.rating does not handle setting the value to 0 (no stars/unrated).
        properties["System.Rating"] = Helpers.convertStarsToSystemRating(
            id("propertiesRating").selectedIndex
            );

        var gpsWriteFailed = false;
        var latitude = [
            parseInt(id("propertiesLatDeg").value),
            parseInt(id("propertiesLatMin").value),
            parseFloat(id("propertiesLatSec").value)
        ];

        var longitude = [
            parseInt(id("propertiesLongDeg").value),
            parseInt(id("propertiesLongMin").value),
            parseFloat(id("propertiesLongSec").value)
        ];

        var latitudeRef = id("propertiesLatRef").value.toUpperCase();
        var longitudeRef = id("propertiesLongRef").value.toUpperCase();

        // Perform some simple validation on the GPS data before storing the values.
        if ((latitude[0] >= 0 && latitude[0] <= 90) &&
            (latitude[1] >= 0 && latitude[1] <= 60) &&
            (latitude[2] >= 0 && latitude[2] <= 60) &&
            (latitudeRef === "N" || latitudeRef === "S") &&
            (longitude[0] >= 0 && longitude[0] <= 180) &&
            (latitude[1] >= 0 && longitude[1] <= 60) &&
            (longitude[2] >= 0 && longitude[2] <= 60) &&
            (longitudeRef === "E" || longitudeRef === "W")) {
            properties["System.GPS.LatitudeRef"] = latitudeRef;
            properties["System.GPS.LongitudeRef"] = longitudeRef;

            // The Latitude and Longitude properties are read-only. Instead,
            // write to System.GPS.LatitudeNumerator, LatitudeDenominator, etc.
            // These are length 3 arrays of integers. For simplicity, the
            // seconds data is rounded to the nearest 10000th.
            var latNum = new Uint32Array([
                latitude[0],
                latitude[1],
                parseInt(latitude[2] * 10000)
            ]);

            var longNum = new Uint32Array([
                longitude[0],
                longitude[1],
                parseInt(longitude[2] * 10000)
            ]);

            var latDen = new Uint32Array([1, 1, 10000]);
            var longDen = new Uint32Array([1, 1, 10000]);

            properties["System.GPS.LatitudeNumerator"] = latNum;
            properties["System.GPS.LongitudeNumerator"] = longNum;
            properties["System.GPS.LatitudeDenominator"] = latDen;
            properties["System.GPS.LongitudeDenominator"] = longDen;
        } else {
            gpsWriteFailed = true;
        }

        ImageProperties.savePropertiesAsync(properties).done(function () {
            WinJS.log && WinJS.log(
                gpsWriteFailed ? "GPS data invalid; other properties successfully updated" :
                    "All properties successfully updated",
                "sample",
                "status"
                );

        }, function (error) {
            switch (error.number) {
                case E_INVALIDARG:
                    // Some imaging formats, such as PNG and BMP, do not support Windows properties.
                    // Other formats do not support all Windows properties.
                    // For example, JPEG does not support System.FlagStatus.
                    WinJS.log && WinJS.log(
                        "Error: One or more properties may not be supported by this file format.",
                        "sample",
                        "error"
                        );

                    break;

                default:
                    WinJS.log && WinJS.log("Failed to update properties: " + error.message, "sample", "error");
                    break;
            }

            resetSessionState();
        });
    }
})();
