//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "File access JS sample";
    var sampleFile = null;
    var mruToken = null;
    var falToken = null;

    var scenarios = [
        { url: "/html/scenario1_CreateAFileInThePicturesLibrary.html",             title: "Creating a file" },
        { url: "/html/scenario2_GetTheParentFolderOfAFile.html",                   title: "Getting a file's parent folder" },
        { url: "/html/scenario3_WriteAndReadTextInAFile.html",                     title: "Writing and reading text in a file" },
        { url: "/html/scenario4_WriteAndReadBytesInAFile.html",                    title: "Writing and reading bytes in a file" },
        { url: "/html/scenario5_WriteAndReadAFileUsingAStream.html",               title: "Writing and reading using a stream" },
        { url: "/html/scenario6_DisplayFileProperties.html",                       title: "Displaying file properties" },
        { url: "/html/scenario7_TrackAFileOrFolderSoThatYouCanAccessItLater.html", title: "Persisting a storage item for future use" },
        { url: "/html/scenario8_CopyAFile.html",                                   title: "Copying a file" },
        { url: "/html/scenario9_CompareTwoFilesToSeeIfTheyAreTheSame.html",        title: "Comparing two files to see if they are the same file" },
        { url: "/html/scenario10_DeleteAFile.html",                                title: "Deleting a file" },
        { url: "/html/scenario11_TryToGetAFileWithoutGettingAnError.html",         title: "Attempting to get a file with no error on failure" }
    ];

    function validateFileExistence() {
        Windows.Storage.KnownFolders.getFolderForUserAsync(null /* current user */, Windows.Storage.KnownFolderId.picturesLibrary).then(function (picturesLibrary) {
            return picturesLibrary.tryGetItemAsync("sample.dat");
        }).done(function (item) {
            SdkSample.sampleFile = item;
            if (!item) {
                // If file doesn't exist, indicate users to use scenario 1. 
                WinJS.log && WinJS.log("The file 'sample.dat' does not exist. Use scenario one to create this file.", "sample", "error");
            }
        });
    };

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        validateFileExistence: validateFileExistence,
        sampleFile: sampleFile,
        mruToken: mruToken,
        falToken: falToken
    });
})();