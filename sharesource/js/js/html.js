//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/html.html", {
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
            var range = document.createRange();
            range.selectNode(document.getElementById("htmlFragment"));
            request.data = MSApp.createDataPackage(range);
            request.data.properties.title = dataPackageTitle;

            // The description is optional.
            var dataPackageDescription = document.getElementById("descriptionInputBox").value;
            if ((typeof dataPackageDescription === "string") && (dataPackageDescription !== "")) {
                request.data.properties.description = dataPackageDescription;
            }

            // Set the Content Source Application Link.
            SdkSample.scenarios.forEach(function (scenario) {
                if (scenario.url === "/html/html.html") {
                    request.data.properties.contentSourceApplicationLink = new Windows.Foundation.Uri(scenario.applink);
                }
            });

            // The HTML fragment we are using has an image tag that references a local file accessible only to this application.
            // To make sure that target application can render this image, we need to populate a resourceMap as part of the share operation data
            // We use the image's relative src property as the key to the resourceMap item we're adding
            var path = document.getElementById("htmlFragmentImage").getAttribute("src");
            var imageUri = new Windows.Foundation.Uri(path);
            var streamReference = Windows.Storage.Streams.RandomAccessStreamReference.createFromUri(imageUri);
            request.data.resourceMap[path] = streamReference;
        } else {
            request.failWithDisplayText(SdkSample.missingTitleError);
        }
    }

    function showShareUI() {
        Windows.ApplicationModel.DataTransfer.DataTransferManager.showShareUI();
    }
})();
