//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

(function () {
    "use strict";
    var DeviceInformation = Windows.Devices.Enumeration.DeviceInformation;
    var DeviceWatcherStatus = Windows.Devices.Enumeration.DeviceWatcherStatus;
    var PosConnectionTypes = Windows.Devices.PointOfService.PosConnectionTypes;
    var LineDisplay = Windows.Devices.PointOfService.LineDisplay;
    var ClaimedLineDisplay = Windows.Devices.PointOfService.ClaimedLineDisplay;

    var deviceListElement;
    var selectButton;
    var refreshButton;

    var watcher;

    var page = WinJS.UI.Pages.define("/html/scenario1-selectDisplay.html", {

        ready: function (element, options) {

            deviceListElement = document.getElementById("deviceList");
            selectButton = document.getElementById("selectButton");
            refreshButton = document.getElementById("refreshButton");

            selectButton.addEventListener("click", selectLineDisplay);
            refreshButton.addEventListener("click", restartWatcher);

            restartWatcher();
        },

        unload: function () {
            stopWatcher();
        }
    });

    function restartWatcher() {
        stopWatcher();

        // Clear any old LineDisplay devices from the list box.
        deviceListElement.innerHTML = "";

        // Enumerate the LineDisplay devices and put them in our list box.
        watcher = DeviceInformation.createWatcher(LineDisplay.getDeviceSelector(PosConnectionTypes.all), null);
        watcher.addEventListener("added", deviceAdded);
        watcher.addEventListener("enumerationcompleted", deviceEnumerationCompleted);
        watcher.start();

    }

    function stopWatcher() {
        if (watcher && (watcher.status === DeviceWatcherStatus.started)) {
            watcher.stop();
        }
        watcher = null;
    }

    function deviceAdded(deviceInfo) {
        SdkSample.addOption(deviceListElement, deviceInfo.name, deviceInfo.id);
    }

    function deviceEnumerationCompleted(e) {
        e.target.stop();
    }

    async function selectLineDisplay() {
        if (deviceListElement.selectedIndex >= 0) {
            selectButton.disabled = true;

            var selectedItem = deviceListElement[deviceListElement.selectedIndex];
            var name = selectedItem.text;
            var deviceId = selectedItem.value;

            var lineDisplay = await ClaimedLineDisplay.fromIdAsync(deviceId);
            if (lineDisplay) {
                await lineDisplay.defaultWindow.tryClearTextAsync();
                WinJS.log(`Selected: ${name}`, "sample", "status");

                // Save this device ID for other scenarios.
                SdkSample.lineDisplayId = deviceId;

                // Close the claimed line display.
                lineDisplay.close();
            } else {
                WinJS.log("Unable to claim the Line Display", "sample", "error");
            }
            selectButton.disabled = false;
        }
    }

})();