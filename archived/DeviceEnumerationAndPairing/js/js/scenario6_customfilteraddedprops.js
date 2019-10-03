//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;
    var deviceWatcherHelper = new SdkSample.DeviceWatcherHelper(resultCollection);

    var page = WinJS.UI.Pages.define("/html/scenario6_customfilteraddedprops.html", {
        ready: function (element, options) {

            // Hook up button event handlers
            document.getElementById("startWatcherButton").addEventListener("click", startWatcher, false);
            document.getElementById("stopWatcherButton").addEventListener("click", stopWatcher, false);

            // Hook up result list data binding
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.itemDataSource = resultCollection.dataSource;

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

        var requestedProperties = [
            "System.Devices.InterfaceClassGuid",
            "System.ItemNameDisplay" // This is the property that's behind DeviceInformation.name
        ];

        var deviceWatcher = DevEnum.DeviceInformation.createWatcher(
            aqsFilterTextBox.value,
            requestedProperties
            );

        WinJS.log && WinJS.log("Starting watcher...", "sample", "status");
        deviceWatcherHelper.startWatcher(deviceWatcher);
        stopWatcherButton.disabled = false;
    }

    function stopWatcher() {
        stopWatcherButton.disabled = true;
        deviceWatcherHelper.stopWatcher();
        startWatcherButton.disabled = false;
    }

    //xx
    function onAdded(deviceInfo) {

        // For simplicity, just creating a new "display object" on the fly since databinding directly with deviceInfo from
        // the callback doesn't work. 
        resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));

        var watcher = deviceInfo.target;
        if (watcher.status == DevEnum.DeviceWatcherStatus.enumerationCompleted) {
            WinJS.log && WinJS.log(resultCollection.length + " devices found. Watching for updates...", "sample", "status");
        }
    }

    function onUpdated(deviceInfoUpdate) {
        // Find the corresponding updated DeviceInformation in the collection and pass the update object
        // to the Update method of the existing DeviceInformation. This automatically updates the object
        // for us.
        resultCollection.forEach(function (value, index, array) {
            if (value.id == deviceInfoUpdate.id) {
                value.update(deviceInfoUpdate);
            }
        });
    }

    function onRemoved(deviceInfoUpdate) {
        for (var i = 0; resultCollection.length; i++) {
            if (resultCollection.getAt(i).id == deviceInfoUpdate.id) {
                resultCollection.splice(i, 1);
                break;
            }
        }

        WinJS.log && WinJS.log(resultCollection.length + " devices found. Watching for updates...", "sample", "status");
    }

    function onEnumerationCompleted(obj) {
        WinJS.log && WinJS.log(resultCollection.length + " devices found. Enumeration completed. Watching for updates...", "sample", "status");
    }

    function onStopped(obj) {
        WinJS.log && WinJS.log(resultCollection.length + " devices found. Watcher stopped", "sample", "status");
    }

})();
