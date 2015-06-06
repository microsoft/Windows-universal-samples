//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/weblink.html", {
        ready: function (element, options) {
            var dataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView();
            dataTransferManager.addEventListener("datarequested", dataRequested);
            document.getElementById("shareButton").addEventListener("click", showShareUI, false);
        },
        unload: function () {
            var dataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView();
            dataTransferManager.removeEventListener("datarequested", dataRequested);
        }
    });

    function dataRequested(e) {
        var request = e.request;

        // Title is required
        var dataPackageTitle = document.getElementById("titleInputBox").value;
        if ((typeof dataPackageTitle === "string") && (dataPackageTitle !== "")) {
            var dataPackageWebLink = document.getElementById("weblinkInputBox").value;
            if ((typeof dataPackageWebLink === "string") && (dataPackageWebLink !== "")) {
                request.data.properties.title = dataPackageTitle;

                // The description is optional.
                var dataPackageDescription = document.getElementById("descriptionInputBox").value;
                if ((typeof dataPackageDescription === "string") && (dataPackageDescription !== "")) {
                    request.data.properties.description = dataPackageDescription;
                }

                // Set the Content Source Application Link.
                SdkSample.scenarios.forEach(function (scenario) {
                    if (scenario.url === "/html/weblink.html") {
                        request.data.properties.contentSourceApplicationLink = new Windows.Foundation.Uri(scenario.applink);
                    }
                });

                try {
                    request.data.setWebLink(new Windows.Foundation.Uri(document.getElementById("weblinkInputBox").value));
                    WinJS.log && WinJS.log("", "sample", "error");
                } catch (ex) {
                    WinJS.log && WinJS.log("Exception occured: the uri provided " + dataPackageWebLink + " is not well formatted.", "sample", "error");
                }
            } else {
                request.failWithDisplayText("Enter the text you would like to share and try again.");
            }
        } else {
            request.failWithDisplayText(SdkSample.missingTitleError);
        }
    }

    function showShareUI() {
        Windows.ApplicationModel.DataTransfer.DataTransferManager.showShareUI();
    }
})();
