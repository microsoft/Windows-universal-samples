//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/deleteWithAppContent.html", {
        ready: function (element, options) {
            document.getElementById("addToIndex").addEventListener("click", IndexerHelpers.addAppContentFilesToIndexedFolder, false);
            document.getElementById("deleteSingleItem").addEventListener("click", deleteSingleItem, false);
            document.getElementById("deleteAllItems").addEventListener("click", deleteAllItems, false);
        }
    });

    // This function deletes a single appcontent-ms file, "sample1.appcontent-ms".  Upon deletion,
    // the indexer is notified and the content indexed from this file is removed from the index.
    function deleteSingleItem() {
        var applicationData = Windows.Storage.ApplicationData.current,
            localFolder = applicationData.localFolder,
            indexedFolder,
            installDirectory = Windows.ApplicationModel.Package.current.installedLocation;
        var output = "Items deleted from the \"Indexed\" folder:\n";
        localFolder.createFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.openIfExists).then(function (retrievedIndexedFolder) {
            indexedFolder = retrievedIndexedFolder;
            return indexedFolder.getFileAsync("sample1.appcontent-ms");
        }).then(function (file) {
            output += file.displayName;
            return file.deleteAsync();
        }).done(function () {
            WinJS.log && WinJS.log(output, "sample", "status");
        }, function (err) {
            var errorString = err.toString() + "Use the \"Add items to index\" button and then try again.";
            WinJS.log && WinJS.log(errorString, "sample", "error");
        });
    }

    // This function deletes all appcontent-ms files in the "LocalState\Indexed" folder.  Upon deletion,
    // the indexer is notified and the content indexed from these files is removed from the index.
    function deleteAllItems() {
        var applicationData = Windows.Storage.ApplicationData.current,
            localFolder = applicationData.localFolder,
            indexedFolder,
            installDirectory = Windows.ApplicationModel.Package.current.installedLocation,
            itemsDeleted;
        var output = "Items deleted from the \"Indexed\" folder:\n";
        localFolder.createFolderAsync("Indexed", Windows.Storage.CreationCollisionOption.openIfExists).then(function (retrievedIndexedFolder) {
            indexedFolder = retrievedIndexedFolder;
            return indexedFolder.getFilesAsync();
        }).then(function (files) {
            var promiseArray = [];
            if (files) {
                var len = files.length;
                for (var i = 0; i < len; i++) {
                    output += files[i].displayName + files[i].fileType;
                    if (i < len - 1) {
                        output += "\n";
                    }
                    promiseArray[i] = files[i].deleteAsync();
                }
                if (len > 0) {
                    itemsDeleted = true;
                } else {
                    itemsDeleted = false;
                }
            }
            return WinJS.Promise.join(promiseArray);
        }).done(function () {
            if (itemsDeleted) {
                WinJS.log && WinJS.log(output, "sample", "status");
            } else {
                WinJS.log && WinJS.log("No files have been added to the \"Indexed\" folder. Use the \"Add items to index\" button and then try again.", "sample", "error");
            }
        });
    }
})();