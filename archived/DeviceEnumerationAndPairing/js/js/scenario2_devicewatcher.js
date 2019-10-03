//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    
    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;
    var deviceWatcherHelper = new SdkSample.DeviceWatcherHelper(resultCollection);

    var page = WinJS.UI.Pages.define("/html/scenario2_devicewatcher.html", {
        ready: function (element, options) {

            // Hook up button event handlers
            document.getElementById("startWatcherButton").addEventListener("click", startWatcher, false);
            document.getElementById("stopWatcherButton").addEventListener("click", stopWatcher, false);

            // Hook up result list data binding
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Manually bind selector options
            DisplayHelpers.deviceWatcherSelectors.forEach(function each(item) {
                var option = document.createElement("option");
                option.textContent = item.displayName;
                selectorComboBox.appendChild(option);
            });

            // Process any data bindings
            WinJS.UI.processAll();
        },
        unload: function () {
            deviceWatcherHelper.reset();
            resultCollection.splice(0, resultCollection.length);
        }
    });

    function startWatcher() {
        startWatcherButton.disabled = true;
        resultCollection.splice(0, resultCollection.length);

        // First get the device selector chosen by the UI.
        var selectedItem = DisplayHelpers.deviceWatcherSelectors.getAt(selectorComboBox.selectedIndex);

        var deviceWatcher;
        if (undefined !== selectedItem.deviceClassSelector) {
            // If the a pre-canned device class selector was chosen, call the DeviceClass overload
            deviceWatcher = DevEnum.DeviceInformation.createWatcher(selectedItem.deviceClassSelector);
        }
        else if (selectedItem.kind === DevEnum.DeviceInformationKind.unknown) {
            // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
            // Kind will be determined by the selector
            deviceWatcher = DevEnum.DeviceInformation.createWatcher(
                selectedItem.selector,
                null // don't request additional properties for this sample
                );
        }
        else {
            // Kind is specified in the selector info
            deviceWatcher = DevEnum.DeviceInformation.createWatcher(
                selectedItem.selector,
                null, // don't request additional properties for this sample
                selectedItem.kind);
        }

        WinJS.log && WinJS.log("Starting watcher...", "sample", "status");
        deviceWatcherHelper.startWatcher(deviceWatcher);
        stopWatcherButton.disabled = false;
    }

    function stopWatcher() {
        stopWatcherButton.disabled = true;
        deviceWatcherHelper.stopWatcher();
        startWatcherButton.disabled = false;
    }
})();
