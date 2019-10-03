//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Has the user enabled switch change events?
    var currentPromise = null;

    // The previous switch values - saved to allow the switch changes to be
    // bolded.
    var previousSwitchValues = null;

    var page = WinJS.UI.Pages.define("/html/scenario4_deviceEvents.html", {
        ready: function (element, options) {
            document.getElementById("device-events-get").addEventListener("click", onGetSwitchState, false);
            document.getElementById("device-events-begin").addEventListener("click", onDeviceEventsBegin, false);
            document.getElementById("device-events-cancel").addEventListener("click", onDeviceEventsCancel, false);
        },
        processed: function () {
            clearSwitchStateTable();
            updateRegisterButton();
        },
        unload: function () {
            if (currentPromise) {
                currentPromise.cancel();
            }
        }
    });

    function updateRegisterButton() {
        var startButton = document.getElementById("device-events-begin");
        var cancelButton = document.getElementById("device-events-cancel");

        if (!startButton || !cancelButton) {
            return;
        }

        if (currentPromise) {
            startButton.setAttribute("disabled", currentPromise ? "disabled" : "");
            cancelButton.removeAttribute("disabled");
        } else {
            startButton.removeAttribute("disabled");
            cancelButton.setAttribute("disabled", !currentPromise ? "disabled" : "");
        }
    }

    // Gets the current switch state of the Fx2 Device dip switches
    function onGetSwitchState() {
        var fx2Device = DeviceList.getSelectedFx2Device();

        if (!fx2Device) {
            WinJS.log && WinJS.log("Fx2 device not connected or accessible", "sample", "error");
            return;
        }

        var outputBuffer = new Windows.Storage.Streams.Buffer(1);

        fx2Device.sendIOControlAsync(
            Fx2Driver.readSwitches,
            null,
            outputBuffer
            ).
        done(
            function (bytesRead) {

                var reader = Windows.Storage.Streams.DataReader.fromBuffer(outputBuffer);
                var switchState = reader.readByte();

                var switchStateArray = createSwitchStateArray(switchState);

                updateSwitchStateTable(switchStateArray);
            },

            function (error) {
                WinJS.log && WinJS.log(error, "sample", "error");
            }
        );
    }

    function onDeviceEventsBegin() {
        var fx2Device = DeviceList.getSelectedFx2Device();

        if (!fx2Device) {
            WinJS.log && WinJS.log("Fx2 device not connected or accessible", "sample", "error");
            return;
        }

        if (!currentPromise) {
            startInterruptMessageWorker(fx2Device);
        }

        updateRegisterButton();
    }

    function onDeviceEventsCancel() {
        var fx2Device = DeviceList.getSelectedFx2Device();

        if (!fx2Device) {
            WinJS.log && WinJS.log("Fx2 device not connected or accessible", "sample", "error");
            return;
        }

        if (currentPromise) {
            currentPromise.cancel();
        }

        updateRegisterButton();
    }

    function startInterruptMessageWorker(fx2Device) {

        var buffer = new Windows.Storage.Streams.Buffer(1);

        interruptMessageWorker(fx2Device, buffer);
    }

    function interruptMessageWorker(fx2Device, switchMessageBuffer) {
        currentPromise = fx2Device.sendIOControlAsync(
                                    Fx2Driver.getInterruptMessage,
                                    null,
                                    switchMessageBuffer
                                    ).
        then(
            function (bytesRead) {
                if (bytesRead === 0) {
                    throw new WinJS.ErrorFromName("NoData");
                }

                var reader = Windows.Storage.Streams.DataReader.fromBuffer(switchMessageBuffer);
                var switchState = reader.readByte();
                var switchStateArray = createSwitchStateArray(switchState);

                updateSwitchStateTable(switchStateArray);

                // Start the next iteration of the worker.
                interruptMessageWorker(fx2Device, switchMessageBuffer);
            },

            function (error) {
                /// <params name="error" type="Error"/>
                if (error.name === "NoData") {
                    WinJS.log && WinJS.log("Fx2 device returned 0 byte interrupt message.  Stopping",
                       "sample",
                       "error");
                }
                else if (error.name === "Canceled") {
                    WinJS.log && WinJS.log("Pending getInterruptMessage IO control cancelled", "sample", "status");
                }
                else {
                    WinJS.log && WinJS.log("Error accessing Fx2 device: " + error, "sample", "error");
                }

                currentPromise = null;

                clearSwitchStateTable();
                updateRegisterButton();
            }
        );
    }

    function createSwitchStateArray(value) {

        var switchStateArray = new Array(8);

        for (var i = 0; i < switchStateArray.length; i += 1) {
            switchStateArray[i] = (value & (1 << i)) !== 0;
        }

        return switchStateArray;
    }


    function clearSwitchStateTable() {
        var table = document.getElementById("device-events-switches");

        if (table) {
            table.innerHTML = "";
        }
    }

    function updateSwitchStateTable(switchStateArray) {

        var table = DeviceList.createBooleanTable(
                                           switchStateArray,
                                           previousSwitchValues,
                                           {
                                               indexTitle: "Switch Number",
                                               valueTitle: "Switch State",
                                               trueValue: "off",
                                               falseValue: "on"
                                           }
                                        );
        var output = document.getElementById("device-events-switches");
        if (output.children.length > 0) {
            output.removeChild(output.children[0]);
        }
        output.insertBefore(table);

        previousSwitchValues = switchStateArray;
    }
})();
