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
    var PosConnectionTypes = Windows.Devices.PointOfService.PosConnectionTypes;
    var LineDisplay = Windows.Devices.PointOfService.LineDisplay;
    var ClaimedLineDisplay = Windows.Devices.PointOfService.ClaimedLineDisplay;

    var deviceListElement;
    var selectButton;

    var page = WinJS.UI.Pages.define("/html/Scenario1_SelectDisplay.html", {

        ready: function (element, options) {

            deviceListElement = document.getElementById("deviceList");
            selectButton = document.getElementById("selectButton");

            selectButton.addEventListener("click", selectLineDisplay, false);

            // Enumerate all the LineDisplay devices and put them in our list box.
            DeviceInformation.findAllAsync(LineDisplay.getDeviceSelector(PosConnectionTypes.all), [])
                .then(function(deviceInfoCollection) {
                    deviceInfoCollection.forEach(function(deviceInfo) {
                        var newOption = document.createElement("option");
                        newOption.text = deviceInfo.name;
                        newOption.value = deviceInfo.id;
                        deviceListElement.add(newOption);
                    });
                });
        }
    });

    function selectLineDisplay() {
        if (deviceListElement.selectedIndex >= 0) {
            selectButton.disabled = true;

            var selectedItem = deviceListElement[deviceListElement.selectedIndex];
            var name = selectedItem.text;
            var deviceId = selectedItem.value;

            ClaimedLineDisplay.fromIdAsync(deviceId).then(function (lineDisplay) {
                if (lineDisplay) {
                    return lineDisplay.defaultWindow.tryClearTextAsync().then(function(result) {
                        WinJS.log("Selected: " + name, "sample", "status");

                        // Save this device ID for other scenarios.
                        SdkSample.lineDisplayId = deviceId;

                        // Close the claimed line display.
                        lineDisplay.close();
                    });
                } else {
                    WinJS.log("Unable to claim the Line Display", "sample", "error");
                }
            }).done(function() {
                selectButton.disabled = false;
            });
        }
    }
})();