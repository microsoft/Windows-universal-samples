//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/image.html", {
        ready: function (element, options) {
            document.getElementById("scenario2Copy").addEventListener("click", scenario2Copy_Click, false);
            document.getElementById("scenario2CopyDelayedRendering").addEventListener("click", scenario2CopyDelayedRendering_Click, false);
            document.getElementById("scenario2Paste").addEventListener("click", scenario2Paste, false);
        }
    });

    function displayStatus(statusString) {
        return WinJS.log && WinJS.log(statusString, "sample", "status");
    }

    function displayError(errorString) {
        return WinJS.log && WinJS.log(errorString, "sample", "error");
    }

    var imageFile = null;

    function scenario2ClearOutput() {
        displayError("");
        displayStatus("");
        document.getElementById("scenario2ImageHolder").style.visibility = "hidden";
        document.getElementById("scenario2ImageHolder").src = "/images/placeholder-sdk.png";
    }

    function scenario2Copy_Click() {
        scenario2Copy(false);
    }

    function scenario2CopyDelayedRendering_Click() {
        scenario2Copy(true);
    }

    function scenario2Copy(isDelayRendered) {
        scenario2ClearOutput();

        imageFile = null;
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.viewMode = Windows.Storage.Pickers.PickerViewMode.thumbnail;
        picker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        picker.fileTypeFilter.replaceAll([".jpg", ".jpeg", ".png", ".bmp"]);
        picker.pickSingleFileAsync().done(function (file) {
            if (file) {
                var dataPackage = new Windows.ApplicationModel.DataTransfer.DataPackage();
                var statusText;

                if (isDelayRendered) {
                    // Delay providing the data until delegate is called
                    imageFile = file;
                    dataPackage.setDataProvider(Windows.ApplicationModel.DataTransfer.StandardDataFormats.bitmap, onDeferredImageRequestedHandler);

                    statusText = "Image copied to Clipboard using delayed rendering";
                } else {
                    // Add the selected image to the DataPackage
                    dataPackage.setBitmap(Windows.Storage.Streams.RandomAccessStreamReference.createFromFile(file));

                    statusText = "Image copied to Clipboard";
                }

                try {
                    // copy the content to Clipboard
                    Windows.ApplicationModel.DataTransfer.Clipboard.setContent(dataPackage);

                    displayStatus(statusText);
                } catch (e) {
                    // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                    displayError("Error copying content to Clipboard: " + e + ". Try again.");
                }
            } else {
                displayStatus("No image selected");
            }
        });
    }

    function onDeferredImageRequestedHandler(request) {
        if (imageFile) {
            // Since this method is calling an async method prior to setting the data in DataPackage,
            // deferral object must be used
            var deferral = request.getDeferral();

            var imageStream,
                imageDecoder,
                inMemoryStream;
            imageFile.openAsync(Windows.Storage.FileAccessMode.Read).then(function (stream) {
                imageStream = stream;
                // Decode the image
                return Windows.Graphics.Imaging.BitmapDecoder.createAsync(imageStream);
            }).then(function (decoder) {
                imageDecoder = decoder;
                // Re-encode the image at 50% width and height
                inMemoryStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
                return Windows.Graphics.Imaging.BitmapEncoder.createForTranscodingAsync(inMemoryStream, imageDecoder);
            }).then(function (imageEncoder) {
                imageEncoder.bitmapTransform.scaledWidth = imageDecoder.orientedPixelWidth / 2;
                imageEncoder.bitmapTransform.scaledHeight = imageDecoder.orientedPixelHeight / 2;
                return imageEncoder.flushAsync();
            }).done(function () {
                request.setData(Windows.Storage.Streams.RandomAccessStreamReference.createFromStream(inMemoryStream));

                // Data has been set, now we need to signal completion of the operation
                deferral.complete();
                imageStream.close();
            }, function () {
                // Didn't succeed, but we still need to release deferral, to avoid a hang in the target app
                deferral.complete();
                imageStream && imageStream.close();
            });
        }
    }

    function scenario2Paste() {
        scenario2ClearOutput();

        // Get the content from clipboard
        var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.getContent();

        if (dataPackageView.contains(Windows.ApplicationModel.DataTransfer.StandardDataFormats.bitmap)) {
            dataPackageView.getBitmapAsync().then(function (streamRef) {
                return streamRef.openReadAsync();
            }).done(function (bitmapStream) {
                document.getElementById("scenario2ImageHolder").src = window.URL.createObjectURL(bitmapStream, { oneTimeOnly: true });
                document.getElementById("scenario2ImageHolder").style.visibility = "visible";
                displayStatus("Image pasted from Clipboard");
            }, function (e) {
                displayError("Error retrieving image stream:  " + e);
            });
        } else {
            displayStatus("Bitmap format is not available in clipboard");
        }
    }
})();
