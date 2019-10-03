//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Exception number constants. These constants are defined using values from winerror.h,
    // and are compared against error.number in the exception handlers in this scenario.

    // This file format does not support the requested operation; for example, metadata or thumbnails.
    var WINCODEC_ERR_UNSUPPORTEDOPERATION = Helpers.convertHResultToNumber(0x88982F81);
    // This file format does not support the requested property/metadata query.
    var WINCODEC_ERR_PROPERTYNOTSUPPORTED = Helpers.convertHResultToNumber(0x88982F41);
    // There is no codec or component that can handle the requested operation; for example, encoding.
    var WINCODEC_ERR_COMPONENTNOTFOUND = Helpers.convertHResultToNumber(0x88982F50);

    // Keep objects in-scope across the lifetime of the scenario.
    var FileToken = "";
    var DisplayWidthNonScaled = 0;
    var DisplayHeightNonScaled = 0;
    var ScaleFactor = 0;
    var UserRotation = 0;
    var ExifOrientation = 0;
    var DisableExifOrientation = false;

    // Namespace and API aliases
    var FutureAccess = Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList;
    var LocalSettings = Windows.Storage.ApplicationData.current.localSettings.values;

    function id(elementId) {
        return document.getElementById(elementId);
    }

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            id("buttonOpen").addEventListener("click", openHandler, false);
            id("buttonSave").addEventListener("click", saveHandler, false);
            id("buttonSaveAs").addEventListener("click", saveAsHandler, false);
            id("buttonClose").addEventListener("click", closeHandler, false);
            id("buttonRotateRight").addEventListener("click", rotateRightHandler, false);
            id("buttonRotateLeft").addEventListener("click", rotateLeftHandler, false);
            id("propertiesScaleFactorEdit").addEventListener("change", changeScaleHandler, false);
            WinJS.Application.addEventListener("checkpoint", saveToPersistedState, false);

            resetSessionState();

            // If the scenario has previously persisted state, restore it.
            if (LocalSettings.hasKey("scenario2FileToken")) {
                restoreImageFromPersistedState();
            }
        },

        unload: function () {
            // Only store the state of the currently open scenario.
            WinJS.Application.removeEventListener("checkpoint", saveToPersistedState, false);
        }
    });

    function closeHandler() {
        resetPersistedState();
        resetSessionState();
        WinJS.log && WinJS.log("Closed file", "sample", "status");
    }

    // Clear all of the state that is stored in application data.
    function resetPersistedState() {
        LocalSettings.remove("scenario2FileToken");
        LocalSettings.remove("scenario2Scale");
        LocalSettings.remove("scenario2Rotation");
    }

    // Clear all of the state that is stored in memory and in the UI.
    function resetSessionState() {
        // Variables width and height reflect rotation but not the scale factor.
        FileToken = "";
        DisplayWidthNonScaled = 0;
        DisplayHeightNonScaled = 0;
        ScaleFactor = 1;
        UserRotation = Windows.Storage.FileProperties.PhotoOrientation.normal;
        ExifOrientation = Windows.Storage.FileProperties.PhotoOrientation.normal;
        DisableExifOrientation = false;

        id("buttonRotateRight").disabled = true;
        id("buttonRotateLeft").disabled = true;
        id("buttonSave").disabled = true;
        id("buttonSaveAs").disabled = true;
        id("buttonClose").disabled = true;

        id("outputImage").src = "";
        id("outputImage").alt = "image holder";
        id("outputImage").onload = null;
        Helpers.applyRotationStyle(
            Windows.Graphics.Imaging.BitmapRotation.none,
            id("outputImage")
            );

        id("propertiesWidth").innerText = "";
        id("propertiesHeight").innerText = "";
        id("propertiesScaleFactor").innerText = "";
        id("propertiesScaleFactorEdit").value = 100;
        id("propertiesScaleFactorEdit").disabled = true;
        id("propertiesUserRotation").innerText = "";
        id("propertiesExifOrientation").innerText = "";
    }

    // Invoked when the user clicks the Open button.
    function openHandler() {
        resetSessionState();

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
        return displayImageFileAsync(file).done(function () {
            WinJS.log && WinJS.log("Loaded image file from picker: " + file.name, "sample", "status");
        }, function (error) {
            WinJS.log && WinJS.log("Failed to load file: " + error.message, "sample", "error");
            resetSessionState();
        });
    }

    // This function is called when the user clicks the Open button,
    // and when they save (to refresh the image state).
    function displayImageFileAsync(file) {
        // Request persisted access permissions to the file the user selected.
        // This allows the app to directly load the file in the future without relying on a
        // broker such as the file picker.
        FileToken = FutureAccess.add(file);

        id("outputImage").src = window.URL.createObjectURL(file, { oneTimeOnly: true });
        id("outputImage").alt = file.name;

        // Use BitmapDecoder to attempt to read EXIF orientation and image dimensions.
        return getImageInformationAsync(file).then(function () {
            id("propertiesExifOrientation").innerText =
                Helpers.getOrientationString(ExifOrientation);

            // Display the image dimensions and transformation state in the UI.
            updateDimensionsUI();

            id("buttonRotateRight").disabled = false;
            id("buttonRotateLeft").disabled = false;
            id("buttonSave").disabled = false;
            id("buttonSaveAs").disabled = false;
            id("buttonClose").disabled = false;
            id("propertiesScaleFactorEdit").disabled = false;
        });
    }

    // When the app resumes from a suspended state, restore the persisted file token and UI text/image.
    function restoreImageFromPersistedState() {
        FileToken = LocalSettings["scenario2FileToken"];
        var desiredScaleFactor = LocalSettings["scenario2Scale"];
        var desiredRotation = LocalSettings["scenario2Rotation"];

        // Keep data in-scope across multiple asynchronous methods.
        var file = {};
        FutureAccess.getFileAsync(FileToken).then(function (_file) {
            file = _file;

            id("outputImage").src = window.URL.createObjectURL(file, { oneTimeOnly: true });
            id("outputImage").alt = file.name;

            // Use BitmapDecoder to attempt to read EXIF orientation.
            return getImageInformationAsync(file);
        }).done(function () {
            id("propertiesExifOrientation").innerText =
                Helpers.getOrientationString(ExifOrientation);

            // Display the image dimensions and transformation state in the UI.
            updateDimensionsUI();

            // Restore the image tag's rotation transform.
            while (desiredRotation !== UserRotation) {
                rotateRightHandler();
            }

            id("buttonRotateRight").disabled = false;
            id("buttonRotateLeft").disabled = false;
            id("buttonSave").disabled = false;
            id("buttonSaveAs").disabled = false;
            id("buttonClose").disabled = false;
            id("propertiesScaleFactorEdit").disabled = false;

            // Restore the scale slider control value.
            id("propertiesScaleFactorEdit").value = desiredScaleFactor * 100;
            changeScaleHandler();

            WinJS.log && WinJS.log("Loaded image file from persisted state: " + file.name, "sample", "status");
        }, function (error) {
            WinJS.log && WinJS.log("Failed to load file: " + error.message, "sample", "error");
            resetSessionState();
        });
    }

    // Serialize file and user transform data to local storage.
    function saveToPersistedState() {
        // Only save state if we have valid data.
        if (FileToken !== "") {
            LocalSettings["scenario2FileToken"] = FileToken;
            LocalSettings["scenario2Scale"] = ScaleFactor;
            LocalSettings["scenario2Rotation"] = UserRotation;
        }
    }

    // Use the BitmapDecoder object to read EXIF orientation and image dimensions.
    function getImageInformationAsync(file) {
        var accessMode = Windows.Storage.FileAccessMode.read;

        // Keep data in-scope across multiple asynchronous methods.
        var stream;
        return file.openAsync(accessMode).then(function (_stream) {
            stream = _stream;
            return Windows.Graphics.Imaging.BitmapDecoder.createAsync(stream);
        }).then(function (decoder) {
            // The orientedPixelWidth and Height members provide the image dimensions
            // reflecting any EXIF orientation.
            DisplayWidthNonScaled = decoder.orientedPixelWidth;
            DisplayHeightNonScaled = decoder.orientedPixelHeight;

            // Property access using BitmapProperties is similar to using
            // Windows.Storage.FileProperties (see Scenario 1); BitmapProperties accepts
            // property keys such as "System.Photo.Orientation".
            // The EXIF orientation flag can be also be read using native metadata queries
            // such as "/app1/ifd/{ushort=274}" (JPEG) or "/ifd/{ushort=274}" (TIFF).
            return decoder.bitmapProperties.getPropertiesAsync(["System.Photo.Orientation"]);
        }).then(function (retrievedProps) {
            // Check to see if the property exists in the file.
            if (retrievedProps.hasKey("System.Photo.Orientation")) {
                ExifOrientation = retrievedProps["System.Photo.Orientation"].value;
            }
        }, function (error) {
            // If the file format does not support properties continue without applying EXIF orientation.
            switch (error.number) {
                case WINCODEC_ERR_UNSUPPORTEDOPERATION:
                case WINCODEC_ERR_PROPERTYNOTSUPPORTED:
                    DisableExifOrientation = true;
                    break;

                default:
                    throw error;
            }
        }).then(function () {
            stream && stream.close();
        });
    }

    function rotateRightHandler() {
        UserRotation = Helpers.add90DegreesCW(UserRotation);
        Helpers.applyRotationStyle(UserRotation, id("outputImage"));

        // Swap width and height.
        var temp = DisplayWidthNonScaled;
        DisplayWidthNonScaled = DisplayHeightNonScaled;
        DisplayHeightNonScaled = temp;
        updateDimensionsUI();
    }

    function rotateLeftHandler() {
        UserRotation = Helpers.add90DegreesCCW(UserRotation);
        Helpers.applyRotationStyle(UserRotation, id("outputImage"));

        // Swap width and height.
        var temp = DisplayWidthNonScaled;
        DisplayWidthNonScaled = DisplayHeightNonScaled;
        DisplayHeightNonScaled = temp;
        updateDimensionsUI();
    }

    // Applies the user-provided scale and rotation operation to the image file.
    // This method attempts to perform "soft" rotation using the EXIF orientation flag when possible,
    // but falls back to a hard rotation of the image pixels.
    function saveHandler() {
        WinJS.log && WinJS.log("Updating the file...", "sample", "status");

        // Keep data in-scope across multiple asynchronous methods.
        var originalWidth;
        var originalHeight;
        var encoder;
        var file;
        var fileStream;
        var memStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();

        // Create a new encoder and initialize it with data from the original file.
        // The encoder writes to an in-memory stream, we then copy the contents to the file.
        // This allows the application to perform in-place editing of the file: any unedited data
        // is copied directly to the destination, and the original file is overwritten
        // with updated data.
        FutureAccess.getFileAsync(FileToken).then(function (_file) {
            file = _file;
            // Open the file with read/write privileges as we will save the edited image in-place.
            return file.openAsync(Windows.Storage.FileAccessMode.readWrite);
        }).then(function (stream) {
            fileStream = stream;
            return Windows.Graphics.Imaging.BitmapDecoder.createAsync(fileStream);
        }).then(function (decoder) {
            originalHeight = decoder.pixelHeight;
            originalWidth = decoder.pixelWidth;

            // Set the encoder's destination to the temporary, in-memory stream.
            return Windows.Graphics.Imaging.BitmapEncoder.createForTranscodingAsync(memStream, decoder);
        }).then(function (_encoder) {
            encoder = _encoder;

            // Scaling occurs before flip/rotation, therefore use the original dimensions
            // (no orientation applied) as parameters for scaling.
            // Dimensions are rounded down by BitmapEncoder to the nearest integer.
            if (ScaleFactor !== 1) {
                encoder.bitmapTransform.scaledWidth = originalWidth * ScaleFactor;
                encoder.bitmapTransform.scaledHeight = originalHeight * ScaleFactor;

                // Fant is a relatively high quality interpolation mode.
                encoder.bitmapTransform.interpolationMode =
                    Windows.Graphics.Imaging.BitmapInterpolationMode.fant;
            }

            // Attempt to generate a new thumbnail to reflect any rotation operation.
            encoder.isThumbnailGenerated = true;

            // If the file format supports EXIF orientation ("System.Photo.Orientation") then
            // update the orientation flag to reflect any user-specified rotation.
            // Otherwise, perform a hard rotate using BitmapTransform.
            if (DisableExifOrientation === false) {
                var netExifOrientation = Helpers.addPhotoOrientation(ExifOrientation, UserRotation);

                // BitmapProperties requires the application to explicitly declare the type
                // of the property to be written - this is different from FileProperties which
                // automatically coerces the value to the correct type. System.Photo.Orientation
                // is defined as a UInt16.
                var orientationTypedValue = new Windows.Graphics.Imaging.BitmapTypedValue(
                    netExifOrientation,
                    Windows.Foundation.PropertyType.uint16
                    );

                var properties = new Windows.Graphics.Imaging.BitmapPropertySet();
                properties["System.Photo.Orientation"] = orientationTypedValue;
                return encoder.bitmapProperties.setPropertiesAsync(properties);
            } else {
                // Note that this return value gets wrapped in a new WinJS Promise.
                return encoder.bitmapTransform.rotation = Helpers.convertToBitmapRotation(UserRotation);
            }
        }).then(function () {
            return encoder.flushAsync();
        }).then(null, function (error) {
            switch (error.number) {
                // If the encoder does not support writing a thumbnail, then try again
                // but disable thumbnail generation.
                case WINCODEC_ERR_UNSUPPORTEDOPERATION:
                    encoder.isThumbnailGenerated = false;
                    return encoder.flushAsync();
                default:
                    throw error;
            }
        }).then(function () {
            // Overwrite the contents of the file with the updated image stream.
            memStream.seek(0);
            fileStream.seek(0);
            fileStream.size = 0;
            return Windows.Storage.Streams.RandomAccessStream.copyAsync(memStream, fileStream);
        }).then(function () {
            // Close each stream to release any locks.
            memStream && memStream.close();
            fileStream && fileStream.close();

            // Because the original file has been overwritten, reload it in the UI.
            resetSessionState();
            return displayImageFileAsync(file);
        }).done(function () {
            WinJS.log && WinJS.log("Successfully updated file: " + file.name, "sample", "status");
        }, function (error) {
            switch (error.number) {
                // Some image formats (e.g. ICO) do not have encoders.
                case WINCODEC_ERR_COMPONENTNOTFOUND:
                    WinJS.log && WinJS.log(
                        "Failed to update file: This file format may not support editing.",
                        "sample",
                        "error"
                        );

                    break;
                default:
                    WinJS.log && WinJS.log("Failed to update file: " + error.message, "sample", "error");
                    break;
            }

            memStream && memStream.close();
            fileStream && fileStream.close();
            resetPersistedState();
            resetSessionState();
        });
    }

    // Invoked when the user clicks the Save As button.
    function saveAsHandler() {
        var picker = new Windows.Storage.Pickers.FileSavePicker();

        // Restrict the user to a fixed list of file formats that support encoding.
        picker.fileTypeChoices.insert("PNG file", [".png"]);
        picker.fileTypeChoices.insert("BMP file", [".bmp"]);
        picker.fileTypeChoices.insert("JPEG file", [".jpg"]);
        picker.defaultFileExtension = ".png";
        picker.suggestedFileName = "Output file";
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;

        picker.pickSaveFileAsync().done(function (file) {
            // The returned file is null if the user cancelled the picker.
            file && loadSaveFileAsync(file);
        });
    }

    // Applies the user-provided scale and rotation operation to a new image file picked by the user.
    // This method writes the edited pixel data to the new file without
    // any regard to existing metadata or other information in the original file.
    function loadSaveFileAsync(file) {
        // Keep data in-scope across multiple asynchronous methods.
        var inputStream;
        var outputStream;
        var encoderId;
        var pixels;
        var pixelFormat;
        var alphaMode;
        var dpiX;
        var dpiY;
        var outputFilename;

        FutureAccess.getFileAsync(FileToken).then(function (inputFile) {
            return inputFile.openAsync(Windows.Storage.FileAccessMode.read);
        }).then(function (stream) {
            inputStream = stream;
            return Windows.Graphics.Imaging.BitmapDecoder.createAsync(inputStream);
        }).then(function (decoder) {
            var transform = new Windows.Graphics.Imaging.BitmapTransform();

            // Scaling occurs before flip/rotation, therefore use the original dimensions
            // (no orientation applied) as parameters for scaling.
            // Dimensions are rounded down by BitmapEncoder to the nearest integer.
            transform.scaledHeight = decoder.pixelHeight * ScaleFactor;
            transform.scaledWidth = decoder.pixelWidth * ScaleFactor;
            transform.rotation = Helpers.convertToBitmapRotation(UserRotation);

            // Fant is a relatively high quality interpolation mode.
            transform.interpolationMode = Windows.Graphics.Imaging.BitmapInterpolationMode.fant;

            // The BitmapDecoder indicates what pixel format and alpha mode best match the
            // natively stored image data. This can provide a performance and/or quality gain.
            pixelFormat = decoder.bitmapPixelFormat;
            alphaMode = decoder.bitmapAlphaMode;
            dpiX = decoder.dpiX;
            dpiY = decoder.dpiY;

            // Get pixel data from the decoder. We apply the user-requested transforms on the
            // decoded pixels to take advantage of potential optimizations in the decoder.
            return decoder.getPixelDataAsync(
                pixelFormat,
                alphaMode,
                transform,
                Windows.Graphics.Imaging.ExifOrientationMode.respectExifOrientation,
                Windows.Graphics.Imaging.ColorManagementMode.colorManageToSRgb
                );
        }).then(function (pixelProvider) {
            pixels = pixelProvider.detachPixelData();

            // The destination file was passed as an argument to loadSaveFileAsync().
            outputFilename = file.name;
            switch (file.fileType) {
                case ".jpg":
                    encoderId = Windows.Graphics.Imaging.BitmapEncoder.jpegEncoderId;
                    break;
                case ".bmp":
                    encoderId = Windows.Graphics.Imaging.BitmapEncoder.bmpEncoderId;
                    break;
                case ".png":
                default:
                    encoderId = Windows.Graphics.Imaging.BitmapEncoder.pngEncoderId;
                    break;
            }

            return file.openAsync(Windows.Storage.FileAccessMode.readWrite);
        }).then(function (stream) {
            outputStream = stream;

            // BitmapEncoder expects an empty output stream; the user may have selected a
            // pre-existing file.
            outputStream.size = 0;
            return Windows.Graphics.Imaging.BitmapEncoder.createAsync(encoderId, outputStream);
        }).then(function (encoder) {
            // Write the pixel data onto the encoder. Note that we can't simply use the
            // BitmapTransform.ScaledWidth and ScaledHeight members as the user may have
            // requested a rotation (which is applied after scaling).
            encoder.setPixelData(
                pixelFormat,
                alphaMode,
                DisplayWidthNonScaled * ScaleFactor,
                DisplayHeightNonScaled * ScaleFactor,
                dpiX,
                dpiY,
                pixels
                );

            return encoder.flushAsync();
        }).then(function () {
            WinJS.log && WinJS.log("Successfully saved a copy: " + outputFilename, "sample", "status");
        }, function (error) {
            WinJS.log && WinJS.log("Failed to update file: " + error.message, "sample", "error");
            resetSessionState();
            resetPersistedState();
        }).done(function () {
            // Finally, close each stream to release any locks.
            inputStream && inputStream.close();
            outputStream && outputStream.close();
        });
    }

    // Called when the user drags the scale factor slider control.
    function changeScaleHandler() {
        // The HTML range element uses percent while ScaleFactor is a coefficient (1 = 100%).
        ScaleFactor = id("propertiesScaleFactorEdit").value / 100;
        updateDimensionsUI();
    }

    // Display the image dimensions and transformation state in the UI.
    function updateDimensionsUI() {
        id("propertiesScaleFactor").innerText = id("propertiesScaleFactorEdit").value + "%";

        id("propertiesWidth").innerText =
            Math.floor(DisplayWidthNonScaled * ScaleFactor) + " pixels";

        id("propertiesHeight").innerText =
            Math.floor(DisplayHeightNonScaled * ScaleFactor) + " pixels";

        id("propertiesUserRotation").innerText =
            Helpers.getOrientationString(UserRotation);
    }
})();
