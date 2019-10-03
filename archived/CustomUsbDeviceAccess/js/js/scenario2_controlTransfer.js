//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ControlTransferClass = WinJS.Class.define(null, {
        /// <summary>
        /// Sets the seven segment display on the OSRFX2 device via control transfer
        /// 
        /// Before sending the data through the control transfer, the numeric value is converted into a hex value that is
        /// bit masked. Different sections of the bit mask will turn on a different LEDs. Please refer to the OSRFX2 spec
        /// to see the hex values per LED.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        ///
        /// Setting Seven Segment LED require setup packet:
        /// bmRequestType:   type:       VENDOR
        ///                  recipient:  DEVICE
        /// bRequest:        0xDB
        /// wLength:         1
        /// </summary>
        /// <param name="numericValue"></param>
        /// <returns>An async that can be used to chain more methods after completing the scenario</returns>
        setOsrFx2SevenSegmentAsync: function (numericValue) {
            var writer = new Windows.Storage.Streams.DataWriter();

            // Convert the numeric value into a 7 segment LED hex value and write it to a buffer.
            writer.writeByte(SdkSample.Constants.osrFx2.sevenLedSegmentMask[numericValue]);

            // The buffer with the data
            var bufferToSend = writer.detachBuffer();

            var setupPacket = new Windows.Devices.Usb.UsbSetupPacket();
            setupPacket.requestType.recipient = Windows.Devices.Usb.UsbControlRecipient.device;
            setupPacket.requestType.controlTransferType = Windows.Devices.Usb.UsbControlTransferType.vendor;
            setupPacket.request = SdkSample.Constants.osrFx2.vendorCommand.setSevenSegment;
            setupPacket.value = 0;
            setupPacket.length = bufferToSend.length;

            return SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.sendControlOutTransferAsync(setupPacket, bufferToSend)
                .then(function (bytesTransferred) {
                    // Make sure we sent the correct number of bytes
                    if (bytesTransferred === bufferToSend.length) {
                        WinJS.log && WinJS.log("The segment display value is set to " + numericValue, "sample", "status");
                    } else {
                        WinJS.log && WinJS.log("Error sending data. Sent bytes: " + bytesTransferred
                            + "; Tried to send : " + bufferToSend.length, "sample", "error");
                    }
                });
        },
        /// <summary>
        /// Gets the current value of the seven segment display on the OSRFX2 device via control transfer.
        ///
        /// The Buffer is used to hold data that is meant to be sent over during the data phase.
        /// The easiest way to write data to an IBuffer is to use a DataWriter. The DataWriter, when instantiated, 
        /// creates a buffer internally. The buffer is of type IBuffer and can be detached from the writer, which gives us
        /// the internal IBuffer.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        ///
        /// When the seven segment hex value is received, we attempt to match it with a known (has a numerical value 0-9) hex value.
        ///
        /// Getting Seven Segment LED require setup packet:
        /// bmRequestType:   type:       VENDOR
        ///                  recipient:  DEVICE
        /// bRequest:        0xD4
        /// wLength:         1
        /// </summary>
        /// <returns>An async method that can be used to chain more methods after completing the scenario</returns>
        getOsrFx2SevenSegmentAsync: function () {
            // We expect to receive 1 byte of data with our control transfer, which is the state of the seven segment
            var receiveDataSize = 1;

            return this.sendVendorControlTransferInToDeviceRecipientAsync(SdkSample.Constants.osrFx2.vendorCommand.getSevenSegment, receiveDataSize)
                .then(function (dataBuffer) {
                    if (dataBuffer.length === receiveDataSize) {
                        // The dataBuffer contains the data sent by the device to us
                        var dataReader = Windows.Storage.Streams.DataReader.fromBuffer(dataBuffer);

                        // The raw hex value representing the seven segment display
                        var rawValue = dataReader.readByte();

                        // The LED value that the raw hex value represents
                        var readableValue = null;

                        // Find which numeric value has the segment hex value
                        for (var i = 0, numSegmentMasks = SdkSample.Constants.osrFx2.sevenLedSegmentMask.length; i < numSegmentMasks; i++) {
                            if (rawValue === SdkSample.Constants.osrFx2.sevenLedSegmentMask[i]) {
                                readableValue = i;

                                break;
                            }
                        }

                        if (readableValue !== null) {
                            WinJS.log && WinJS.log("The segment display value is " + readableValue, "sample", "status");
                        } else {
                            WinJS.log && WinJS.log("The segment display is not yet initialized", "sample", "status");
                        }
                    } else {
                        WinJS.log && WinJS.log("Expected to read " + receiveDataSize + " bytes, but got " + dataBuffer.length, "sample", "error");
                    }
                });
        },
        /// <summary>
        /// Initiates a control transfer to set the blink pattern on the SuperMutt's LED
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        ///
        /// Setting LED blink pattern require setup packet:
        /// bmRequestType:   type:       VENDOR
        ///                  recipient:  DEVICE
        /// bRequest:        0x03
        /// wValue:          0-7 (any number in that range, inclusive)
        /// wLength:         0
        ///
        /// The SuperMutt has the following patterns:
        /// 0 - LED always on
        /// 1 - LED flash 2 seconds on, 2 off, repeat
        /// 2 - LED flash 2 seconds on, 1 off, 2 on, 4 off, repeat
        /// ...
        /// 7 - 7 iterations of 2 on, 1 off, followed by 4 off, repeat
        /// </summary>
        /// <param name="pattern">A number from 0-7. Each number represents a different blinking pattern</param>
        /// <returns>An async that can be used to chain more methods after completing the scenario</returns>
        setSuperMuttLedBlinkPatternAsync: function (pattern) {
            var setupPacket = new Windows.Devices.Usb.UsbSetupPacket();
            setupPacket.requestType.recipient = Windows.Devices.Usb.UsbControlRecipient.device;
            setupPacket.requestType.controlTransferType = Windows.Devices.Usb.UsbControlTransferType.vendor;
            setupPacket.request = SdkSample.Constants.superMutt.vendorCommand.setLedBlinkPattern;
            setupPacket.value = pattern;
            setupPacket.length = 0;

            return SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.sendControlOutTransferAsync(setupPacket)
                .then(function (bytesTransferred) {
                    WinJS.log && WinJS.log("The Led blink pattern is set to " + pattern, "sample", "status");
                });
        },
        /// <summary>
        /// Sets up a UsbSetupPacket that will let the device know that we are trying to get the Led blink pattern on the SuperMutt via
        /// control transfer.
        /// 
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        ///
        /// The simplest way to obtain a byte from the buffer is by using a DataReader. DataReader provides a simple way
        /// to read from buffers (e.g. can return bytes, strings, ints).
        ///
        /// Do note the method that is used to send a control transfer. There are two methods to send a control transfer. 
        /// One is to send data and the other is to get data.
        /// </summary>
        /// <returns>An async that can be used to chain more methods after completing the scenario</returns>
        getSuperMuttLedBlinkPatternAsync: function () {
            // We expect to receive 1 byte of data with our control transfer, which is the led pattern
            var receiveDataSize = 1;

            // The blink pattern is 1 byte long, so we only need to retrieve 1 byte of data
            return this.sendVendorControlTransferInToDeviceRecipientAsync(SdkSample.Constants.superMutt.vendorCommand.getLedBlinkPattern, receiveDataSize)
                .then(function (dataBuffer) {
                    if (dataBuffer.length === receiveDataSize) {
                        // Easiest way to read from a buffer
                        var dataReader = Windows.Storage.Streams.DataReader.fromBuffer(dataBuffer);

                        var pattern = dataReader.readByte();

                        WinJS.log && WinJS.log("The Led blink pattern is " + pattern, "sample", "status");
                    } else {
                        WinJS.log && WinJS.log("Expected to read " + receiveDataSize + " bytes, but got " + dataBuffer.length, "sample", "error");
                    }
                });
        },
        /// <summary>
        /// Sets up a UsbSetupPacket and sends control transfer that will let the device know that we are trying to retrieve data from the device.
        /// This method only supports vendor commands because the scenario only uses vendor commands.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be 
        /// handled at the end of the task chain.
        ///
        /// Do note the method that is used to send a control transfer. There are two methods to send a control transfer. 
        /// One involves receiving buffer data in the Data stage of the control transfer, and the other involves transmiting the buffer data.
        /// 
        /// The simplest way to obtain a byte from the buffer is by using a DataReader. DataReader provides a simple way
        /// to read from buffers (e.g. can return bytes, strings, ints).
        /// </summary>
        /// <param name="vendorCommand">Command to put into SetupPacket's Request property</param>
        /// <param name="dataPacketLength">Number of bytes in the data packet that is sent in the Data Stage</param>
        /// <returns>An async method that can be used to chain more methods after completing the scenario</returns>
        sendVendorControlTransferInToDeviceRecipientAsync: function (vendorCommand, dataPacketLength) {
            // Data will be written to this buffer when we receive it
            var buffer = new Windows.Storage.Streams.Buffer(dataPacketLength);

            var setupPacket = new Windows.Devices.Usb.UsbSetupPacket();
            setupPacket.requestType.recipient = Windows.Devices.Usb.UsbControlRecipient.device;
            setupPacket.requestType.controlTransferType = Windows.Devices.Usb.UsbControlTransferType.vendor;
            setupPacket.request = vendorCommand;
            setupPacket.length = dataPacketLength;

            return SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.sendControlInTransferAsync(setupPacket, buffer);
        }
    },
    null);

    var controlTransfer = new ControlTransferClass();

    var page = WinJS.UI.Pages.define("/html/scenario2_controlTransfer.html", {
        ready: function (element, options) {
            // Set up Button listeners before hiding scenarios in case the button is removed when hiding scenario
            document.getElementById("buttonGetOsrFx2SevenSegment").addEventListener("click", getOsrFx2SevenSegmentSettingClick, false);
            document.getElementById("buttonSetOsrFx2SevenSegment").addEventListener("click", setOsrFx2SevenSegmentSettingClick, false);
            document.getElementById("buttonGetSuperMuttLedBlinkPattern").addEventListener("click", getSuperMuttLedBlinkPatternClick, false);
            document.getElementById("buttonSetSuperMuttLedBlinkPattern").addEventListener("click", setSuperMuttLedBlinkPatternClick, false);

            // Available scenarios
            var deviceScenarios = {};
            deviceScenarios[SdkSample.Constants.deviceType.osrFx2] = document.querySelector(".osrFx2Scenario");
            deviceScenarios[SdkSample.Constants.deviceType.superMutt] = document.querySelector(".superMuttScenario");

            SdkSample.CustomUsbDeviceAccess.utilities.setUpDeviceScenarios(deviceScenarios, document.querySelector(".deviceScenarioContainer"));
        }
    });

    function getOsrFx2SevenSegmentSettingClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            window.buttonGetOsrFx2SevenSegment.disabled = true;

            // Re-enable get button after completing the get
            controlTransfer.getOsrFx2SevenSegmentAsync().done(function () {
                window.buttonGetOsrFx2SevenSegment.disabled = false;
            });
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function setOsrFx2SevenSegmentSettingClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            window.buttonSetOsrFx2SevenSegment.disabled = true;

            // Re-enable set button after completing the set
            controlTransfer.setOsrFx2SevenSegmentAsync(window.osrFx2SevenSegmentSettingInput.selectedIndex).done(function () {
                window.buttonSetOsrFx2SevenSegment.disabled = false;
            });
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function getSuperMuttLedBlinkPatternClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            window.buttonGetSuperMuttLedBlinkPattern.disabled = true;

            // Re-enable get button after completing the get
            controlTransfer.getSuperMuttLedBlinkPatternAsync().done(function () {
                window.buttonGetSuperMuttLedBlinkPattern.disabled = false;
            });
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }

    function setSuperMuttLedBlinkPatternClick(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            window.buttonSetSuperMuttLedBlinkPattern.disabled = true;

            // Re-enable set button after completing the set
            controlTransfer.setSuperMuttLedBlinkPatternAsync(window.ledBlinkPatternInput.selectedIndex).done(function () {
                window.buttonSetSuperMuttLedBlinkPattern.disabled = false;
            });
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }
})();
