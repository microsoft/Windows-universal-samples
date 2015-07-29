//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/applicationlink.html", {
        ready: function (element, options) {
            var dataTransferManager = Windows.ApplicationModel.DataTransfer.DataTransferManager.getForCurrentView();
            dataTransferManager.addEventListener("datarequested", dataRequested);
            document.getElementById("shareButton").addEventListener("click", showShareUI, false);
            document.getElementById("defaultLogo").addEventListener("change", toggleSliders, false);
            document.getElementById("microsoftLogo").addEventListener("change", toggleSliders, false);
            document.getElementById("visualStudioLogo").addEventListener("change", toggleSliders, false);
            document.getElementById("redSlider").addEventListener("change", updateSliderLabels, false);
            document.getElementById("greenSlider").addEventListener("change", updateSliderLabels, false);
            document.getElementById("blueSlider").addEventListener("change", updateSliderLabels, false);
            document.getElementById("alphaSlider").addEventListener("change", updateSliderLabels, false);
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
            var dataPackageApplicationLink = document.getElementById("selectionList").value;
            if ((typeof dataPackageApplicationLink === "string") && (dataPackageApplicationLink !== "")) {
                request.data.properties.title = dataPackageTitle;

                // The description is optional.
                var dataPackageDescription = document.getElementById("descriptionInputBox").value;
                if ((typeof dataPackageDescription === "string") && (dataPackageDescription !== "")) {
                    request.data.properties.description = dataPackageDescription;
                }

                // Set the Content Source Application Link.
                SdkSample.scenarios.forEach(function (scenario) {
                    if (scenario.url === "/html/applicationlink.html") {
                        request.data.properties.contentSourceApplicationLink = new Windows.Foundation.Uri(scenario.applink);
                    }
                });

                // Place the selected logo and the background color in the data package properties
                if (document.getElementById("microsoftLogo").checked) {
                    var microsoftLogoUri = new Windows.Foundation.Uri("ms-appx:///images/microsoftLogo.png");
                    request.data.properties.square30x30Logo = Windows.Storage.Streams.RandomAccessStreamReference.createFromUri(microsoftLogoUri);
                }
                else if (document.getElementById("visualStudioLogo").checked) {
                    var visualStudioLogoUri = new Windows.Foundation.Uri("ms-appx:///images/visualStudioLogo.png");
                    request.data.properties.square30x30Logo = Windows.Storage.Streams.RandomAccessStreamReference.createFromUri(visualStudioLogoUri);
                }

                if (!document.getElementById("defaultLogo").checked) {
                    var alphaColor = document.getElementById("alphaSlider").valueAsNumber;
                    var redColor = document.getElementById("redSlider").valueAsNumber;
                    var greenColor = document.getElementById("greenSlider").valueAsNumber;
                    var blueColor = document.getElementById("blueSlider").valueAsNumber;
                    request.data.properties.logoBackgroundColor = Windows.UI.ColorHelper.fromArgb(alphaColor, redColor, blueColor, greenColor);
                }

                try {
                    request.data.setApplicationLink(new Windows.Foundation.Uri(dataPackageApplicationLink));
                    WinJS.log && WinJS.log("", "sample", "error");
                } catch (ex) {
                    WinJS.log && WinJS.log("Exception occured: the uri provided " + dataPackageApplicationLink + " is not well formatted.", "sample", "error");
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

    function toggleSliders() {
        var isDefaultLogoSelected = document.getElementById("defaultLogo").checked;
        document.getElementById("redSlider").disabled = isDefaultLogoSelected;
        document.getElementById("greenSlider").disabled = isDefaultLogoSelected;
        document.getElementById("blueSlider").disabled = isDefaultLogoSelected;
        document.getElementById("alphaSlider").disabled = isDefaultLogoSelected;
    }

    function updateSliderLabels() {
        document.getElementById("redSliderCurrentValue").innerText = document.getElementById("redSlider").valueAsNumber;
        document.getElementById("greenSliderCurrentValue").innerText = document.getElementById("greenSlider").valueAsNumber;
        document.getElementById("blueSliderCurrentValue").innerText = document.getElementById("blueSlider").valueAsNumber;
        document.getElementById("alphaSliderCurrentValue").innerText = document.getElementById("alphaSlider").valueAsNumber;
    }
})();
