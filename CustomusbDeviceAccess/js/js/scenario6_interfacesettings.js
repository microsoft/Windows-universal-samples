//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    /// <summary>
    /// Scenario will demonstrate how to change interface settings (also known as alternate interface setting).
    /// 
    /// This scenario can work for any device as long as it is "added" into the SdkSample.CustomUsbDeviceAccess. For more information on how to add a 
    /// device to make it compatible with this scenario, please see Scenario1_DeviceConnect.
    /// </summary>
    var InterfaceSettingsClass = WinJS.Class.define(null, {
        /// <summary>
        /// Selects the interface setting on the default interface.
        /// 
        /// The interfaceSetting is 0 based, where setting 0 is the default interfaceSetting.
        /// </summary>
        /// <param name="settingNumber"></param>
        setInterfaceSetting: function (settingNumber) {
            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                var interfaceSetting = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.interfaceSettings.getAt(settingNumber);

                interfaceSetting.selectSettingAsync().done(function () {
                    WinJS.log && WinJS.log("Interface Setting is set to " + settingNumber, "sample", "status");
                });
            }
        },
        /// <summary>
        /// Figures out which interface setting is currently selected by checking each interface setting even if the
        /// setting cannot be selected by this sample app.
        /// It will print out the selected interface setting number.
        /// </summary>
        getInterfaceSetting: function () {
            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                var availableInterfaceSettings = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.interfaceSettings;

                for (var interfaceSetting = 0, numInterfaceSettings = availableInterfaceSettings.size; interfaceSetting < numInterfaceSettings; interfaceSetting++) {
                    if (availableInterfaceSettings.getAt(interfaceSetting).selected) {
                        var interfaceSettingNumber = availableInterfaceSettings.getAt(interfaceSetting).interfaceDescriptor.alternateSettingNumber;

                        WinJS.log && WinJS.log("Current interface setting : " + interfaceSettingNumber, "sample", "status");

                        break;
                    }
                }

            }
        }
    },
    null);

    var interfaceSettings = new InterfaceSettingsClass();

    var page = WinJS.UI.Pages.define("/html/scenario6_interfaceSettings.html", {
        ready: function (element, options) {
            // Set up Button listeners before hiding scenarios in case the button is removed when hiding scenario
            document.getElementById("buttonSetSetting").addEventListener("click", setSuperMuttSettingClick, false);
            document.getElementById("buttonGetSetting").addEventListener("click", getSettingClick, false);

            // Available scenarios
            var deviceScenarios = {};
            deviceScenarios[SdkSample.Constants.deviceType.all] = document.querySelector(".generalScenario");

            SdkSample.CustomUsbDeviceAccess.utilities.setUpDeviceScenarios(deviceScenarios, document.querySelector(".deviceScenarioContainer"));

            // Enumerate all the interface settings of the default interface and add them to list for user to choose
            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                var defaultInterface = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface;

                defaultInterface.interfaceSettings.forEach(function (interfaceSetting, settingNumber) {
                    // Create item for the list
                    var item = document.createElement("option");
                    item.text = settingNumber;
                    item.value = settingNumber;

                    window.interfaceSettingsToChoose.appendChild(item);
                });

                // Default select the first interface setting because it's always going to be available
                window.interfaceSettingsToChoose.selectedIndex = 0;

                // Only allow setting of the interface settings for the SuperMutt device because it will not break the rest of the scenarios
                if (!SdkSample.CustomUsbDeviceAccess.utilities.isSuperMuttDevice(SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device)) {
                    window.buttonSetSetting.disabled = true;
                }
            }
        }
    });

    /// <summary>
    /// Determines which item is clicked and what the interfaceSettingNumber that item corresponds to.
    /// Will not allow changing of the interface setting if not a SuperMutt. If it is a SuperMutt,
    /// only the first two interface settings can be used because the first two interface settings
    /// are identical with respect to the API.
    /// </summary>
    function setSuperMuttSettingClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            // Since we added the the settings in order, the index of the item clicked will give us the index of the
            // setting in UsbInterface->InterfaceSettings
            interfaceSettings.setInterfaceSetting(window.interfaceSettingsToChoose.selectedIndex);
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function getSettingClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            interfaceSettings.getInterfaceSetting();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }
})();
