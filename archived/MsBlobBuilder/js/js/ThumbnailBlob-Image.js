//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/ThumbnailBlob-Image.html", {
        ready: function (element, options) {
            document.getElementById("capture").addEventListener("click", /*@static_cast(EventListener)*/showThumbnail, false);
            reset();
        },
        
        unload: function () {
            reset();
        }
    });

    function asyncThumbnailError(error) {
        WinJS.log && WinJS.log("Error displaying one or more thumbnails, thumbnails may not be available for these file types.", "sample", "error");
    }

    function showThumbnail() {
        reset();
        var library = Windows.Storage.KnownFolders.picturesLibrary;
        library.getFilesAsync(Windows.Storage.Search.CommonFileQuery.orderByDate, 0, 20).then(function (fileList) {
            if (fileList.size === 0) {
                WinJS.log && WinJS.log("This scenario requires that there is at least one picture in your Pictures Library, please try again after saving a picture to your Pictures Library", "sample", "error");
            } else {
                WinJS.log && WinJS.log("Loading thumbnails:", "sample", "status");
            }
            fileList.forEach(function (file) {
                file.getThumbnailAsync(Windows.Storage.FileProperties.ThumbnailMode.singleItem).done(function (thumbnail) {
                    if (thumbnail) {
                        var imageTag = document.createElement("img");
                        imageTag.className = "gridImages";
                        document.getElementById("imageHolder").appendChild(/*@static_cast(Node)*/imageTag);
                        imageTag.src = URL.createObjectURL(thumbnail, {oneTimeOnly: true});
                    } else {
                        asyncThumbnailError();
                    }
                }, asyncThumbnailError);
            });
        }, BlobSample.asyncError);
    }

    function reset() {
        document.getElementById("imageHolder").innerHTML = "";
    }
})();
