//// Copyright (c) Microsoft Corporation. All rights reserved

// Provides shared imaging functionality to all scenarios, focusing on
// EXIF orientation, GPS, and rating metadata.
(function () {
    "use strict";

    // Converts a failure HRESULT (Windows error code) to a number which can be compared to the
    // WinRTError.number parameter. This allows us to define error codes in terms of well-known
    // Windows HRESULTs, found in winerror.h.
    function convertHResultToNumber(hresult) {
        if ((hresult > 0xFFFFFFFF) || (hresult < 0x80000000)) {
            throw new Error("Value is not a failure HRESULT.");
        }

        return hresult - 0xFFFFFFFF - 1;
    }

    // Gets the file extensions supported by all of the bitmap codecs installed on the system.
    // The "collection" argument is of type IVector and implements the append method. The
    // function does not return a value; instead, it populates the collection argument with
    // the file extensions.
    function fillDecoderExtensions(collection) {
        var enumerator = Windows.Graphics.Imaging.BitmapDecoder.getDecoderInformationEnumerator();

        enumerator.forEach(function (decoderInfo) {
            // Each bitmap codec contains a list of file extensions it supports; get this list
            // and append every element in the list to "collection".
            decoderInfo.fileExtensions.forEach(function (fileExtension) {
                collection.append(fileExtension);
            });
        });
    }

    // Calculates the dimensions necessary to fit an HTML element within a square of a
    // specified side length, while preserving aspect ratio. For example, a 500x250 element
    // must be scaled to 300x150 in order to fit within a square with side length 300.
    // Does not rescale dimensions if the element fits entirely within the square.
    function getScaledDimensions(width, height, maxSideLength) {
        var scaleFactor = maxSideLength / Math.max(width, height);
        var dimensions = {};

        if (scaleFactor >= 1) {
            dimensions.width = width;
            dimensions.height = height;
        } else {
            dimensions.width = Math.floor(width * scaleFactor);
            dimensions.height = Math.floor(height * scaleFactor);
        }

        return dimensions;
    }

    //
    //  ***** Rating helpers (System.Rating property and 5 star ratings) *****
    //

    // convertSystemRatingToStars and convertStarsToSystemRating convert between the
    // "System.Rating" value (1-99) and a typical 5 star rating system.
    // The behavior is consistent with how Windows handles "System.Rating", and is
    // documented at:
    // http://msdn.microsoft.com/en-us/library/bb787554(v=VS.85).aspx
    // A value of 0 is equivalent to 0 stars/no rating.
    // This function only works for rating controls with 5 values.
    function convertSystemRatingToStars(systemRating) {
        if (systemRating <= 0) {
            return 0;
        } else if (systemRating <= 12) {
            return 1;
        } else if (systemRating <= 37) {
            return 2;
        } else if (systemRating <= 62) {
            return 3;
        } else if (systemRating <= 87) {
            return 4;
        } else {
            return 5;
        }
    }

    function convertStarsToSystemRating(numStars) {
        if (numStars <= 0) {
            return 0;
        } else if (numStars === 1) {
            return 1;
        } else if (numStars === 2) {
            return 25;
        } else if (numStars === 3) {
            return 50;
        } else if (numStars === 4) {
            return 75;
        } else {
            return 99;
        }
    }

    //
    //  ***** GPS/Geolocation property helpers *****
    //

    // ImageProperties.Latitude and ImageProperties.Longitude are returned
    // as double precision numbers. This function converts them to a 
    // degrees/minutes/seconds/reference ("N/E/W/S") format.
    // latLong is either a signed latitude or longitude value.
    // isLatitude is a boolean indicating whether latLong is latitude or longitude.
    function convertLatLongToString(latLong, isLatitude) {
        var reference;
        if (isLatitude) {
            reference = (latLong >= 0) ? "N" : "S";
        } else {
            reference = (latLong >= 0) ? "E" : "W";
        }

        latLong = Math.abs(latLong);
        var degrees = Math.floor(latLong);
        var minutes = Math.floor((latLong - degrees) * 60);
        var seconds = ((latLong - degrees - minutes / 60) * 3600).toFixed(2);

        return degrees + "°" + minutes + "\'" + seconds + "\"" + reference;
    }

    //
    //  ***** EXIF orientation helpers *****
    //

    // Converts a PhotoOrientation value into a human-readable string.
    // The text is adapted from the EXIF specification.
    // Note that PhotoOrientation uses a counterclockwise convention,
    // while the EXIF spec uses a clockwise convention.
    function getOrientationString(photoOrientation) {
        switch (photoOrientation) {
            case Windows.Storage.FileProperties.PhotoOrientation.normal:
                return "No rotation";
            case Windows.Storage.FileProperties.PhotoOrientation.flipHorizontal:
                return "Flip horizontally";
            case Windows.Storage.FileProperties.PhotoOrientation.rotate180:
                return "Rotate 180° clockwise";
            case Windows.Storage.FileProperties.PhotoOrientation.flipVertical:
                return "Flip vertically";
            case Windows.Storage.FileProperties.PhotoOrientation.transpose:
                return "Rotate 270° clockwise, then flip horizontally";
            case Windows.Storage.FileProperties.PhotoOrientation.rotate270:
                return "Rotate 90° clockwise";
            case Windows.Storage.FileProperties.PhotoOrientation.transverse:
                return "Rotate 90° clockwise, then flip horizontally";
            case Windows.Storage.FileProperties.PhotoOrientation.rotate90:
                return "Rotate 270° clockwise";
            case Windows.Storage.FileProperties.PhotoOrientation.unspecified:
            default:
                return "Unspecified";
        }
    }

    // Converts a Windows.Storage.FileProperties.PhotoOrientation value into a
    // Windows.Graphics.Imaging.BitmapRotation value.
    // For PhotoOrientation values reflecting a flip/mirroring operation, returns "None";
    // therefore this is a potentially lossy transformation.
    // Note that PhotoOrientation uses a counterclockwise convention,
    // while BitmapRotation uses a clockwise convention.
    function convertToBitmapRotation(photoOrientation) {
        switch (photoOrientation) {
            case Windows.Storage.FileProperties.PhotoOrientation.normal:
                return Windows.Graphics.Imaging.BitmapRotation.none;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate270:
                return Windows.Graphics.Imaging.BitmapRotation.clockwise90Degrees;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate180:
                return Windows.Graphics.Imaging.BitmapRotation.clockwise180Degrees;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate90:
                return Windows.Graphics.Imaging.BitmapRotation.clockwise270Degrees;
            default:
                // Ignore any flip/mirrored values.
                return BitmapRotation.none;
        }
    }

    // Converts a Windows.Graphics.Imaging.BitmapRotation value into a
    // Windows.Storage.FileProperties.PhotoOrientation value.
    // Note that PhotoOrientation uses a counterclockwise convention,
    // while BitmapRotation uses a clockwise convention.
    function convertToPhotoOrientation(bitmapRotation) {
        switch (bitmapRotation) {
            case Windows.Graphics.Imaging.BitmapRotation.none:
                return Windows.Storage.FileProperties.PhotoOrientation.normal;
            case Windows.Graphics.Imaging.BitmapRotation.clockwise90Degrees:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate270;
            case Windows.Graphics.Imaging.BitmapRotation.clockwise180Degrees:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate180;
            case Windows.Graphics.Imaging.BitmapRotation.clockwise270Degrees:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate90;
            default:
                return PhotoOrientation.normal;
        }
    }

    // "Adds" two PhotoOrientation values. For simplicity, does not handle any values with
    // flip/mirroring; therefore this is a potentially lossy transformation.
    // Note that PhotoOrientation uses a counterclockwise convention.
    function addPhotoOrientation(photoOrientation1, photoOrientation2) {
        switch (photoOrientation2) {
            case Windows.Storage.FileProperties.PhotoOrientation.rotate90:
                return Helpers.add90DegreesCCW(photoOrientation1);
            case Windows.Storage.FileProperties.PhotoOrientation.rotate180:
                return Helpers.add90DegreesCCW(Helpers.add90DegreesCCW(photoOrientation1));
            case Windows.Storage.FileProperties.PhotoOrientation.rotate270:
                return Helpers.add90DegreesCW(photoOrientation1);
            case Windows.Storage.FileProperties.PhotoOrientation.normal:
            default:
                // Ignore any values with flip/mirroring.
                return photoOrientation1;
        }
    }

    // "Add" 90 degrees clockwise rotation to a PhotoOrientation value.
    // For simplicity, does not handle any values with flip/mirroring; therefore this is a potentially
    // lossy transformation.
    // Note that PhotoOrientation uses a counterclockwise convention.
    function add90DegreesCW(photoOrientation) {
        switch (photoOrientation) {
            case Windows.Storage.FileProperties.PhotoOrientation.normal:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate270;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate90:
                return Windows.Storage.FileProperties.PhotoOrientation.normal;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate180:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate90;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate270:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate180;
            default:
                // Ignore any values with flip/mirroring.
                return Windows.Storage.FileProperties.PhotoOrientation.unspecified;
        }
    }

    // "Add" 90 degrees counter-clockwise rotation to a PhotoOrientation value.
    // For simplicity, does not handle any values with flip/mirroring; therefore this is a potentially
    // lossy transformation.
    // Note that PhotoOrientation uses a counterclockwise convention.
    function add90DegreesCCW(photoOrientation) {
        switch (photoOrientation) {
            case Windows.Storage.FileProperties.PhotoOrientation.normal:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate90;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate90:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate180;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate180:
                return Windows.Storage.FileProperties.PhotoOrientation.rotate270;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate270:
                return Windows.Storage.FileProperties.PhotoOrientation.normal;
            default:
                // Ignore any values with flip/mirroring.
                return Windows.Storage.FileProperties.PhotoOrientation.unspecified;
        }
    }

    // Modifies the style of an HTML element to reflect the specified PhotoOrientation.
    // Ignores any values with flip/mirroring.
    // Note that PhotoOrientation uses a counterclockwise convention.
    function applyRotationStyle(photoOrientation, htmlElement) {
        var style = htmlElement.style;
        switch (photoOrientation) {
            case Windows.Storage.FileProperties.PhotoOrientation.rotate270:
                style.transform = "rotate(90deg)";
                break;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate180:
                style.transform = "rotate(180deg)";
                break;
            case Windows.Storage.FileProperties.PhotoOrientation.rotate90:
                style.transform = "rotate(270deg)";
                break;
            case Windows.Storage.FileProperties.PhotoOrientation.normal:
            default:
                // Ignore any values with flip/mirroring.
                style.transform = "";
                break;
        }
    }

    // Expose the methods in the Helpers namespace.
    WinJS.Namespace.define("Helpers", {
        "convertHResultToNumber": convertHResultToNumber,
        "fillDecoderExtensions": fillDecoderExtensions,
        "getScaledDimensions": getScaledDimensions,
        "convertSystemRatingToStars": convertSystemRatingToStars,
        "convertStarsToSystemRating": convertStarsToSystemRating,
        "convertLatLongToString": convertLatLongToString,
        "getOrientationString": getOrientationString,
        "convertToBitmapRotation": convertToBitmapRotation,
        "convertToPhotoOrientation": convertToPhotoOrientation,
        "addPhotoOrientation": addPhotoOrientation,
        "add90DegreesCW": add90DegreesCW,
        "add90DegreesCCW": add90DegreesCCW,
        "applyRotationStyle": applyRotationStyle
    });
})();