//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var deviceWatcher = null;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;

    var page = WinJS.UI.Pages.define("/html/scenario8_pairdevice.html", {
        ready: function (element, options) {

            // Setup beforeNavigate event
            WinJS.Navigation.addEventListener("beforenavigate", onLeavingPage);

            // Hook up button event handlers
            document.getElementById("startWatcherButton").addEventListener("click", startWatcher, false);
            document.getElementById("stopWatcherButton").addEventListener("click", stopWatcher, false);
            document.getElementById("pairButton").addEventListener("click", pairDevice, false);

            // Hook up result list selection changed event handler
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.addEventListener("selectionchanged", onSelectionChanged);
            // Hook up result list data binding
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Manually bind selector options
            DisplayHelpers.protocolList.forEach(function each(item) {
                var option = document.createElement("option");
                option.textContent = item.displayName;
                selectorComboBox.appendChild(option);
            });

            // Process any data bindings
            WinJS.UI.processAll();
        }
    });

    function onLeavingPage() {
        stopWatcher();
        WinJS.Navigation.removeEventListener("beforenavigate", onLeavingPage);
        resultCollection.splice(0, resultCollection.length);
    }

    function startWatcher() {
        startWatcherButton.disabled = true;
        resultCollection.splice(0, resultCollection.length);

        // Request the IsAssociated property so we can display the associated ("paired") status in the UI
        var requestedProperties = [
            "System.Devices.Aep.IsAssociated"
        ];

        var selectedItem = DisplayHelpers.protocolList.getAt(selectorComboBox.selectedIndex);

        // Get the device selector chosen by the UI, then 'AND' it with the 'IsAssociatable' property
        var aqsFilter = selectedItem.selector + " AND System.Devices.Aep.IsAssociatable:=System.StructuredQueryType.Boolean#True";

        deviceWatcher = DevEnum.DeviceInformation.createWatcher(
            aqsFilter,
            requestedProperties,
            DevEnum.DeviceInformationKind.associationEndpoint
            );

        // Add event handlers
        deviceWatcher.addEventListener("added", onAdded);
        deviceWatcher.addEventListener("updated", onUpdated);
        deviceWatcher.addEventListener("removed", onRemoved);
        deviceWatcher.addEventListener("enumerationcompleted", onEnumerationCompleted);
        deviceWatcher.addEventListener("stopped", onStopped);

        WinJS.log && WinJS.log("Starting watcher...", "sample", "status");
        deviceWatcher.start();
        stopWatcherButton.disabled = false;
    }

    function stopWatcher() {

        stopWatcherButton.disabled = true;

        if (null != deviceWatcher) {

            // First unhook all event handlers except the stopped handler. This ensures our
            // event handlers don't get called after stop, as stop won't block for any "in flight" 
            // event handler calls.  We leave the stopped handler as it's guaranteed to only be called
            // once and we'll use it to know when the query is completely stopped. 
            deviceWatcher.removeEventListener("added", onAdded);
            deviceWatcher.removeEventListener("updated", onUpdated);
            deviceWatcher.removeEventListener("removed", onRemoved);
            deviceWatcher.removeEventListener("enumerationcompleted", onEnumerationCompleted);

            if (DevEnum.DeviceWatcherStatus.started == deviceWatcher.status ||
                DevEnum.DeviceWatcherStatus.enumerationCompleted == deviceWatcher.status) {
                deviceWatcher.stop();
            }
        }

        startWatcherButton.disabled = false;
    }

    function onAdded(deviceInfo) {
        // For simplicity, just creating a new "display object" on the fly since databinding directly with deviceInfo from
        // the callback doesn't work. 
        resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));

        if (deviceWatcher.status == DevEnum.DeviceWatcherStatus.enumerationCompleted) {
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

    function pairDevice() {
        var selectedItems = resultsListView.selection.getIndices();

        if (selectedItems.length > 0) {
            var deviceDispInfo = resultCollection.getAt(selectedItems[0]);

            pairButton.disabled = true;
            deviceDispInfo.deviceInfo.pairing.pairAsync();
        }
    }

    function onSelectionChanged() {
        var selectedItems = resultsListView.selection.getIndices();
        var deviceDispInfo = null;
        
        if (selectedItems.length > 0) {
            deviceDispInfo = resultCollection.getAt(selectedItems[0]);
        }

        if (null != deviceDispInfo &&
            true == deviceDispInfo.canPair &&
            false == deviceDispInfo.isPaired) {

            pairButton.disabled = false;
        }
        else {
            pairButton.disabled = true;
        }
    }

})();
