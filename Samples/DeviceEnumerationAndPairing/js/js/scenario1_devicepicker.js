//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var devicePicker = null;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;

    var page = WinJS.UI.Pages.define("/html/scenario1_devicepicker.html", {
        ready: function (element, options) {

            // Setup beforeNavigate event
            WinJS.Navigation.addEventListener("beforenavigate", onLeavingPage);

            // Hook up button event handlers
            document.getElementById("pickSingleDeviceButton").addEventListener("click", pickSingleDeviceClick, false);
            document.getElementById("showDevicePickerButton").addEventListener("click", showDevicePickerClick, false);

            // Hook up result list data binding
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Manually bind selector options
            DisplayHelpers.devicePickerSelectors.forEach(function each(item) {
                var option = document.createElement("option");
                option.textContent = item.displayName;
                selectorComboBox.appendChild(option);
            });

            // Process any data bindings
            WinJS.UI.processAll();
        }
    });

    function onLeavingPage() {
        WinJS.Navigation.removeEventListener("beforenavigate", onLeavingPage);
        resultCollection.splice(0, resultCollection.length);
    }

    function pickSingleDeviceClick() {
        showDevicePicker(true);
    }
    function showDevicePickerClick() {
        showDevicePicker(false);
    }

    function showDevicePicker(pickSingle) {
        var buttonRect;

        showDevicePickerButton.disabled = true;
        resultCollection.splice(0, resultCollection.length);

        devicePicker = new DevEnum.DevicePicker();

        var selectedItem = DisplayHelpers.devicePickerSelectors.getAt(selectorComboBox.selectedIndex);

        // If the selected device selector is a pre-canned device class, call CreateWatcher with the device class
        // overload.  Otherwise assume the device selector contains an AQS filter selector and use that overload.
        if (null != selectedItem.deviceClassSelector) {
            devicePicker.filter.supportedDeviceClasses.push(selectedItem.deviceClassSelector);
        }
        else {
            devicePicker.filter.supportedDeviceSelectors.push(selectedItem.selector);
        }

        WinJS.log && WinJS.log("Showing Device Picker...", "sample", "status");

        if (pickSingle) {
            buttonRect = document.getElementById("pickSingleDeviceButton").getBoundingClientRect();
        }
        else {
            buttonRect = document.getElementById("showDevicePickerButton").getBoundingClientRect();
        }

        var rect = { x: buttonRect.left, y: buttonRect.top, width: buttonRect.width, height: buttonRect.height };

        if (pickSingle) {
            devicePicker.pickSingleDeviceAsync(rect).done(function (deviceInfo) {
                if (null != deviceInfo) {
                    resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));
                }
                showDevicePickerButton.disabled = false;
            });
        }
        else {
            devicePicker.addEventListener("devicepickerdismissed", onPickerDismissed);
            devicePicker.addEventListener("deviceselected", onDeviceSelected);

            // Show the picker
            devicePicker.show(rect);
        }        
    }

    function onPickerDismissed() {
        showDevicePickerButton.disabled = false;
        WinJS.log && WinJS.log("Hiding Device Picker", "sample", "status");
    }

    function onDeviceSelected(args) {
        resultCollection.splice(0, resultCollection.length);
        resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(args.selectedDevice));
    }

})();
