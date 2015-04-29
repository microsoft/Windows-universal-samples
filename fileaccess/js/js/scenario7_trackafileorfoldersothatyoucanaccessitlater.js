//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario7_TrackAFileOrFolderSoThatYouCanAccessItLater.html", {
        ready: function (element, options) {
            document.getElementById("addToList").addEventListener("click", addToList, false);
            document.getElementById("showList").addEventListener("click", showList, false);
            document.getElementById("openFromList").addEventListener("click", openFromList, false);
            SdkSample.validateFileExistence();
        }
    });

    function addToList() {
        if (SdkSample.sampleFile !== null) {
            var MRUradio = document.getElementById("MRUradio");
            var systemMRUcheckbox = document.getElementById("systemMRUcheckbox");
            var FALradio = document.getElementById("FALradio");
            if (MRUradio.checked) {
                // Add the file to app and possibly system MRU
                var visibility = systemMRUcheckbox.checked ?
                    Windows.Storage.AccessCache.RecentStorageItemVisibility.appAndSystem :
                    Windows.Storage.AccessCache.RecentStorageItemVisibility.appOnly;
                SdkSample.mruToken = Windows.Storage.AccessCache.StorageApplicationPermissions.mostRecentlyUsedList.add(SdkSample.sampleFile, SdkSample.sampleFile.name,
                    visibility);
                WinJS.log && WinJS.log("The file '" + SdkSample.sampleFile.name + "' was added to the MRU list and a token was stored.", "sample", "status");
            } else if (FALradio.checked) {
                SdkSample.falToken = Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList.add(SdkSample.sampleFile, SdkSample.sampleFile.name);
                WinJS.log && WinJS.log("The file '" + SdkSample.sampleFile.name + "' was added to the FAL list and a token was stored.", "sample", "status");
            }
        }
    }

    function showList() {
        if (SdkSample.sampleFile !== null) {
            var MRUradio = document.getElementById("MRUradio");
            var FALradio = document.getElementById("FALradio");
            if (MRUradio.checked) {
                var mruEntries = Windows.Storage.AccessCache.StorageApplicationPermissions.mostRecentlyUsedList.entries;
                if (mruEntries.size > 0) {
                    var mruOutputText = "The MRU list contains the following item(s):";
                    mruEntries.forEach(function (entry) {
                        mruOutputText += "\n" + entry.metadata; // Application previously chose to store sampleFile.name in this field
                    });

                    WinJS.log && WinJS.log(mruOutputText, "sample", "status");
                } else {
                    WinJS.log && WinJS.log("The MRU list is empty, please select 'Most Recently Used' and click 'Add to List' to add a file to the MRU list.", "sample", "error");;
                }
            } else if (FALradio.checked) {
                var falEntries = Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList.entries;
                if (falEntries.size > 0) {
                    var falOutputText = "The FAL list contains the following item(s):";
                    falEntries.forEach(function (entry) {
                        falOutputText += "\n" + entry.metadata; // Application previously chose to store sampleFile.name in this field
                    });

                    WinJS.log && WinJS.log(falOutputText, "sample", "status");
                } else {
                    WinJS.log && WinJS.log("The FAL list is empty, please select 'Future Access List' and click 'Add to List' to add a file to the FAL list.", "sample", "error");
                }
            }
        }
    }

    function openFromList() {
        if (SdkSample.sampleFile !== null) {
            var MRUradio = document.getElementById("MRUradio");
            var FALradio = document.getElementById("FALradio");
            if (MRUradio.checked) {
                if (SdkSample.mruToken !== null) {
                    // Open the 'sample.dat' via the token that was stored when adding this file into the MRU list
                    Windows.Storage.AccessCache.StorageApplicationPermissions.mostRecentlyUsedList.getFileAsync(SdkSample.mruToken).then(function (file) {
                        // Read the file
                        Windows.Storage.FileIO.readTextAsync(file).done(function (fileContent) {
                            WinJS.log && WinJS.log("The file '" + file.name + "' was opened by a stored token from the MRU list, it contains the following text:\n" + fileContent, "sample", "status");
                        },
                        function (error) {
                            WinJS.log && WinJS.log(error, "sample", "error");
                        });
                    },
                    function (error) {
                        WinJS.log && WinJS.log(error, "sample", "error");
                    });
                } else {
                    WinJS.log && WinJS.log("The MRU list is empty, please select 'Most Recently Used' list and click 'Add to List' to add a file to the MRU list.", "sample", "error");
                }
            } else if (FALradio.checked) {
                if (SdkSample.falToken !== null) {
                    // Open the 'sample.dat' via the token that was stored when adding this file into the FAL list
                    Windows.Storage.AccessCache.StorageApplicationPermissions.futureAccessList.getFileAsync(SdkSample.falToken).then(function (file) {
                        // Read the file
                        Windows.Storage.FileIO.readTextAsync(file).done(function (fileContent) {
                            WinJS.log && WinJS.log("The file '" + file.name + "' was opened by a stored token from the FAL list, it contains the following text:\n" + fileContent, "sample", "status");
                        },
                        function (error) {
                            WinJS.log && WinJS.log(error, "sample", "error");
                        });
                    },
                    function (error) {
                        WinJS.log && WinJS.log(error, "sample", "error");
                    });
                } else {
                    WinJS.log && WinJS.log("The FAL list is empty, please select 'Future Access List' list and click 'Add to List' to add a file to the FAL list.", "sample", "error");
                }
            }
        }
    }
})();
