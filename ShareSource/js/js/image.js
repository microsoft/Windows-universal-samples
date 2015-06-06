//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var imageFile;

    var page = WinJS.UI.Pages.define("/html/image.html", {
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
                    if (scenario.url === "/html/image.html") {
                        request.data.properties.contentSourceApplicationLink = new Windows.Foundation.Uri(scenario.applink);
                    }
                });

                var streamReference = Windows.Storage.Streams.RandomAccessStreamReference.createFromFile(imageFile);
                request.data.properties.thumbnail = streamReference;

                // It's recommended to always use both setBitmap and setStorageItems for sharing a single image
                // since the Target app may only support one or the other

                // Put the image file in an array and pass it to setStorageItems
                request.data.setStorageItems([imageFile]);

                // The setBitmap method requires a RandomAccessStreamReference
                request.data.setBitmap(streamReference);
            } else {
                request.failWithDisplayText("Select an image you would like to share and try again.");
            }
        } else {
            request.failWithDisplayText(SdkSample.missingTitleError);
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
                // The imageFile variable will be set to shareValue when the user clicks Set
                imageFile = file;
                document.getElementById("scenarioOutput").className = "unhidden";
            }
        });
    }

    function showShareUI() {
        Windows.ApplicationModel.DataTransfer.DataTransferManager.showShareUI();
    }
})();
