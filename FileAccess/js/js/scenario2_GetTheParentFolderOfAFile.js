//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario2_GetTheParentFolderOfAFile.html", {
        ready: function (element, options) {
            document.getElementById("getParent").addEventListener("click", getParent, false);
            SdkSample.validateFileExistence();
        }
    });

    function getParent() {
        if (SdkSample.sampleFile !== null) {
            SdkSample.sampleFile.getParentAsync().done(function (parentFolder) {
                var outputText = "Item: " + SdkSample.sampleFile.name + " (" + SdkSample.sampleFile.path + ")";
                outputText += "\nParent: " + parentFolder.name + " (" + parentFolder.path + ")";
                WinJS.log && WinJS.log(outputText, "sample", "status");
            },
            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            });
        }
    }
})();
