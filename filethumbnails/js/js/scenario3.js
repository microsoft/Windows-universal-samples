//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("document-thumb-start").addEventListener("click", pickDocument, false);
        }
    });

    function pickDocument() {
        // Set API parameters
        var requestedSize = 100,
            thumbnailMode = Windows.Storage.FileProperties.ThumbnailMode.documentsView;

        // Clean output in case of repeat usage
        cleanOutput();

        var openpicker = new Windows.Storage.Pickers.FileOpenPicker();
        openpicker.fileTypeFilter.replaceAll([".doc", ".xls", ".ppt", ".docx", ".xlsx", ".pptx", ".pdf", ".txt", ".rtf"]);
        openpicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.documentsLibrary;
        openpicker.pickSingleFileAsync().done(function (file) {
            if (file) {
                file.getThumbnailAsync(thumbnailMode, requestedSize).done(
                    function (thumbnail) {
                        if (thumbnail) {
                            outputResult(file, thumbnail, "ThumbnailMode.documentsView", requestedSize);
                        } else {
                            WinJS.log && WinJS.log(SdkSample.errors.noIcon, "sample", "status");
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
        document.getElementById("document-thumb-imageHolder").src = URL.createObjectURL(thumbnailImage, { oneTimeOnly: true });
        // Close the thumbnail stream once the image is loaded
        document.getElementById("document-thumb-imageHolder").onload = function () {
            thumbnailImage.close();
        };
        document.getElementById("document-thumb-modeName").innerText = thumbnailMode;
        document.getElementById("document-thumb-fileName").innerText = "File used: " + item.name;
        document.getElementById("document-thumb-requestedSize").innerText = "Requested size: " + requestedSize;
        document.getElementById("document-thumb-returnedSize").innerText = "Returned size: " + thumbnailImage.originalWidth + "x" + thumbnailImage.originalHeight;
    }

    function cleanOutput() {
        WinJS.log && WinJS.log("", "sample", "status");
        document.getElementById("document-thumb-imageHolder").src = "/images/placeholder-sdk.png";
        document.getElementById("document-thumb-modeName").innerText = "";
        document.getElementById("document-thumb-fileName").innerText = "";
        document.getElementById("document-thumb-requestedSize").innerText = "";
        document.getElementById("document-thumb-returnedSize").innerText = "";
    }
})();
