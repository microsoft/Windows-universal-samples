//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;
    var deviceInformationKind;

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
                        // When you want to "save" a DeviceInformation to get it back again later,
                        // use both the DeviceInformation.Kind and the DeviceInformation.Id.
                        interfaceIdTextBox.value = deviceInfoCollection.getAt(0).id;
                        deviceInformationKind = deviceInfoCollection.getAt(0).kind;
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
            // When you want to "save" a DeviceInformation to get it back again later,
            // use both the DeviceInformation.Kind and the DeviceInformation.Id.
            DevEnum.DeviceInformation.createFromIdAsync(interfaceIdTextBox.value, null, deviceInformationKind).done(
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
