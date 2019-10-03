//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/files.html", {
        ready: function (element, options) {
            document.getElementById("scenario3Copy").addEventListener("click", scenario3Copy, false);
            document.getElementById("scenario3Paste").addEventListener("click", scenario3Paste, false);
        }
    });

    function displayStatus(statusString) {
        return WinJS.log && WinJS.log(statusString, "sample", "status");
    }

    function displayError(errorString) {
        return WinJS.log && WinJS.log(errorString, "sample", "error");
    }

    function scenario3ClearOutput() {
        displayError("");
        displayStatus("");
        document.getElementById("scenario3RequestedOperation").innerText = "";
        document.getElementById("scenario3FileOutput").innerText = "";
    }

    function scenario3Copy() {
        scenario3ClearOutput();

        var picker = new Windows.Storage.Pickers.FileOpenPicker();
        picker.fileTypeFilter.replaceAll(["*"]);
        picker.viewMode = Windows.Storage.Pickers.PickerViewMode.list;
        picker.pickMultipleFilesAsync().done(function (files) {
            if (files.size > 0) {
                var dataPackage = new Windows.ApplicationModel.DataTransfer.DataPackage();

                dataPackage.setStorageItems(files);

                // Request a copy operation from targets that support different file operations, like File Explorer
                dataPackage.requestedOperation = Windows.ApplicationModel.DataTransfer.DataPackageOperation.copy;

                try {
                    // copy the content to Clipboard
                    Windows.ApplicationModel.DataTransfer.Clipboard.setContent(dataPackage);

                    displayStatus("Files have been copied to Clipboard. Try pasting them in another application, or click Paste button above.");
                } catch (e) {
                    // Copying data to Clipboard can potentially fail - for example, if another application is holding Clipboard open
                    displayError("Error copying content to Clipboard: " + e + ". Try again.");
                }
            } else {
                displayStatus("No file selected");
            }
        });
    }

    function scenario3Paste() {
        scenario3ClearOutput();

        // get the content from clipboard
        var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.getContent();

        if (dataPackageView.contains(Windows.ApplicationModel.DataTransfer.StandardDataFormats.storageItems)) {
            dataPackageView.getStorageItemsAsync().done(function (storageItems) {
                // find out what the requested operation is
                switch (dataPackageView.requestedOperation) {
                    case Windows.ApplicationModel.DataTransfer.DataPackageOperation.copy:
                        document.getElementById("scenario3RequestedOperation").innerText = "Requested Operation: Copy";
                        break;

                    case Windows.ApplicationModel.DataTransfer.DataPackageOperation.move:
                        document.getElementById("scenario3RequestedOperation").innerText = "Requested Operation: Move";
                        break;

                    case Windows.ApplicationModel.DataTransfer.DataPackageOperation.link:
                        document.getElementById("scenario3RequestedOperation").innerText = "Requested Operation: Link";
                        break;

                    case Windows.ApplicationModel.DataTransfer.DataPackageOperation.none:
                        document.getElementById("scenario3RequestedOperation").innerText = "Requested Operation: None";
                        break;

                    default:
                        break;
                }

                document.getElementById("scenario3FileOutput").innerHTML = "Following files have been copied from the Clipboard to the app's localFolder: <br />";
                storageItems.forEach(function (file) {
                    if (file.isOfType(Windows.Storage.StorageItemTypes.file)) {
                        file.copyAsync(Windows.Storage.ApplicationData.current.localFolder, file.name, Windows.Storage.NameCollisionOption.replaceExisting).done(function (newFile) {
                            document.getElementById("scenario3FileOutput").innerHTML += file.name + "<br />";
                        }, function (e) {
                            displayError("Error copying file: " + e);
                        });
                    } else {
                        // It's a folder - skipping folders for brevity sake
                        document.getElementById("scenario3FileOutput").innerHTML += file.path + " is a folder, skipping <br />";
                    }
                });
            }, function (e) {
                displayError("Error retrieving files from Clipboard: " + e);
            });
        } else {
            displayStatus("StorageItems format is not available in clipboard");
        }
    }
})();
