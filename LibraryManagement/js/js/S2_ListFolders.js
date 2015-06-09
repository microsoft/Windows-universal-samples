//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/S2_ListFolders.html", {
        ready: function (element, options) {
            document.getElementById("listFolders").addEventListener("click", listFolders, false);
        },
        unload: function () {
            if (picturesLibrary) {
                picturesLibrary.removeEventListener("definitionchanged", updateListAndHeader);
            }
        }
    });

    var picturesLibrary = null;

    // Obtains the list of folders that make up the Pictures library and displays them.
    function listFolders() {
        document.getElementById("listFolders").disabled = true;
        Windows.Storage.StorageLibrary.getLibraryAsync(Windows.Storage.KnownLibraryId.pictures).then(function (library) {
            picturesLibrary = library;

            // Register for the definitionchanged event to be notified if other apps modify the library
            picturesLibrary.addEventListener("definitionchanged", updateListAndHeader);
            updateListAndHeader();
        });
    }

    // Displays the list of folders that make up the library.
    function fillList() {
        var list = document.getElementById("foldersList");
        list.innerHTML = "";
        picturesLibrary.folders.forEach(function (folder) {
            var option = document.createElement("p");
            option.innerText = folder.displayName;
            list.appendChild(option);
        });
    }

    // Updates the foldersListHeader with the count of folders in the Pictures library.
    function updateListAndHeader() {
        fillList();
        document.getElementById("foldersListHeader").innerText = "Pictures library (" + picturesLibrary.folders.length + " folders)";
    }
})();
