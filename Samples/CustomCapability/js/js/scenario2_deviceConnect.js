//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2_deviceConnect.html", {
        ready: function (element, options) {
            document.getElementById("device-connect-devices").addEventListener("selectionchanged", onSelectFx2Device, false);

            document.getElementById("device-connect-start").addEventListener("click", onStartWatcher, false);
            document.getElementById("device-connect-stop").addEventListener("click", onStopWatcher, false);

            updateStartStopButtons();
        },
        processed: function (element, options) {
            if (DeviceList.fx2Device) {
                var match = DeviceList.DeviceListEntry.findInList(DeviceList.fx2Device.id);

                // Select the list element corresponding to the Fx2 device we have open
                var listView = document.getElementById("device-connect-devices").winControl;
                var selection = listView.selection;
                selection.set(match.key);
            }
            return;
        }
    });

    function onStartWatcher() {
        document.getElementById("device-connect-start").setAttribute("disabled", "disabled");
        WinJS.log && WinJS.log("starting device watcher", "sample", "status");
        DeviceList.startFx2Watcher();
        updateStartStopButtons();
    }

    function onStopWatcher() {
        document.getElementById("device-connect-stop").setAttribute("disabled", "");
        DeviceList.stopFx2Watcher();
        updateStartStopButtons();
    }

    function updateStartStopButtons() {
        var started = DeviceList.watcherStarted;
        var start = document.getElementById("device-connect-start");
        var stop = document.getElementById("device-connect-stop");

        var enable = started ? stop : start;
        var disable = started ? start : stop;

        enable.removeAttribute("disabled");
        disable.setAttribute("disabled", "disabled");
    }

    // event handler for selected events on the UI list of devices
    function onSelectFx2Device(e) {
        var x = 0;
        var selection = e.target.winControl.selection;
        selection.getItems().
            then(function (i) {
                var currentlySelectedId = DeviceList.getSelectedFx2DeviceId();
                var newlySelectedId = i.length ? i[0].data.id : null;

                //
                // If the ids match there's no work to do.
                //
                if (currentlySelectedId === newlySelectedId) {
                    return;
                }

                //
                // If there's a currently selected item them close it.
                //
                if (currentlySelectedId) {
                    closeFx2Device();
                }

                //
                // Open the new item
                //
                if (newlySelectedId) {
                    e.target.disabled = true;
                    var p = openFx2DeviceAsync(newlySelectedId);
                    
                    p.then(
                        function() {e.target.disabled = false;}
                        );
                }
            }).
            done(
                null,
                function () {
                    // An error occurred while trying to select a device.  Clear the selection.
                    e.target.winControl.selection.clear();
                    return;
                });
    }

    // Called before starting the device watcher, this clears
    // the verified flag on every device in the list.
    function beginDeviceListUpdate() {
        DeviceList.fx2Devices.forEach(function (d) { d.verified = false; });
    }

    // Called after the device watcher's enumeration completes, this
    // removes every list entry that hasn't been verified yet.
    function endDeviceListUpdate() {

        // Iterate through the list and remove any unverified entry.
        DeviceList.fx2Devices.forEach(
            function (element, index) {
                if (!element.verified) {
                    DeviceList.fx2Devices.splice(index, 1);
                }
            }
        );
    }

    // Iterates across the list of Fx2 devices until the match function returns true
    function findDeviceInfo(matchFunction) {
        var match = null;
        DeviceList.fx2Devices.forEach(function (e, index) {
            if (matchFunction(e)) {
                match = e;
            }
        });
        return match;
    }

    // creates an instance of the Samples.Devices.Fx2.Fx2Device class for the specified
    // device
    function openFx2DeviceAsync(id) {

        DeviceList.clearSelectedDevice();

        document.getElementById("device-connect-output").innerHTML = "";

        // Open a handle to the fx2 device.
        var p =
        Windows.Devices.Custom.CustomDevice.fromIdAsync(
            id,
            Windows.Devices.Custom.DeviceAccessMode.readWrite,
            Windows.Devices.Custom.DeviceSharingMode.exclusive
            );

        p = p.then(
            function(device)
            {
                DeviceList.setSelectedDevice(id, device);
                WinJS.log && WinJS.log("Fx2 " + id + " opened", "sample", "status");
            },
            function(error)
            {
                WinJS.log && WinJS.log("Error opening Fx2 device @" + id + ": " + error, "sample", "error");
                diagnoseConnectionError(id, error);
                throw error;
            }
        );

        return p;
    }

    // Closes the currently opened device
    function closeFx2Device() {

        if (DeviceList.fx2Device !== null) {
            // drop the reference to the current device, so it can be closed
            WinJS.log && WinJS.log("Fx2 " + DeviceList.fx2Device.id + " closed", "sample", "status");
            DeviceList.fx2Device = null;
        }

        return;
    }

    // Diagnostic routine that analyzes why creating the Fx2Device failed.  This looks at
    // the device's properties and matches them to the expected values to report common
    // developer errors in the device metadata or INF file.
    //
    // A real app might use this to report error data back to the developer, but none of
    // the errors reported here are something the user has done wrong or can fix, so they
    // shouldn't be reported to a real user.
    function diagnoseConnectionError(id, error) {

        WinJS.log && WinJS.log("Unable to open Fx2 device " + id + ".\n" +
                               error + "\nLikely causes are:\n" +
                               "1. Device metadata does not grant custom privileged access to any apps\n" +
                               "2. Device metadata and the app package have a mismatch on the app name and/or publisher ID\n" +
                               "3. The device interface was not properly marked as restricted by the driver\n" +
                               "4. The device is no longer connected\n",
                               "sample", "error");

    }
})();
