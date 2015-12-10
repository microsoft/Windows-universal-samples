//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;

    var page = WinJS.UI.Pages.define("/html/scenario4_snapshot.html", {
        ready: function (element, options) {

            // Setup beforeNavigate event
            WinJS.Navigation.addEventListener("beforenavigate", onLeavingPage);

            // Hook up button event handlers
            document.getElementById("findButton").addEventListener("click", findButtonClick, false);

            // Hook up result list data binding
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.itemDataSource = resultCollection.dataSource;

            // Manually bind selector options
            DisplayHelpers.findAllAsyncSelectors.forEach(function each(item) {
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

    function findButtonClick() {
        var deviceInfoCollection = null;

        findButton.disabled = true;
        resultCollection.splice(0, resultCollection.length);

        var selectedItem = DisplayHelpers.findAllAsyncSelectors.getAt(selectorComboBox.selectedIndex);

        WinJS.log && WinJS.log("FindAllAsync operation started...", "sample", "status");

        if (null != selectedItem.deviceClassSelector) {
            // If the a pre-canned device class selector was chosen, call the DeviceClass overload
            DevEnum.DeviceInformation.findAllAsync(selectedItem.deviceClassSelector).done(findAllComplete, findAllError);
        }
        else {
            // Use AQS string selector from dynamic call to a device api's GetDeviceSelector call
            DevEnum.DeviceInformation.findAllAsync(
                selectedItem.selector,
                null // don't request additional properties for this sample
                ).done(findAllComplete, findAllError);
        }

        findButton.disabled = false;
    }

    function findAllComplete(deviceInfoCollection) {

        WinJS.log && WinJS.log("FindAllAsync operation completed. " + deviceInfoCollection.length + " devices found.", "sample", "status");

        for (var i = 0; i < deviceInfoCollection.length; i++) {
            var deviceInfo = deviceInfoCollection.getAt(i);

            // For simplicity, just creating a new "display object" on the fly since databinding directly with deviceInfo from
            // the callback doesn't work. 
            resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));
        }
    }

    function findAllError(e) {
        WinJS.log && WinJS.log("FindAllAsync operation failed. error = " + e.message, "sample", "error");
    }

})();
