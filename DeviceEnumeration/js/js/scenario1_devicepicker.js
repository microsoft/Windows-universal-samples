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
            document.getElementById("hideDevicePickerButton").addEventListener("click", hideDevicePickerClick, false);

            // Hook up result list data binding
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Manually bind selector options
            DisplayHelpers.selectorsList.forEach(function each(item) {
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
    function hideDevicePickerClick() {
    }

    function showDevicePicker(pickSingle) {
        showDevicePickerButton.disabled = true;
        resultCollection.splice(0, resultCollection.length);

        devicePicker = new DevEnum.DevicePicker();

        var selectedItem = DisplayHelpers.selectorsList.getAt(selectorComboBox.selectedIndex);

        // If the selected device selector is a pre-canned device class, call CreateWatcher with the device class
        // overload.  Otherwise assume the device selector contains an AQS filter selector and use that overload.
        if (null != selectedItem.deviceClassSelector) {
            devicePicker.filter.supportedDeviceClasses.push(selectedItem.deviceClassSelector);
        }
        else {
            devicePicker.filter.supportedDeviceSelectors.push(selectedItem.selector);
        }

        WinJS.log && WinJS.log("Showing Device Picker...", "sample", "status");

        var rect = {x:0, y:0, width:200, height:300};

        if (pickSingle) {
            devicePicker.pickSingleDeviceAsync(rect).done(function (deviceInfo) {
                resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));
                showDevicePickerButton.disabled = false;
            });
        }
        else {
            devicePicker.addEventListener("devicePickerDismissed", onPickerDismissed);
            devicePicker.addEventListener("deviceSelected", onDeviceSelected);

            // Show the picker
            devicePicker.show(rect);
            hideDevicePickerButton.disabled = false;
        }        
    }

    function onPickerDismissed() {
        showDevicePickerButton.disabled = false;
        hideDevicePickerButton.disabled = true;
        WinJS.log && WinJS.log("Hiding Device Picker", "sample", "status");
    }

    function onDeviceSelected(picker, args) {
        resultCollection.splice(0, resultCollection.length);
        resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(args.selectedDevice));
    }

})();
