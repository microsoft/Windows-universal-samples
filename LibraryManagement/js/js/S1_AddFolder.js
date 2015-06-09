//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/S1_AddFolder.html", {
        ready: function (element, options) {
            document.getElementById("addFolder").addEventListener("click", addFolder, false);
        }
    });

    // Displays the folder Picker to request that the user select a folder that will be added to the
    // Pictures library.
    function addFolder() {
        Windows.Storage.StorageLibrary.getLibraryAsync(Windows.Storage.KnownLibraryId.pictures).then(function (library) {
            return library.requestAddFolderAsync();
        }).done(function (folderAdded) {
            var outputDiv = document.getElementById("output");
            if (folderAdded !== null) {
                outputDiv.innerHTML = folderAdded.displayName + " was added to the Pictures library.";
            } else {
                outputDiv.innerHTML = "Operation canceled.";
            }
        });
    }
})();
