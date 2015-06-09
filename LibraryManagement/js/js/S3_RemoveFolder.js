//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/S3_RemoveFolder.html", {
        ready: function (element, options) {
            document.getElementById("removeFolder").addEventListener("click", removeFolder, false);
            getPicturesLibrary();
        },
        unload: function () {
            if (picturesLibrary) {
                picturesLibrary.removeEventListener("definitionchanged", updateControls);
            }
        }
    });

    var picturesLibrary;

    // Requests the user's permission to remove the selected location from the Pictures library
    function removeFolder() {
        var folderToRemove = picturesLibrary.folders[document.getElementById("foldersSelect").selectedIndex];
        picturesLibrary.requestRemoveFolderAsync(folderToRemove).done(function (folderRemoved) {
            var outputDiv = document.getElementById("output");
            if (folderRemoved) {
                outputDiv.innerHTML = folderToRemove.displayName + " was removed from the Pictures library.";
            } else {
                outputDiv.innerHTML = "Operation canceled.";
            }
        });
    }

    // Gets the Pictures library and sets up the select control with the list of its folders
    function getPicturesLibrary() {
        Windows.Storage.StorageLibrary.getLibraryAsync(Windows.Storage.KnownLibraryId.pictures).done(function (library) {
            picturesLibrary = library;
            picturesLibrary.addEventListener("definitionchanged", updateControls);

            updateControls();
        });
    }

    // Fills the select control with the folders that make up the Pictures library
    function fillSelect() {
        var select = document.getElementById("foldersSelect");
        select.options.length = 0;
        picturesLibrary.folders.forEach(function (folder) {
            var option = document.createElement("option");
            option.textContent = folder.displayName;
            select.appendChild(option);
        });
    }

    // Updates the visibility and disabled state of the controls and display elements that depend
    // upon the Pictures library having at least one folder in its Folders list.
    function updateControls() {
        fillSelect();
        var libraryHasFolders = (picturesLibrary.folders.size > 0);
        document.getElementById("foldersSelect").selectedIndex = libraryHasFolders ? 0 : -1;
        document.getElementById("foldersSelect").className = libraryHasFolders ? "visible" : "hidden";
        document.getElementById("libraryEmptyMessage").className = libraryHasFolders ? "hidden" : "visible";
        document.getElementById("removeFolder").disabled = !libraryHasFolders;
    }
})();
