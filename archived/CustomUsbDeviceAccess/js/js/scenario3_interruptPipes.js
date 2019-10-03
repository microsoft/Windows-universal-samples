//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var InterruptPipesClass = WinJS.Class.define(null, {
        _registeredInterruptPipeIndex: 0, // Pipe index of the pipe we that we registered for. Only valid if registeredInterrupte is true
        // Indicate if we navigate away from this page or not.
        navigatedAway: false,
        numInterruptsReceived: 0,
        totalNumberBytesReceived: 0,
        totalNumberBytesWritten: 0,
        registeredInterrupt: false,
        dataReceivedEventHandler: null,
        previousSwitchStates: new Array(8),     // 8 switches on the OSRFX2
        /// <summary>
        /// Register for the interrupt that is triggered when the device sends an interrupt to us (e.g. OSRFX2's device's switch state changes
        /// or SuperMUTT's software generated interrupts).
        /// The DefaultInterface on the the device is the first interface on the device. We navigate to
        /// the InterruptInPipes because that collection contains all the interrupt in pipes for the
        /// selected interface setting.
        ///
        /// Each pipe has a property that links to an EndpointDescriptor. This descriptor can be used to find information about
        /// the pipe (e.g. id and interval).
        ///
        /// The function must save the event token so that we can unregister from the even later on.
        /// </summary>
        /// <param name="pipeIndex">The index of the pipe found in UsbInterface::InterruptInPipes. It is not the endpoint number</param>
        /// <param name="eventHandler">Event handler that will be called each time the event is raised</param>
        registerForInterruptEvent: function (pipeIndex, eventHandler) {
            var interruptInPipes = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.interruptInPipes;

            if (!this.registeredInterrupt && (pipeIndex < interruptInPipes.length)) {
                // Search for the correct pipe that has the specified endpoint number
                var interruptInPipe = interruptInPipes.getAt(pipeIndex);

                interruptInPipe.addEventListener("datareceived", eventHandler, false);

                this._registeredInterruptPipeIndex = pipeIndex;

                this.registeredInterrupt = true;
                this.dataReceivedEventHandler = eventHandler;

                this.numInterruptsReceived = 0;
                this.totalNumberBytesReceived = 0;
            }
        },
        /// <summary>
        /// Unregisters from the interrupt event that was registered for in the RegisterForInterruptEvent();
        /// </summary>
        unregisterForInterruptEvent: function () {
            if (this.registeredInterrupt) {
                // Search for the correct pipe that we know we used to register events
                var interruptInPipe = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.interruptInPipes.getAt(this._registeredInterruptPipeIndex);

                interruptInPipe.removeEventListener("datareceived", this.dataReceivedEventHandler, false);

                this.registeredInterrupt = false;
                this.dataReceivedEventHandler = null;

                updateRegisterEventButton();
                clearSwitchStateTable();
            }
        },
        /// <summary>
        /// Will write garbage data to the specified output pipe
        /// </summary>
        /// <param name="pipeIndex">Index of pipe in the list of Device->DefaultInterface->InterruptOutPipes</param>
        /// <param name="bytesToWrite">Bytes of garbage data to write</param>
        writeToInterruptOut: function (pipeIndex, bytesToWrite) {
            var interruptOutPipes = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.interruptOutPipes;

            if (pipeIndex < interruptOutPipes.length) {
                // Create an array, all default initialized to 0, and write it to the buffer
                // The data inside the buffer will be garbage
                var arrayBuffer = new Array(bytesToWrite);

                var pipe = interruptOutPipes.getAt(pipeIndex);

                var pipeWriter = new Windows.Storage.Streams.DataWriter(pipe.outputStream);
                pipeWriter.writeBytes(arrayBuffer);

                // This is where the data is flushed out to the device.
                pipeWriter.storeAsync().done(function (bytesWritten) {
                    interruptPipes.totalNumberBytesWritten += bytesWritten;

                    WinJS.log && WinJS.log("Total Bytes written: " + interruptPipes.totalNumberBytesWritten, "sample", "status");
                });
            } else {
                WinJS.log && WinJS.log("Pipe index provided (" + pipeIndex + ") is out of range", "sample", "error");
            }
        },
        /// <summary>
        /// This method is called whenever the device sends an interrupt on the pipe we registered this callback on.
        ///
        /// We will read a byte from the buffer that the device sent to us and then look at each bit to determine state of
        /// each switch. AFter determining the state of each switch, we will print out a table with each row representing a 
        /// switch and its state.
        /// </summary>
        /// <param name="eventArgs">Contains the data, in an IBuffer, that was received through the interrupts</param> 
        onOsrFx2SwitchStateChangeEvent: function (eventArgs) {
            // If we navigated away from this page, we don't need to process this event
            // This also prevents output from spilling into another page
            if (!interruptPipes.navigatedAway) {
                interruptPipes.numInterruptsReceived++;

                // The OSRFX2 gives us 1 byte, each bit representing the state of a switch
                var numberOfSwitches = 8;

                var switchStateArray = new Array(numberOfSwitches);

                var buffer = eventArgs.interruptData;

                if (buffer.length > 0) {
                    interruptPipes.totalNumberBytesReceived += buffer.length;

                    var reader = Windows.Storage.Streams.DataReader.fromBuffer(buffer);

                    var newSwitchStates = reader.readByte();

                    // Loop through each bit of what the device sent us and determine the state of each switch
                    for (var switchIndex = 0; switchIndex < numberOfSwitches; switchIndex++) {
                        var result = (1 << switchIndex) & newSwitchStates;

                        if (result) {
                            switchStateArray[switchIndex] = true;
                        } else {
                            switchStateArray[switchIndex] = false;
                        }
                    }

                    // Close the reader explicitly to free resources and not have to wait till the GC deletes the reader because interrupts can occur very often
                    reader.close();

                    WinJS.log && WinJS.log("Total number of interrupt events received: " + interruptPipes.numInterruptsReceived
                        + "\nTotal number of bytes received: " + interruptPipes.totalNumberBytesReceived, "sample", "status");

                    updateSwitchStateTable(interruptPipes.previousSwitchStates, switchStateArray);

                    // Save the new state as the previous one so we can compare it with future changes
                    interruptPipes.previousSwitchStates = switchStateArray;
                } else {
                    WinJS.log && WinJS.log("Received 0 bytes from interrupt", "sample", "error");
                }
            }
        },
        /// <summary>
        /// This callback only increments the total number of interrupts received and prints it
        ///
        /// This method is called whenever the device sends an interrupt on the pipe we registered this callback on.
        /// </summary>
        /// <param name="eventArgs">Contains the data, in an IBuffer, that was received through the interrupts</param> 
        onGeneralInterruptEvent: function (eventArgs) {
            // If we navigated away from this page, we don't need to process this event
            // This also prevents output from spilling into another page
            if (!interruptPipes.navigatedAway) {
                interruptPipes.numInterruptsReceived++;

                // The data from the interrupt
                var buffer = eventArgs.interruptData;

                interruptPipes.totalNumberBytesReceived += buffer.length;

                WinJS.log && WinJS.log("Total number of interrupt events received: " + interruptPipes.numInterruptsReceived
                    + "\nTotal number of bytes received: " + interruptPipes.totalNumberBytesReceived, "sample", "status");
            }
        },
        onDeviceClosing: function (deviceInformation) {
            interruptPipes.unregisterForInterruptEvent();
        },
        /// <summary>
        /// Clears and populates the provided div with text, where each line is:
        /// [row #]    [state of new value]
        ///
        /// If the state of a value has changed, the value will be bolded.
        /// </summary>
        /// <param name="outputDiv">The div will be cleared and new rows will populate this table. 
        ///     The contents of the table will be modified by this method</param>
        /// <param name="newValues">The new boolean values</param>
        /// <param name="oldValues">The boolean values that the new ones are being compared to</param>
        /// <param name="trueValue">Text if the new value is TRUE</param>
        /// <param name="falseValue">Text if the new vlaue is FALSE</param>
        createBooleanChartInDiv: function (outputDiv, newValues, oldValues, trueValue, falseValue) {
            var isBooleanChartCreated = false;

            // Make sure there are at least newValues to check or that there are the same number of old values as new values
            if (newValues && (!oldValues || (newValues.length === oldValues.length))) {
                outputDiv.innerText = "";

                newValues.forEach(function (newValue, index) {
                    var line = document.createElement("div");
                    var lineText = "";

                    // Row #
                    lineText = index + 1;

                    // Space between row # and the value (simulates a column)
                    lineText += "    ";

                    // Print the textual form of TRUE/FALSE values
                    lineText += newValue ? trueValue : falseValue;

                    // Bold values that have changed
                    if (oldValues && (oldValues[index] !== newValue)) {
                        var bold = document.createElement("b");
                        bold.innerText = lineText;
                        line.appendChild(bold);
                    } else {
                        line.innerText = lineText;
                    }

                    outputDiv.appendChild(line);
                });

                isBooleanChartCreated = true;
            }

            return isBooleanChartCreated;
        }
    },
    null);

    var interruptPipes = new InterruptPipesClass();

    var page = WinJS.UI.Pages.define("/html/scenario3_interruptPipes.html", {
        ready: function (element, options) {
            interruptPipes.navigatedAway = false;

            interruptPipes.totalNumberBytesWritten = 0;

            // Set up Button listeners before hiding scenarios in case the button is removed when hiding scenario
            document.getElementById("buttonRegisterOsrFx2InterruptEvent").addEventListener("click", registerOsrFx2InterruptEventClick, false);
            document.getElementById("buttonRegisterSuperMuttInterruptEvent").addEventListener("click", registerSuperMuttInterruptEventClick, false);
            document.getElementById("buttonUnregisterInterruptEvent").addEventListener("click", unregisterInterruptEventClick, false);
            document.getElementById("buttonWriteInterruptOut").addEventListener("click", writeSuperMuttInterruptOutClick, false);

            // Available scenarios
            var deviceScenarios = {};
            deviceScenarios[SdkSample.Constants.deviceType.osrFx2] = document.querySelector(".osrFx2Scenario");
            deviceScenarios[SdkSample.Constants.deviceType.superMutt] = document.querySelector(".superMuttScenario");

            SdkSample.CustomUsbDeviceAccess.utilities.setUpDeviceScenarios(deviceScenarios, document.querySelector(".deviceScenarioContainer"));

            // Get notified when the device is about to be closed so we can unregister events
            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.onDeviceCloseCallback = interruptPipes.onDeviceClosing;

            clearSwitchStateTable();
            updateRegisterEventButton();
        },
        unload: function () {
            interruptPipes.navigatedAway = true;

            // Unregister events
            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                interruptPipes.unregisterForInterruptEvent();
            }

            // We are leaving and no longer care about the device closing
            SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.onDeviceCloseCallback = null;
        }
    });

    /// <summary>
    /// This method will register for events on the correct interrupt in pipe index on the OsrFx2 device.
    /// </summary>
    /// <param name="eventArgs"></param>
    function registerOsrFx2InterruptEventClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            var interruptPipeIndex = SdkSample.Constants.osrFx2.pipe.interruptInPipeIndex;
            var interruptEventHandler = interruptPipes.onOsrFx2SwitchStateChangeEvent;

            interruptPipes.registerForInterruptEvent(interruptPipeIndex, interruptEventHandler);

            updateRegisterEventButton();
            clearSwitchStateTable();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    /// <summary>
    /// This method will register for events on the correct interrupt in pipe index on the OsrFx2 device.
    /// </summary>
    /// <param name="eventArgs"></param>
    function registerSuperMuttInterruptEventClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            var interruptPipeIndex = SdkSample.Constants.superMutt.pipe.interruptInPipeIndex;
            var interruptEventHandler = interruptPipes.onGeneralInterruptEvent;

            interruptPipes.registerForInterruptEvent(interruptPipeIndex, interruptEventHandler);

            updateRegisterEventButton();
            clearSwitchStateTable();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function unregisterInterruptEventClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            interruptPipes.unregisterForInterruptEvent();
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    /// <summary>
    /// This scenario is only available on the SuperMutt device because the OSRFX2 doesn't have Out Interrupt Endpoints.
    /// 
    /// This method will determine the right InterruptOutPipe to use and will call another method to do the actual writing.
    /// </summary>
    /// <param name="eventArgs"></param>
    function writeSuperMuttInterruptOutClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected
            && (SdkSample.CustomUsbDeviceAccess.utilities.isSuperMuttDevice(SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device))) {
            var outPipeIndex = SdkSample.Constants.superMutt.pipe.interruptOutPipeIndex;

            // We will write the maximum number of bytes we can per interrupt
            var interruptOutPipes = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface.interruptOutPipes;

            if (outPipeIndex < interruptOutPipes.length) {
                var bytesToWrite = interruptOutPipes.getAt(outPipeIndex).endpointDescriptor.maxPacketSize;

                interruptPipes.writeToInterruptOut(outPipeIndex, bytesToWrite);
            } else {
                WinJS.log && WinJS.log("Pipe index provided (" + outPipeIndex + ") is out of range", "sample", "error");
            }

        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function updateRegisterEventButton() {
        if (window.buttonRegisterOsrFx2InterruptEvent) {
            window.buttonRegisterOsrFx2InterruptEvent.disabled = interruptPipes.registeredInterrupt;
        }

        if (window.buttonRegisterSuperMuttInterruptEvent) {
            window.buttonRegisterSuperMuttInterruptEvent.disabled = interruptPipes.registeredInterrupt;
        }

        if (window.buttonUnregisterInterruptEvent) {
            window.buttonUnregisterInterruptEvent.disabled = !interruptPipes.registeredInterrupt;
        }
    }

    function clearSwitchStateTable() {
        document.getElementById("switchStates").innerHTML = "";
    }

    /// <summary>
    /// Prints a table in the UI representing the current state of each of the switches on the OSRFX2 board.
    /// The bolded states/switches are the switches that have their states changed.
    ///
    /// Note that the OSRFX2 board has the switch states reversed (1 is off and 0 is on)
    /// </summary>
    /// <param name="oldStates">States of previous interrupt. Switch states where 0 is off and 1 is on</param>
    /// <param name="newStates">Switch states where 0 is off and 1 is on</param>
    function updateSwitchStateTable(oldStates, newStates) {
        var switchStatesDiv = document.getElementById("switchStates");

        // The table will be written directly to the UI's textbox
        interruptPipes.createBooleanChartInDiv(switchStatesDiv, newStates, oldStates, "off", "on");
    }
})();
