//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario4.html", {
        ready: function (element, options) {
            document.getElementById("folder-thumb-start").addEventListener("click", pickFolder, false);
        }
    });

    function pickFolder() {

        // Set API parameters
        var requestedSize = 200,
            thumbnailMode = Windows.Storage.FileProperties.ThumbnailMode.picturesView;

        // Clean output in case of repeat usage
        cleanOutput();

        var openpicker = new Windows.Storage.Pickers.FolderPicker;
        openpicker.fileTypeFilter.replaceAll([".jpg", ".png", ".bmp", ".gif", ".tif"]);
        openpicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        openpicker.pickSingleFolderAsync().done(function (folder) {
            if (folder) {
                folder.getThumbnailAsync(thumbnailMode, requestedSize).done(
                    function (thumbnail) {
                        if (thumbnail) {
                            outputResult(folder, thumbnail, "ThumbnailMode.picturesView", requestedSize);
                        } else {
                            WinJS.log && WinJS.log(SdkSample.errors.noImages, "sample", "status");
                        }
                    },
                    function (error) {
                        WinJS.log && WinJS.log(SdkSample.errors.fail, "sample", "status");
                    }
                );
            } else {
                WinJS.log && WinJS.log(SdkSample.errors.cancel, "sample", "status");
            }
        });
    }

    function outputResult(item, thumbnailImage, thumbnailMode, requestedSize) {
        document.getElementById("folder-thumb-imageHolder").src = URL.createObjectURL(thumbnailImage, { oneTimeOnly: true });
        // Close the thumbnail stream once the image is loaded
        document.getElementById("folder-thumb-imageHolder").onload = function () {
            thumbnailImage.close();
        };
        document.getElementById("folder-thumb-modeName").innerText = thumbnailMode;
        document.getElementById("folder-thumb-folderName").innerText = "Folder used: " + item.name;
        document.getElementById("folder-thumb-requestedSize").innerText = "Requested size: " + requestedSize;
        document.getElementById("folder-thumb-returnedSize").innerText = "Returned size: " + thumbnailImage.originalWidth + "x" + thumbnailImage.originalHeight;
    }

    function cleanOutput() {
        WinJS.log && WinJS.log("", "sample", "status");
        document.getElementById("folder-thumb-imageHolder").src = "/images/placeholder-sdk.png";
        document.getElementById("folder-thumb-modeName").innerText = "";
        document.getElementById("folder-thumb-folderName").innerText = "";
        document.getElementById("folder-thumb-requestedSize").innerText = "";
        document.getElementById("folder-thumb-returnedSize").innerText = "";
    }
})();
