//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;
    var interfaceIdTextBox;
    var getButton;
    var deviceInformationKind;

    var page = WinJS.UI.Pages.define("/html/scenario5_getsingledevice.html", {
        ready: function (element, options) {

            resultsListView = element.querySelector("#resultsListView").winControl;
            interfaceIdTextBox = document.getElementById("interfaceIdTextBox");
            getButton = document.getElementById("getButton");

            // Hook up button event handlers
            getButton.addEventListener("click", getButtonClick, false);

            // Hook up result list data binding
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Capture this in case the user navigates before the findAllAsync completes.
            var informationKindTextBox = document.getElementById("informationKindTextBox");

            // Pre-populate the interface id input box with something valid.
            DevEnum.DeviceInformation.findAllAsync(DevEnum.DeviceClass.audioRender).done(
                function (deviceInfoCollection) {
                    if (deviceInfoCollection.length > 0) {
                        // When you want to "save" a DeviceInformation to get it back again later,
                        // use both the DeviceInformation.Kind and the DeviceInformation.Id.
                        interfaceIdTextBox.value = deviceInfoCollection.getAt(0).id;
                        deviceInformationKind = deviceInfoCollection.getAt(0).kind;
                        informationKindTextBox.value = DisplayHelpers.kindStringFromType(deviceInformationKind);
                        getButton.disabled = false;
                    }
                });

            // Process any data bindings
            WinJS.UI.processAll();
        },
        unload: function () {
            resultCollection.splice(0, resultCollection.length);
        }
    });

    function getButtonClick() {

        getButton.disabled = true;
        resultCollection.splice(0, resultCollection.length);
        
        WinJS.log && WinJS.log("CreateFromIdAsync operation started...", "sample", "status");

        // When you want to "save" a DeviceInformation to get it back again later,
        // use both the DeviceInformation.Kind and the DeviceInformation.Id.
        DevEnum.DeviceInformation.createFromIdAsync(interfaceIdTextBox.value, null, deviceInformationKind).then(
            function (deviceInfo) {
                WinJS.log && WinJS.log("CreateFromIdAsync operation completed.", "sample", "status");
                resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));
            },
            function (e) {
                if (e.number === (0x80070002 | 0)) { // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
                    WinJS.log && WinJS.log("Invalid Interface Id", "sample", "error");
                } else {
                    throw e;
                }
            }).done(function () {
                getButton.disabled = false;
            });
    }

})();
