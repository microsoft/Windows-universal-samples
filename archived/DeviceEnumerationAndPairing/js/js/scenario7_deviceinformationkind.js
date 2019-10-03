//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;
    var deviceWatcherHelper = new SdkSample.DeviceWatcherHelper(resultCollection);

    var page = WinJS.UI.Pages.define("/html/scenario7_deviceinformationkind.html", {
        ready: function (element, options) {

            // Hook up button event handlers
            document.getElementById("startWatcherButton").addEventListener("click", startWatcher, false);
            document.getElementById("stopWatcherButton").addEventListener("click", stopWatcher, false);

            // Hook up result list data binding
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Manually bind selector options
            DisplayHelpers.kindList.forEach(function each(item) {
                var option = document.createElement("option");
                option.textContent = item.displayName;
                kindComboBox.appendChild(option);
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

        var selectedItem = DisplayHelpers.kindList.getAt(kindComboBox.selectedIndex);

        var deviceWatcher = DevEnum.DeviceInformation.createWatcher(
            "", // No filter
            null, // No requested properties
            selectedItem.deviceInformationKind);

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
