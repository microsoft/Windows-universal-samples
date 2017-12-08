//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario11_TryToGetAFileWithoutGettingAnError.html", {
        ready: function (element, options) {
            document.getElementById("getFile").addEventListener("click", getFile, false);
        }
    });

    function getFile() {
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            return picturesLibrary.tryGetItemAsync("sample.dat");
        }).done(function (file) {
            if (file !== null) {
                WinJS.log && WinJS.log("Operation result: " + file.name, "sample", "status");;
            } else {
                WinJS.log && WinJS.log("Operation result: null", "sample", "status");;
            }
        });
    }
})();
