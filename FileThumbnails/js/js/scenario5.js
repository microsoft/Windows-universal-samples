//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario5.html", {
        ready: function (element, options) {
            document.getElementById("group-thumb-start").addEventListener("click", pickFolder, false);
        }
    });

    function pickFolder() {

        // Set API parameters
        var requestedSize = 200,
            thumbnailMode = Windows.Storage.FileProperties.ThumbnailMode.picturesView;

        // Clean output in case of repeat usage
        cleanOutput();

        var openpicker = new Windows.Storage.Pickers.FolderPicker(),
            monthShape = Windows.Storage.Search.CommonFolderQuery.groupByMonth;
        openpicker.fileTypeFilter.replaceAll([".jpg", ".png", ".bmp", ".gif", ".tif"]);
        openpicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.picturesLibrary;
        openpicker.pickSingleFolderAsync().done(function (folder) {
            if (folder) {
                // Verify the folder is in a library since file groups only work in libraries
                if (folder.isCommonFolderQuerySupported(monthShape)) {
                    // Convert folder to file group and query for items
                    var query = folder.createFolderQuery(monthShape);
                    query.getFoldersAsync().done(function (monthList) {
                        if (monthList && monthList.size > 0) {
                            var firstMonth = monthList.getAt(0);
                            firstMonth.getThumbnailAsync(thumbnailMode, requestedSize).done(
                                function (thumbnail) {
                                    if (thumbnail) {
                                        outputResult(firstMonth, thumbnail, "ThumbnailMode.picturesView", requestedSize, true);
                                    } else {
                                        WinJS.log && WinJS.log(SdkSample.errors.noImages, "sample", "status");
                                    }
                                },
                                function (error) {
                                    WinJS.log && WinJS.log(SdkSample.errors.fail, "sample", "status");
                                }
                            );
                            // Then display the list of files in the file group
                            document.getElementById("group-thumb-outputDetails").innerText = "List of files in the group:";
                            firstMonth.getFilesAsync().done(function (files) {
                                if (files.size > 0) {
                                    files.forEach(function (file) {
                                        outputHeader(file.name);
                                    });
                                }
                            });
                        } else {
                            WinJS.log && WinJS.log(SdkSample.errors.emptyFilegroup, "sample", "status");
                        }
                    });
                } else {
                    WinJS.log && WinJS.log(SdkSample.errors.filegroupLocation, "sample", "status");
                }
            } else {
                WinJS.log && WinJS.log(SdkSample.errors.cancel, "sample", "status");
            }
        });
    }

    function outputHeader(string) {
        var outputElement = document.getElementById("group-thumb-outputDetails");
        var outputGroup = document.createElement("div");

        var headerElement = document.createElement("span");
        headerElement.textContent = string;

        outputGroup.appendChild(headerElement);
        outputElement.appendChild(outputGroup);
    }

    function outputResult(item, thumbnailImage, thumbnailMode, requestedSize) {
        document.getElementById("group-thumb-imageHolder").src = URL.createObjectURL(thumbnailImage, { oneTimeOnly: true });
        // Close the thumbnail stream once the image is loaded
        document.getElementById("group-thumb-imageHolder").onload = function () {
            thumbnailImage.close();
        };
        document.getElementById("group-thumb-modeName").innerText = thumbnailMode;
        document.getElementById("group-thumb-groupName").innerText = "Group used: " + item.name;
        document.getElementById("group-thumb-requestedSize").innerText = "Requested size: " + requestedSize;
        document.getElementById("group-thumb-returnedSize").innerText = "Returned size: " + thumbnailImage.originalWidth + "x" + thumbnailImage.originalHeight;
    }

    function cleanOutput() {
        WinJS.log && WinJS.log("", "sample", "status");
        document.getElementById("group-thumb-imageHolder").src = "/images/placeholder-sdk.png";
        document.getElementById("group-thumb-modeName").innerText = "";
        document.getElementById("group-thumb-groupName").innerText = "";
        document.getElementById("group-thumb-requestedSize").innerText = "";
        document.getElementById("group-thumb-returnedSize").innerText = "";
        document.getElementById("group-thumb-outputDetails").innerText = "";
    }
})();
