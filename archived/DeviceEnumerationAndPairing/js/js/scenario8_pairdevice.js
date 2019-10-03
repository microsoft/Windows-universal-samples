//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;
    var deviceWatcherHelper = new SdkSample.DeviceWatcherHelper(resultCollection);

    var page = WinJS.UI.Pages.define("/html/scenario8_pairdevice.html", {
        ready: function (element, options) {

            // Hook up button event handlers
            document.getElementById("startWatcherButton").addEventListener("click", startWatcher, false);
            document.getElementById("stopWatcherButton").addEventListener("click", stopWatcher, false);
            document.getElementById("pairButton").addEventListener("click", pairDevice, false);
            document.getElementById("unpairButton").addEventListener("click", unpairDevice, false);

            // Hook up result list selection changed event handler
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.addEventListener("selectionchanged", onSelectionChanged);
            // Hook up result list data binding
            resultsListView.itemDataSource = resultCollection.dataSource;

            // We need to do some extra work when an item changes.
            // This event is raised by the DeviceWatcherHelper.
            resultCollection.addEventListener("deviceupdated", onDeviceUpdated);

            // Manually bind selector options
            DisplayHelpers.pairingSelectors.forEach(function each(item) {
                var option = document.createElement("option");
                option.textContent = item.displayName;
                selectorComboBox.appendChild(option);
            });
            selectorComboBox.selectedIndex = 0;

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

        // Get the device selector chosen by the UI then add additional constraints for devices that 
        // can be paired or are already paired.
        var selectedItem = DisplayHelpers.pairingSelectors.getAt(selectorComboBox.selectedIndex);
        var selector = "(" + selectedItem.selector + ")" + " AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

        var deviceWatcher;
        if (selectedItem.kind == DevEnum.DeviceInformationKind.unknown) {
            // Kind will be determined by the selector
            deviceWatcher = DevEnum.DeviceInformation.createWatcher(
                selector,
                null // don't request additional properties for this sample
                );
        }
        else {
            // Kind will be determined by the selector
            deviceWatcher = DevEnum.DeviceInformation.createWatcher(
                selector,
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

    function getSelectedItem() {
        var selectedIndices = resultsListView.selection.getIndices();
        if (selectedIndices.length > 0) {
            return resultCollection.getAt(selectedIndices[0]);
        }
    }

    function onDeviceUpdated(e) {
        var id = e.detail.id;
        // If the item being updated is currently "selected", then update the pairing buttons
        var selectedItem = getSelectedItem();
        if (selectedItem && selectedItem.id === id) {
            UpdatePairingButtons();
        }
    }

    function pairDevice() {
        // Gray out the pair button and results view while pairing is in progress.
        pairButton.disabled = true;
        WinJS.log && WinJS.log("Pairing started. Please wait...", "sample", "status");

        var deviceDispInfo = getSelectedItem();
        if (deviceDispInfo) {
            deviceDispInfo.deviceInfo.pairing.pairAsync().done(
                function (pairingResult) {
                    var statusType = (pairingResult.status == DevEnum.DevicePairingResultStatus.paired) ? "status" : "error";
                    var message = (pairingResult.status == DevEnum.DevicePairingResultStatus.paired) ? "Paired" : "NotPaired";
                    WinJS.log && WinJS.log("Pairing result = " + message, "sample", statusType);

                    updatePairingButtons();
                });
        }
    }

    function unpairDevice() {
        // Gray out the pair button and results view while pairing is in progress.
        unpairButton.disabled = true;
        WinJS.log && WinJS.log("Unpairing started. Please wait...", "sample", "status");

        var deviceDispInfo = getSelectedItem();
        if (deviceDispInfo) {
            deviceDispInfo.deviceInfo.pairing.unpairAsync().done(
                function (unpairingResult) {
                    var message = (unpairingResult.status == DevEnum.DeviceUnpairingResultStatus.unpaired) ? "Unpaired" : "Failed";
                    var statusType = (unpairingResult.status == DevEnum.DeviceUnpairingResultStatus.unpaired) ? "status" : "error";
                    WinJS.log && WinJS.log("Unpairing result = " + message, "sample", statusType);

                    updatePairingButtons();
                });
        }
    }

    function onSelectionChanged() {
        updatePairingButtons();
    }

    function updatePairingButtons() {

        var selectedItems = resultsListView.selection.getIndices();
        var deviceDispInfo = getSelectedItem();

        if (deviceDispInfo &&
            deviceDispInfo.canPair &&
            !deviceDispInfo.isPaired) {
            pairButton.disabled = false;
        }
        else {
            pairButton.disabled = true;
        }

        if (deviceDispInfo &&
            deviceDispInfo.isPaired) {
            unpairButton.disabled = false;
        }
        else {
            unpairButton.disabled = true;
        }
    }
})();
