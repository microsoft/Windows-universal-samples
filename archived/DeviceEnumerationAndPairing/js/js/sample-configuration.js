//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var DevEnum = Windows.Devices.Enumeration;

    var sampleTitle = "Device Enumeration and Pairing JS Sample";

    var scenarios = [
        { url: "/html/scenario1_devicepicker.html", title: "Device Picker Common Control" },
        { url: "/html/scenario2_devicewatcher.html", title: "Enumerate and Watch Devices" },
        { url: "/html/scenario3_backgrounddevicewatcher.html", title: "Enumerate and Watch Devices in a Background Task" },
        { url: "/html/scenario4_snapshot.html", title: "Enumerate Snapshot of Devices" },
        { url: "/html/scenario5_getsingledevice.html", title: "Get Single Device" },
        { url: "/html/scenario6_customfilteraddedprops.html", title: "Custom Filter with Additional Properties" },
        { url: "/html/scenario7_deviceinformationkind.html", title: "Request Specific DeviceInformationKind" },
        { url: "/html/scenario8_pairdevice.html", title: "Basic Device Pairing" },
        { url: "/html/scenario9_custompairdevice.html", title: "Custom Device Pairing" }
    ];

    /// <summary>
    /// Updates a WinJS.Binding.List based on events from a DeviceWatcher.
    /// </summary>
    /// <remarks>
    /// Encapsulates the work necessary to register for watcher events,
    /// start and stop the watcher, handle race conditions, and break cycles.
    /// </remarks>

    class DeviceWatcherHelper {
        constructor(resultCollection) {
            this._resultCollection = resultCollection;
            this._deviceWatcher = null;
            this.updateStatus = true;
        }

        startWatcher(deviceWatcher) {
            this._deviceWatcher = deviceWatcher;

            // Connect events to update our collection as the watcher report results.
            deviceWatcher.addEventListener("added", DeviceWatcherHelper._onDeviceAdded.bind(this));
            deviceWatcher.addEventListener("updated", DeviceWatcherHelper._onDeviceUpdated.bind(this));
            deviceWatcher.addEventListener("removed", DeviceWatcherHelper._onDeviceRemoved.bind(this));
            deviceWatcher.addEventListener("enumerationcompleted", DeviceWatcherHelper._onEnumerationCompleted.bind(this));
            deviceWatcher.addEventListener("stopped", DeviceWatcherHelper._onStopped.bind(this));

            deviceWatcher.start();
        }

        stopWatcher() {
            // Since the device watcher runs in the background, it is possible that
            // a notification is "in flight" at the time we stop the watcher.
            // In other words, it is possible for the watcher to become stopped while a
            // handler is running, or for a handler to run after the watcher has stopped.

            if (DeviceWatcherHelper.isWatcherStarted(this._deviceWatcher)) {
                // We do not null out the deviceWatcher yet because we want to receive
                // the Stopped event.
                this._deviceWatcher.stop();
            }
        }

        reset() {
            if (this._deviceWatcher) {
                this.stopWatcher();
                this._deviceWatcher = null;
            }
        }

        static isWatcherStarted(watcher) {
            return (watcher.status === DevEnum.DeviceWatcherStatus.started) ||
                (watcher.status === DevEnum.DeviceWatcherStatus.enumerationCompleted);
        }

        static _onDeviceAdded(deviceInfo) {
            var sender = deviceInfo.target;

            // Watcher may have stopped while we were waiting for our chance to run.
            if (DeviceWatcherHelper.isWatcherStarted(sender)) {

                // For simplicity, just creating a new "display object" on the fly since databinding directly with deviceInfo from
                // the callback doesn't work. 
                this._resultCollection.push(new DisplayHelpers.DeviceInformationDisplay(deviceInfo));
                this._raiseDeviceChanged(sender, deviceInfo.id);
            }
        }

        static _onDeviceUpdated(deviceInfoUpdate) {
            var sender = deviceInfoUpdate.target;

            // Watcher may have stopped while we were waiting for our chance to run.
            if (DeviceWatcherHelper.isWatcherStarted(sender)) {

                // Find the corresponding updated DeviceInformation in the collection and pass the update object
                // to the Update method of the existing DeviceInformation. This automatically updates the object
                // for us.
                this._resultCollection.forEach(value => {
                    if (value.id === deviceInfoUpdate.id) {
                        value.update(deviceInfoUpdate);
                        this._raiseDeviceChanged(sender, deviceInfoUpdate.id);
                    }
                });
            }
        }

        static _onDeviceRemoved(deviceInfoUpdate) {
            var sender = deviceInfoUpdate.target;

            // Watcher may have stopped while we were waiting for our chance to run.
            if (DeviceWatcherHelper.isWatcherStarted(sender)) {
                for (var i = 0; this._resultCollection.length; i++) {
                    if (this._resultCollection.getAt(i).id === deviceInfoUpdate.id) {
                        this._resultCollection.splice(i, 1);
                        this._raiseDeviceChanged(sender, deviceInfoUpdate.id);
                        break;
                    }
                }
            }
        }

        static _onEnumerationCompleted(e) {
            this._raiseDeviceChanged(e.target, "");
        }

        static _onStopped(e) {
            this._raiseDeviceChanged(e.target, "");
        }

        _raiseDeviceChanged(sender, id) {
            if (this.updateStatus) {
                // Display a default status message.
                var message = "";
                switch (sender.status) {
                    case DevEnum.DeviceWatcherStatus.started:
                        message = `${this._resultCollection.length} devices found.`;
                        break;

                    case DevEnum.DeviceWatcherStatus.enumerationCompleted:
                        message = `${this._resultCollection.length} devices found. Enumeration completed. Watching for updates...`;
                        break;

                    case DevEnum.DeviceWatcherStatus.stopped:
                        message = `${this._resultCollection.length} devices found. Watcher stopped.`;
                        break;

                    case DevEnum.DeviceWatcherStatus.aborted:
                        message = `${this._resultCollection.length} devices found. Watcher aborted.`;
                        break;
                }

                if (message) {
                    WinJS.log && WinJS.log(message, "sample", "status");
                }
            }

            this._resultCollection.dispatchEvent("deviceupdated", { sender: sender, id: id });
        }
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        DeviceWatcherHelper: DeviceWatcherHelper
    });
})();