//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var SerialEventsClass = WinJS.Class.define(null, {

        // Device that we registered for events with
        _registeredDevice: null,
        _registeredDeviceForErrorReceived: null,
        isRegisteredForPinChangedEvents: false,
        pinChangedEventHandler: null,

        isRegisteredForErrorReceivedEvents: false,
        errorReceivedEventHandler: null,

        // Did we navigate away from this page?
        navigatedAway: false,

        /// <summary>
        /// </summary>
        onPinChangedEvent: function (eventArgs) {

            // If we navigated away from this page, we don't need to process this event
            // This also prevents output from spilling into another page
            if (!serialEvents.navigatedAway) {
                WinJS.log && WinJS.log("Pin Changed Event Received", "sample", "status");
            }
        },

        /// <summary>
        /// </summary>
        onErrorReceivedEvent: function (eventArgs) {

            // If we navigated away from this page, we don't need to process this event
            // This also prevents output from spilling into another page
            if (!serialEvents.navigatedAway) {
                WinJS.log && WinJS.log("Error Received Event Received", "sample", "status");
            }
        },

        /// <summary>
        /// </summary>
        registerForPinChangedEvents: function () {
            if (!this.isRegisteredForPinChangedEvents) {
                this.pinChangedEventHandler = this.onPinChangedEvent;

                // Remember which device we are registering the device with, in case there is a device disconnect and reconnect. We want to avoid unregistering
                // an event handler that is not registered with the device object.
                // Ideally, one should remove the event token (e.g. assign to null) upon the device removal to avoid using it again.
                this._registeredDevice = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device;
                this._registeredDevice.addEventListener("pinchanged", this.pinChangedEventHandler, false);
                this.isRegisteredForPinChangedEvents = true;
                WinJS.log && WinJS.log("Pin Changed Event Registered", "sample", "status");
            }
        },

        /// <summary>
        /// </summary>
        unregisterForPinChangedEvents: function () {
            if (this.isRegisteredForPinChangedEvents) {
                // Don't unregister event if the device was removed and reconnected because the PinChanged event no longer contains our event handler
                if (this._registeredDevice === SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device) {
                    this._registeredDevice.removeEventListener("pinchanged", this.pinChangedEventHandler, false);
                    WinJS.log && WinJS.log("Pin Changed Event Unregistered", "sample", "status");
                }

                this._registeredDevice = null;
                this.isRegisteredForPinChangedEvents = false;
            }
        },

        /// <summary>
        /// </summary>
        registerForErrorReceivedEvents: function () {
            if (!this.isRegisteredForErrorReceivedEvents) {
                this.errorReceivedEventHandler = this.onErrorReceivedEvent;

                // Remember which device we are registering the device with, in case there is a device disconnect and reconnect. We want to avoid unregistering
                // an event handler that is not registered with the device object.
                // Ideally, one should remove the event token (e.g. assign to null) upon the device removal to avoid using it again.
                this._registeredDeviceForErrorReceived = SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device;
                this._registeredDeviceForErrorReceived.addEventListener("errorreceived", this.errorReceivedEventHandler, false);
                this.isRegisteredForErrorReceivedEvents = true;
                WinJS.log && WinJS.log("Error Received Event Registered", "sample", "status");
            }
        },

        /// <summary>
        /// </summary>
        unregisterForErrorReceivedEvents: function () {
            if (this.isRegisteredForErrorReceivedEvents) {
                // Don't unregister event if the device was removed and reconnected because the Error Received event no longer contains our event handler
                if (this._registeredDeviceForErrorReceived === SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.device) {
                    this._registeredDeviceForErrorReceived.removeEventListener("errorreceived", this.errorReceivedEventHandler, false);
                    WinJS.log && WinJS.log("Error Received Event Unregistered", "sample", "status");
                }

                this._registeredDeviceForErrorReceived = null;
                this.isRegisteredForErrorReceivedEvents = false;
            }
        },

        _onDeviceClosing: function (deviceInformation) {
            serialEvents.unregisterForPinChangedEvents();
            serialEvents.unregisterForErrorReceivedEvents();
        },
    },
    null);

    var serialEvents = new SerialEventsClass();

    var page = WinJS.UI.Pages.define("/html/scenario4_events.html", {

        ready: function (element, options) {
            serialEvents.navigatedAway = false;

            if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.isDeviceConnected == false) {
                document.getElementById("scenarioContent").hidden = true;
                WinJS.log && WinJS.log("Device is NOT connected", "sample", "status");
                return;
            }

            document.getElementById("PinChangedEventToggleSwitch").addEventListener("change", registerForPinChangedEventToggled, false);
            updatePinChangedView();

            document.getElementById("ErrorReceivedEventToggleSwitch").addEventListener("change", registerForErrorReceivedEventToggled, false);
            updateErrorReceivedView();
        }

    });

    function updatePinChangedView() {
        if (serialEvents.isRegisteredForPinChangedEvents) {
            document.getElementById("PinChangedEventToggleSwitch").winControl.checked = true;
            document.getElementById("PinChangedEventValue").textContent = "On";
        } else {
            document.getElementById("PinChangedEventToggleSwitch").winControl.checked = false;
            document.getElementById("PinChangedEventValue").textContent = "Off";
        }
    }

    function registerForPinChangedEventToggled(eventArgs) {
        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {

            if (document.getElementById("PinChangedEventToggleSwitch").winControl.checked) {
                serialEvents.registerForPinChangedEvents();
            } else {
                serialEvents.unregisterForPinChangedEvents();
            }
            updatePinChangedView();

        } else {

            SdkSample.CustomSerialDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function updateErrorReceivedView() {
        if (serialEvents.isRegisteredForErrorReceivedEvents) {
            document.getElementById("ErrorReceivedEventToggleSwitch").winControl.checked = true;
            document.getElementById("ErrorReceivedEventValue").textContent = "On";
        } else {
            document.getElementById("ErrorReceivedEventToggleSwitch").winControl.checked = false;
            document.getElementById("ErrorReceivedEventValue").textContent = "Off";
        }
    }

    function registerForErrorReceivedEventToggled(eventArgs) {
        if (SdkSample.CustomSerialDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {

            if (document.getElementById("ErrorReceivedEventToggleSwitch").winControl.checked) {
                serialEvents.registerForErrorReceivedEvents();
            } else {
                serialEvents.unregisterForErrorReceivedEvents();
            }
            updateErrorReceivedView();

        } else {

            SdkSample.CustomSerialDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

})();