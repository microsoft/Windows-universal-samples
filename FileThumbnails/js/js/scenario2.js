//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("music-thumb-start").addEventListener("click", pickMusic, false);
        }
    });

    function pickMusic() {
        // Set API parameters
        var requestedSize = 100,
            thumbnailMode = Windows.Storage.FileProperties.ThumbnailMode.musicView;

        // Clean output in case of repeat usage
        cleanOutput();

        var openpicker = new Windows.Storage.Pickers.FileOpenPicker();
        openpicker.fileTypeFilter.replaceAll([".mp3", ".wma", ".m4a", ".aac"]);
        openpicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.musicLibrary;
        openpicker.pickSingleFileAsync().done(function (file) {
            if (file) {
                file.getThumbnailAsync(thumbnailMode, requestedSize).done(
                    function (thumbnail) {
                        if (thumbnail) {
                            // Also verify the type is ThumbnailType.image (album art) instead of ThumbnailType.icon
                            // (which may be returned as a fallback if the file does not provide album art)
                            if (thumbnail.type === Windows.Storage.FileProperties.ThumbnailType.image) {
                                outputResult(file, thumbnail, "ThumbnailMode.musicView", requestedSize);
                            } else {
                                WinJS.log && WinJS.log(SdkSample.errors.noAlbumArt, "sample", "status");
                            }
                        } else {
                            WinJS.log && WinJS.log(SdkSample.errors.noAlbumArt, "sample", "status");
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
        document.getElementById("music-thumb-imageHolder").src = URL.createObjectURL(thumbnailImage, { oneTimeOnly: true });
        // Close the thumbnail stream once the image is loaded
        document.getElementById("music-thumb-imageHolder").onload = function () {
            thumbnailImage.close();
        };
        document.getElementById("music-thumb-modeName").innerText = thumbnailMode;
        document.getElementById("music-thumb-fileName").innerText = "File used: " + item.name;
        document.getElementById("music-thumb-requestedSize").innerText = "Requested size: " + requestedSize;
        document.getElementById("music-thumb-returnedSize").innerText = "Returned size: " + thumbnailImage.originalWidth + "x" + thumbnailImage.originalHeight;
    }

    function cleanOutput() {
        WinJS.log && WinJS.log("", "sample", "status");
        document.getElementById("music-thumb-imageHolder").src = "/images/placeholder-sdk.png";
        document.getElementById("music-thumb-modeName").innerText = "";
        document.getElementById("music-thumb-fileName").innerText = "";
        document.getElementById("music-thumb-requestedSize").innerText = "";
        document.getElementById("music-thumb-returnedSize").innerText = "";
    }
})();
