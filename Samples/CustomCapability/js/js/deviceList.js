//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var _fx2Watcher;
    var _fx2Devices = new WinJS.Binding.List();

    //
    // Define a class to hold entries in the list of devices.
    //
    var DeviceListEntry = WinJS.Class.define(
        function (deviceInterface) {
            this.deviceInterface = deviceInterface;
            this.matched = true;
        },
        {
            id: { get: function () { return this.deviceInterface.id; } },
            instanceId: { get: function () { return this.deviceInterface.properties["System.Devices.DeviceInstanceId"]; } },
            device: { get: function () { return this.deviceInterface; } }
        },
        {
            findInList: function (id) {
                var match = null;
                var index;
                _fx2Devices.forEach(function (e, i) { if (e.id === id) { match = e; index = i; } });
                return match ? { key: index, data: match } : null;
            }
        }
    );

    // currently selected fx2 device
    var _fx2Device = null;
    var _fx2DeviceId = null;
    var _switchEventHandler = null;

    //
    // Create a public namespace to hold these values.
    //
    WinJS.Namespace.define("DeviceList", {
        // Class for entries held in the list of devices
        DeviceListEntry: DeviceListEntry,

        // The list of Fx2 devices found by the PNP watcher
        fx2Devices: { get: function () { return _fx2Devices; } },

        // The projection for the currently selected Fx2 device
        getSelectedFx2Device: function () {
            return _fx2Device;
        },

        getSelectedFx2DeviceId: function () { return _fx2DeviceId; },

        setSelectedDevice: function (id, device) {

            if (_fx2Device !== null) {
                DeviceList.events.dispatchEvent("deviceclosing");
            }

            _fx2Device = device;
            _fx2DeviceId = id;
        },

        clearSelectedDevice: function () {this.setSelectedDevice(null, null);},

        // Whether the fx2 watcher is currently active.  Retains its 
        // value across suspend/resume
        watcherStarted: false,

        // Method to start the PNP watcher
        startFx2Watcher: startFx2Watcher,

        // Method to stop the PNP watcher
        stopFx2Watcher: stopFx2Watcher,

        // Helper function to generate a table containing boolean values
        createBooleanTable: function (newValues, oldValues, options) {
            // helper function to generate the tables used to display bar graph and 
            // switch state

            var table = document.createElement("table");
            table.border = 1;
            table.width = "40%";

            var head = table.createTHead();
            head = head.insertRow();
            head.insertCell().innerText = options.indexTitle;
            head.insertCell().innerText = options.valueTitle;

            var body = table.createTBody();
            var count = newValues.length;

            for (var i = 0; i < count; i += 1) {

                var value = newValues[i];
                var valueName = value ? options.trueValue : options.falseValue;

                var row = body.insertRow();
                row.insertCell().innerText = i;

                var cell = row.insertCell();

                if (oldValues && oldValues[i] !== newValues[i]) {
                    var bold = document.createElement("b");
                    bold.innerText = valueName;
                    cell = cell.appendChild(bold);
                }
                cell.innerText = valueName;
            }
            return table;
        },
        events: new (WinJS.Class.mix(WinJS.Class.define(null), WinJS.Utilities.eventMixin))
    });

    function initDeviceWatcher() {
        // Define the selector to enumerate all of the fx2 device interface class instances
        var selector = Windows.Devices.Custom.CustomDevice.getDeviceSelector(Fx2Driver.deviceInterfaceGuid);

        // Create a device watcher to look for instances of the fx2 device interface
        _fx2Watcher = Windows.Devices.Enumeration.DeviceInformation.createWatcher(
                    selector,
                    ["System.Devices.DeviceInstanceId"]
                    );

        // register to know when devices come and go, and when an enumeration ends
        _fx2Watcher.addEventListener("added", onFx2Added);
        _fx2Watcher.addEventListener("removed", onFx2Removed);
        _fx2Watcher.addEventListener("enumerationcompleted", onFx2EnumerationComplete);
    }

    function startFx2Watcher() {
        WinJS.log && WinJS.log("starting device watcher", "sample", "info");
        // Clear the matched property on each entry that's already in the list
        if (!DeviceList.watcherStarted) {
            DeviceList.watcherStarted = true;
            resumeFx2Watcher();
        }
    }

    function suspendFx2Watcher() {
        if (DeviceList.watcherStarted) {
            _fx2Watcher.stop();
        }
    }

    function resumeFx2Watcher() {
        if (DeviceList.watcherStarted) {
            // Clear the matched property on each entry that's already in the list
            _fx2Devices.forEach(function (e) { e.matched = false; });
            _fx2Watcher.start();
        }
    }

    function stopFx2Watcher() {
        WinJS.log && WinJS.log("stopping fx2 watcher", "sample", "info");
        suspendFx2Watcher();
        DeviceList.watcherStarted = false;
    }

    // Event handler for arrival of Fx2 devices.  If the device isn't already in the
    // list then it's added.  If it is already in the list, then the verified property
    // is set to true so that endDeviceListUpdate() knows the device is still present
    function onFx2Added(devInterface) {
        WinJS.log && WinJS.log("onFx2Added: " + devInterface.id, "sample", "info");

        // Search the device list for a device with a matching interface ID
        var match = DeviceListEntry.findInList(devInterface.id);

        // If we found a match then mark it as verified and return
        if (match !== null) {
            match.data.matched = true;
            return;
        }

        // Create a new element for this device interface, and queue up the query of its
        // device information.
        match = new DeviceListEntry(devInterface);

        // Add the new element to the end of the list of devices
        _fx2Devices.push(match);
    }

    // event handler for removal of an Fx2 device.  If the device is in the list, it clears
    // the verified property and then purges it from the list by calling endDeviceListUpdate()
    function onFx2Removed(devInformation) {
        var deviceId = devInformation.id;
        WinJS.log && WinJS.log("onFx2Removed: " + deviceId, "sample", "info");

        // Search the list of devices for one with a matching ID.
        var match = DeviceListEntry.findInList(devInformation.id);
        if (match !== null) {
            // remove the matched item
            WinJS.log && WinJS.log("onFx2Removed: " + deviceId, "sample", "info");
            _fx2Devices.splice(match.key, 1);
        }
    }

    // Event handler for the end of the enumeration triggered when starting the watcher
    // This calls endDeviceListUpdate() to purge the device list of any devices which
    // are no longer present (their verified property is false)
    function onFx2EnumerationComplete(devInformation) {
        // Iterate through the list of devices and remove any that hasn't been matched.
        // removing the selected entry will automatically trigger its close
        _fx2Devices.forEach(
            function (entry, index) {
                if (!entry.matched) {
                    _fx2Devices.splice(index, 1);
                }
            }
            );
    }

    // event callback for suspend events.  Stops the device watcher
    function onSuspend() {
        // stop the device watcher when we get suspended.  We will restart it on resume,
        // which will trigger a new enumeration.
        suspendFx2Watcher();
    }

    // event callback for resume events.  clears the verified property on all devices
    // then restarts the device watcher.
    function onResume() {
        resumeFx2Watcher();
    }

    //
    // Register global handlers for suspend/resume to stop and restart the device
    // watcher.
    //
    Windows.UI.WebUI.WebUIApplication.addEventListener("suspending", onSuspend, false);
    Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", onResume, false);

    //
    // Initialize the device watcher
    //
    initDeviceWatcher();
})();
