//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var imageFile;

    var page = WinJS.UI.Pages.define("/html/delay-render.html", {
        ready: function (element, options) {
            imageFile = null;
            var dataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView();
            dataTransferManager.addEventListener("datarequested", dataRequested);
            document.getElementById("chooseImageButton").addEventListener("click", chooseImage, false);
            document.getElementById("shareButton").addEventListener("click", showShareUI, false);
        },
        unload: function () {
            var dataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView();
            dataTransferManager.removeEventListener("datarequested", dataRequested);
        }
    });

    function dataRequested(e) {
        var request = e.request;

        // Title is required
        var dataPackageTitle = document.getElementById("titleInputBox").value;
        if ((typeof dataPackageTitle === "string") && (dataPackageTitle !== "")) {
            if (imageFile) {
                request.data.properties.title = dataPackageTitle;

                // The description is optional.
                var dataPackageDescription = document.getElementById("descriptionInputBox").value;
                if ((typeof dataPackageDescription === "string") && (dataPackageDescription !== "")) {
                    request.data.properties.description = dataPackageDescription;
                }

                // Set the Content Source Application Link.
                SdkSample.scenarios.forEach(function (scenario) {
                    if (scenario.url === "/html/delay-render.html") {
                        request.data.properties.contentSourceApplicationLink = new Windows.Foundation.Uri(scenario.applink);
                    }
                });

                // When sharing an image, don't forget to set the thumbnail for the DataPackage
                var streamReference = Windows.Storage.Streams.RandomAccessStreamReference.createFromFile(imageFile);
                request.data.properties.thumbnail = streamReference;
                request.data.setDataProvider(Windows.ApplicationModel.DataTransfer.StandardDataFormats.bitmap, onDeferredImageRequested);
            } else {
                request.failWithDisplayText("Select an image you would like to share and try again.");
            }
        } else {
            request.failWithDisplayText(SdkSample.missingTitleError);
        }
    }

    /// <summary>
    /// Specific handler for requests that require the use of the deferral (Image only)
    /// </summary>
    function onDeferredImageRequested(request) {
        if (imageFile) {
            // Here we provide updated Bitmap data using delayed rendering
            var deferral = request.getDeferral();
            var imageDecoder, inMemoryStream;
            imageFile.openAsync(Windows.Storage.FileAccessMode.read).then(function (stream) {
                // Decode the image
                return Windows.Graphics.Imaging.BitmapDecoder.createAsync(stream);
            }).then(function (decoder) {
                // Re-encode the image at 50% width and height
                inMemoryStream = new Windows.Storage.Streams.InMemoryRandomAccessStream();
                imageDecoder = decoder;
                return Windows.Graphics.Imaging.BitmapEncoder.createForTranscodingAsync(inMemoryStream, decoder);
            }).then(function (encoder) {
                encoder.bitmapTransform.scaledWidth = imageDecoder.orientedPixelWidth * 0.5;
                encoder.bitmapTransform.scaledHeight = imageDecoder.orientedPixelHeight * 0.5;
                return encoder.flushAsync();
            }).done(function () {
                var streamReference = Windows.Storage.Streams.RandomAccessStreamReference.createFromStream(inMemoryStream);
                request.setData(streamReference);
                deferral.complete();
            }, function (e) {
                // didn't succeed, but we still need to release the deferral to avoid a hang in the target app
                deferral.complete();
            });
        }
    }

    function chooseImage() {
        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll([".jpg", ".bmp", ".gif", ".png"]);
        picker.viewMode = Windows.Storage.Pickers.PickerViewMode.thumbnail;
        picker.pickSingleFileAsync().done(function (file) {
            if (file) {
                // Display the image to the user
                document.getElementById("imageHolder").src = URL.createObjectURL(file, { oneTimeOnly: true });
                // The imageFile variable will be set to imageFile when the user clicks Set
                imageFile = file;
                document.getElementById("scenarioOutput").className = "unhidden";
            }
        });
    }

    function showShareUI() {
        Windows.ApplicationModel.DataTransfer.DataTransferManager.showShareUI();
    }
})();
