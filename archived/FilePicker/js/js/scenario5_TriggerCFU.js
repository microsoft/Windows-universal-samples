//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Elements
    var writeToFileButton;
    var writeToFileWithCFUButton;
    var saveToFutureAccessListButton;
    var getFileFromFutureAccessListButton;

    // Scenario state variables
    var beforeReadFile;
    var afterWriteFile;
    var faToken;

    var page = WinJS.UI.Pages.define("/html/scenario5_TriggerCFU.html", {
        ready: function (element, options) {
            writeToFileButton = document.getElementById("WriteToFileButton");
            writeToFileWithCFUButton = document.getElementById("WriteToFileWithCFUButton");
            saveToFutureAccessListButton = document.getElementById("SaveToFutureAccessListButton");
            getFileFromFutureAccessListButton = document.getElementById("GetFileFromFutureAccessListButton");

            document.getElementById("CreateFileButton").addEventListener("click", createFile);
            writeToFileButton.addEventListener("click", writeToFile);
            writeToFileWithCFUButton.addEventListener("click", writeToFileWithExplicitCFU);
            document.getElementById("PickAFileButton").addEventListener("click", pickAFile);
            saveToFutureAccessListButton.addEventListener("click", saveToFutureAccessList);
            getFileFromFutureAccessListButton.addEventListener("click", getFileFromFutureAccessList);

            updateButtons();
        }
    });

    function updateButtons() {
        writeToFileButton.disabled = !afterWriteFile;
        writeToFileWithCFUButton.disabled = !afterWriteFile;

        saveToFutureAccessListButton.disabled = !beforeReadFile;
        getFileFromFutureAccessListButton.disabled = !faToken;
    }

    function createFile() {
        WinJS.log && WinJS.log("", "sample", "status");

        // Create the picker object and set options
        var savePicker = new Windows.Storage.Pickers.FileSavePicker();
        savePicker.suggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.documentsLibrary;
        // Dropdown of file types the user can save the file as
        savePicker.fileTypeChoices.insert("Plain Text", [".txt"]);
        // Default file name if the user does not type one in or select a file to replace
        savePicker.suggestedFileName = "New Document";

        savePicker.pickSaveFileAsync().done(function (file) {
            afterWriteFile = file;
            if (afterWriteFile) {
                WinJS.log && WinJS.log("File created.", "sample", "status");
            } else {
                WinJS.log && WinJS.log("Operation cancelled.", "sample", "status");
            }
            updateButtons();
        });
    }

    function writeToFile() {
        Windows.Storage.FileIO.writeTextAsync(afterWriteFile, "The File Picker App just wrote to the file!").done(function () {
            WinJS.log && WinJS.log("File write complete. If applicable, a WriteActivationMode.AfterWrite activation will occur in approximately 60 seconds on desktop.", "sample", "status");
        });
    }

    function writeToFileWithExplicitCFU() {
        var file = afterWriteFile;
        Windows.Storage.CachedFileManager.deferUpdates(file);
        Windows.Storage.FileIO.writeTextAsync(file, "The File Picker App just wrote to the file!").then(function () {
            WinJS.log && WinJS.log("File write complete. Explicitly completing updates.", "sample", "status");
            return Windows.Storage.CachedFileManager.completeUpdatesAsync(file);
        }).done(function (status) {
            switch (status) {
                case Windows.Storage.Provider.FileUpdateStatus.complete:
                    WinJS.log && WinJS.log(`File ${file.name} was saved.`, "sample", "status");
                    break;

                case Windows.Storage.Provider.FileUpdateStatus.completeAndRenamed:
                    WinJS.log && WinJS.log(`File ${file.name} was renamed and saved.`, "sample", "status");
                    break;

                default:
                    WinJS.log && WinJS.log(`File ${file.name} couldn't be saved.`, "sample", "error");
                    break;
            }
        });
    }

    function pickAFile() {
        WinJS.log && WinJS.log("", "sample", "status");

        var openPicker = new Windows.Storage.Pickers.FileOpenPicker();
        openPicker.viewMode = Windows.Storage.Pickers.PickerViewMode.thumbnail;
        openPicker.fileTypeFilter.push(".txt");

        openPicker.pickSingleFileAsync().done(function (file) {
            beforeReadFile = file;
            if (file) {
                WinJS.log && WinJS.log(`Picked file: ${file.name}`, "sample", "status");
            } else {
                WinJS.log && WinJS.log("Operation cancelled.", "sample", "error");
            }
            updateButtons();
        });
    }

    function saveToFutureAccessList() {
        faToken = Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList.add(beforeReadFile);
        WinJS.log && WinJS.log("Saved to Future Access List", "sample", "status");
        beforeReadFile = null;
        updateButtons();
    }

    function getFileFromFutureAccessList() {
        WinJS.log && WinJS.log("Getting the file from the Future Access List. If applicable, a ReadActivationMode.BeforeAccess activation will occur in approximately 60 seconds on desktop.", "sample", "status");
        Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList.getFileAsync(faToken).done(function (file) {
            if (file) {
                WinJS.log && WinJS.log(`Retrieved file from Future Access List: ${file.name}`, "sample", "status");
                beforeReadFile = file;
            } else {
                WinJS.log && WinJS.log("Unable to retrieve file from Future Access List.", "sample", "error");
            }
            updateButtons();
        });
    }
})();
