//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario6.html", {
        ready: function (element, options) {
            document.getElementById("picture-thumb-start").addEventListener("click", pickPhoto, false);
        }
    });

    function pickPhoto() {
        // Get scenario options from HTML
        var isFastSelected = document.getElementById("picture-thumb-fastThumbnail").checked;

        var requestedSize = parseInt(document.getElementById("picture-thumb-requestSize").value);
        if (requestedSize > 0 && !isNaN(requestedSize)) {
            // Set API parameters
            var thumbnailModeName = "ThumbnailMode.singleItem",
                thumbnailMode = Windows.Storage.FileProperties.ThumbnailMode.singleItem,
                thumbnailOptions = Windows.Storage.FileProperties.ThumbnailOptions.useCurrentScale;

            if (isFastSelected) {
                thumbnailOptions |= Windows.Storage.FileProperties.ThumbnailOptions.returnOnlyIfCached;
            }

            // Clean output in case of repeat usage
            cleanOutput();

            var openpicker = new Windows.Storage.Pickers.FileOpenPicker();
            openpicker.fileTypeFilter.replaceAll([".jpg", ".png", ".bmp", ".gif", ".tif"]);
            openpicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
            openpicker.pickSingleFileAsync().done(function (file) {
                if (file) {
                    file.getScaledImageAsThumbnailAsync(thumbnailMode, requestedSize, thumbnailOptions).done(function (thumbnail) {
                        if (thumbnail) {
                            outputResult(file, thumbnail, thumbnailModeName, requestedSize);
                        } else if (isFastSelected) {
                            WinJS.log && WinJS.log(SdkSample.errors.noExif, "sample", "status");
                        } else {
                            WinJS.log && WinJS.log(SdkSample.errors.noThumbnail, "sample", "status");
                        }
                    }, function (error) {
                        WinJS.log && WinJS.log(SdkSample.errors.fail, "sample", "status");
                    });
                } else {
                    WinJS.log && WinJS.log(SdkSample.errors.cancel, "sample", "status");
                }
            });
        } else {
            WinJS.log && WinJS.log(SdkSample.errors.invalidSize, "sample", "error");
        }
    }

    function outputResult(item, thumbnailImage, thumbnailMode, requestedSize) {
        document.getElementById("picture-thumb-imageHolder").src = URL.createObjectURL(thumbnailImage, { oneTimeOnly: true });
        // Close the thumbnail stream once the image is loaded
        document.getElementById("picture-thumb-imageHolder").onload = function () {
            thumbnailImage.close();
        };
        document.getElementById("picture-thumb-modeName").innerText = thumbnailMode;
        document.getElementById("picture-thumb-fileName").innerText = "File used: " + item.name;
        document.getElementById("picture-thumb-requestedSize").innerText = "Requested size: " + requestedSize;
        document.getElementById("picture-thumb-returnedSize").innerText = "Returned size: " + thumbnailImage.originalWidth + "x" + thumbnailImage.originalHeight;
    }

    function cleanOutput() {
        WinJS.log && WinJS.log("", "sample", "status");
        document.getElementById("picture-thumb-imageHolder").src = "/images/placeholder-sdk.png";
        document.getElementById("picture-thumb-modeName").innerText = "";
        document.getElementById("picture-thumb-fileName").innerText = "";
        document.getElementById("picture-thumb-requestedSize").innerText = "";
        document.getElementById("picture-thumb-returnedSize").innerText = "";
    }
})();
