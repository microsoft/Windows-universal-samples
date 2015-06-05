//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;

    var page = WinJS.UI.Pages.define("/html/scenario5_getsingledevice.html", {
        ready: function (element, options) {

            // Setup beforeNavigate event
            WinJS.Navigation.addEventListener("beforenavigate", onLeavingPage);

            // Hook up button event handlers
            document.getElementById("getButton").addEventListener("click", getButtonClick, false);

            // Hook up result list data binding
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Pre-populate the interface id input box with something valid.
            DevEnum.DeviceInformation.findAllAsync(DevEnum.DeviceClass.audioRender).done(
                function (deviceInfoCollection) {
                    if (deviceInfoCollection.length > 0) {
                        interfaceIdTextBox.value = deviceInfoCollection.getAt(0).id;
                    }
                });

            // Process any data bindings
            WinJS.UI.processAll();
        }
    });

    function onLeavingPage() {
        WinJS.Navigation.removeEventListener("beforenavigate", onLeavingPage);
        resultCollection.splice(0, resultCollection.length);
    }

    function getButtonClick() {

        resultCollection.splice(0, resultCollection.length);
        
        WinJS.log && WinJS.log("CreateFromIdAsync operation started...", "sample", "status");

        try {
            DevEnum.DeviceInformation.createFromIdAsync(interfaceIdTextBox.value).done(
                function (deviceInfo) {

                    WinJS.log && WinJS.log("CreateFromIdAsync operation completed.", "sample", "status");
                    resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));
                },
                function (e) {
                    WinJS.log && WinJS.log("CreateFromIdAsync operation failed. error = " + e.message, "sample", "error");
                });
        }
        catch (e) {
            WinJS.log && WinJS.log("CreateFromIdAsync call failed. error = " + e.message, "sample", "error");
        }
    }

})();
