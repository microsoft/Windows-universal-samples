//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var FA_E_MAX_PERSISTED_ITEMS_REACHED = 0x80270220 | 0;
    var E_INVALIDARG = 0x80070057 | 0;

    var StorageApplicationPermissions = Windows.Storage.AccessCache.StorageApplicationPermissions;

    var page = WinJS.UI.Pages.define("/html/scenario7_TrackAFileOrFolderSoThatYouCanAccessItLater.html", {
        ready: function (element, options) {
            document.getElementById("addToList").addEventListener("click", addToList, false);
            document.getElementById("showList").addEventListener("click", showList, false);
            document.getElementById("openFromList").addEventListener("click", openFromList, false);
        }
    });

    function addToList() {
        if (SdkSample.sampleFile !== null) {
            var MRUradio = document.getElementById("MRUradio");
            var systemMRUcheckbox = document.getElementById("systemMRUcheckbox");
            if (MRUradio.checked) {
                // Add the file to app and possibly system MRU
                var visibility = systemMRUcheckbox.checked ?
                    Windows.Storage.AccessCache.RecentStorageItemVisibility.appAndSystem :
                    Windows.Storage.AccessCache.RecentStorageItemVisibility.appOnly;
                SdkSample.mruToken = StorageApplicationPermissions.mostRecentlyUsedList.add(SdkSample.sampleFile, SdkSample.sampleFile.name,
                    visibility);
                WinJS.log && WinJS.log("The file '" + SdkSample.sampleFile.name + "' was added to the MRU list and a token was stored.", "sample", "status");
            } else {
                try { 
                    SdkSample.falToken = StorageApplicationPermissions.futureAccessList.add(SdkSample.sampleFile, SdkSample.sampleFile.name);
                    WinJS.log && WinJS.log("The file '" + SdkSample.sampleFile.name + "' was added to the FAL list and a token was stored.", "sample", "status");
                } catch (error) {
                    if (error.number == FA_E_MAX_PERSISTED_ITEMS_REACHED) {
                        // A real program would call remove() to create room in the FAL.
                        WinJS.log && WinJS.log("The file '" + SdkSample.sampleFile.name + "' was not added to the FAL list because the FAL list is full.", "sample", "error");
                    } else {
                        throw error;
                    }
                }
            }
        } else {
            SdkSample.validateFileExistence();
        }
    }

    function showList() {
        var entries;
        var listName;
        var MRUradio = document.getElementById("MRUradio");
        if (MRUradio.checked) {
            listName = "MRU";
            entries = StorageApplicationPermissions.mostRecentlyUsedList.entries;
        } else {
            listName = "FAL";
            entries = StorageApplicationPermissions.futureAccessList.entries;
        }

        if (entries.size > 0) {
            var outputText = "The " + listName + " list contains the following item(s):";
            entries.forEach(function (entry) {
                outputText += "\n" + entry.metadata; // Application previously chose to store sampleFile.name in this field
            });

            WinJS.log && WinJS.log(outputText, "sample", "status");
        } else {
            WinJS.log && WinJS.log("The " + listName + " list is empty.", "sample", "error");
        }
    }

    function openFromList() {
        var fileTask = WinJS.Promise.wrap();
        var MRUradio = document.getElementById("MRUradio");
        if (MRUradio.checked) {
            if (SdkSample.mruToken !== null) {
                // When the MRU becomes full, older entries are automatically deleted, so check if the
                // token is still valid before using it.
                if (StorageApplicationPermissions.mostRecentlyUsedList.containsItem(SdkSample.mruToken)) {
                    // Open the 'sample.dat' via the token that was stored when adding this file into the MRU list.
                    fileTask = StorageApplicationPermissions.mostRecentlyUsedList.getFileAsync(SdkSample.mruToken);
                } else {
                    // When the MRU becomes full, older entries are automatically deleted.
                    WinJS.log && WinJS.log("The token is no longer valid.", "sample", "error");
                }
            } else {
                WinJS.log && WinJS.log("This operation requires a token. Add file to the MRU list first.", "sample", "error");
            }
        } else {
            if (SdkSample.falToken !== null) {
                // Open the 'sample.dat' via the token that was stored when adding this file into the FAL list.
                // The token remains valid until we explicitly remove it.
                fileTask = StorageApplicationPermissions.futureAccessList.getFileAsync(SdkSample.falToken);
            } else {
                WinJS.log && WinJS.log("This operation requires a token. Add file to the FAL list first.", "sample", "error");
            }
        }

        fileTask.done(function (file) {
            if (file) {
                // Read the file
                Windows.Storage.FileIO.readTextAsync(file).done(function (fileContent) {
                    WinJS.log && WinJS.log("The file '" + file.name + "' was opened by a stored token. It contains the following text:\n" + fileContent, "sample", "status");
                },
                function (error) {
                    WinJS.log && WinJS.log(error, "sample", "error");
                });
            }
        });
    }
})();
