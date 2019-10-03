//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario1_CreateAFileInThePicturesLibrary.html", {
        ready: function (element, options) {
            document.getElementById("createFile").addEventListener("click", createFile, false);
        }
    });

    function createFile() {
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            return picturesLibrary.createFileAsync("sample.dat", Windows.Storage.CreationCollisionOption.replaceExisting);
        }).done(
        function (file) {
            SdkSample.sampleFile = file;
            WinJS.log && WinJS.log("The file '" + SdkSample.sampleFile.name + "' was created.", "sample", "status");
        },
        function (error) {
            WinJS.log && WinJS.log(error, "sample", "error");
        });
    }
})();
