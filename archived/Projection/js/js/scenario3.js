//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var dataList = new WinJS.Binding.List();
    var ViewManagement = Windows.UI.ViewManagement;

    // Create ready handlers for start/stop projection button.
    var FindAllDisplaysButton;

    // Create a new view for projecting.
    var view = window.open("ms-appx:///html/secondaryView.html", null, "msHideView=yes");
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            WinJS.UI.processAll();
            FindAllDisplaysButton = document.getElementById("FindAllDisplaysButton");
            FindAllDisplaysButton.addEventListener("click", FindAllDisplays, false);
        }
    });

    function FindAllDisplays() {
        var listView = document.getElementById("devicesListView");

        // Use the device selector query of the ProjectionManager to list wired/wireless displays
        var projectorSelectorQuery = ViewManagement.ProjectionManager.getDeviceSelector();
        var propertiesToRetrieve = ["System.ItemNameDisplay", "System.Devices.ModelName", "System.Devices.Connected"];

        // Disable Start projection button to avoid clicking it twice.
        FindAllDisplaysButton.disabled = true;

        WinJS.log("Searching for devices...", "sample", "status");

        // Start projection using the previously created secondary view.
        Windows.Devices.Enumeration.DeviceInformation.findAllAsync(projectorSelectorQuery, propertiesToRetrieve).done(function (deviceCollection) {
            for (var i = 0; i < deviceCollection.length; i++) {
                var device = new Object();
                device.displayName = deviceCollection[i].properties["System.ItemNameDisplay"];
                device.id = deviceCollection[i].id;
                dataList.push(device);
            }
            
            // Bind listView to our dataList.
            listView.winControl.itemDataSource = dataList.dataSource;
            listView.winControl.itemTemplate = document.querySelector("#deviceTileTemplate");
            listView.winControl.forceLayout();

            // Re-enable find displays button so that user can re-enumerate displays.
            FindAllDisplaysButton.disabled = false;

            // Add event listener for iteminvoked on the listView.
            listView.winControl.addEventListener('iteminvoked', startProjectingToDisplay);

            WinJS.log("Found devices are now listed", "sample", "status");
        }, function (e) {
            WinJS.log && WinJS.log(e + "An error occured when querying and listing devices.", "sample", "error");
        });
    }

    function startProjectingToDisplay(eventObj) {
        var selectedItem = dataList.getAt(eventObj.detail.itemIndex);
        Windows.Devices.Enumeration.DeviceInformation.createFromIdAsync(selectedItem.id).done(function (selectedDevice) {
           ViewManagement.ProjectionManager.startProjectingAsync(
                    MSApp.getViewId(view),
                    ViewManagement.ApplicationView.getForCurrentView().id,
                    selectedDevice
                ).done(function () {
                    while (dataList.length > 0)
                    {
                        dataList.pop();
                    }

                    // Clear out the previous error message if there is any.
                    WinJS.log("", "sample", "status");
                }, function (e) {
                    while (dataList.length > 0) {
                        dataList.pop();
                    }
                    WinJS.log && WinJS.log(e + "\n The current app or projection view window must be active for startProjectingAsync() to succeed", "sample", "error");
                });
            }, false);
    }

    WinJS.UI.processAll();
})();

