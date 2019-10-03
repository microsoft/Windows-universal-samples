//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Will be defined when the page is initialized, otherwise we won't be able to find element
    var listOfDescriptorTypesControl = null;

    /// <summary>
    /// ListOfDescriptorTypesClass is used so that the UI can bind to the list of these entries and display them.
    /// </summary>
    var ListOfDescriptorTypesClass = WinJS.Class.define(function (descriptorTypeEntryName, descriptorType) {
        this._entryName = descriptorTypeEntryName;
        this._descriptorType = descriptorType;
    }, {
        _entryName: null,
        _descriptorType: null,
        entryName: {
            get: function () {
                return this._entryName;
            }
        },
        descriptorType: {
            get: function () {
                return this._descriptorType;
            }
        }
    },
    null);

    /// <summary>
    /// ListOfDescriptorTypesClass is used so that the UI can bind to the list of descriptors that can be displayed
    /// 
    /// This scenario can work for any device as long as it is "added" into the SdkSample.CustomUsbDeviceAccess. For more information on how to add a 
    /// device to make it compatible with this scenario, please see Scenario1_DeviceConnect.
    /// </summary>
    var UsbDescriptorsClass = WinJS.Class.define(function () {
        this._displayableDescriptorTypes.push(new ListOfDescriptorTypesClass("Device Descriptor", SdkSample.Constants.descriptor.deviceDescriptor));
        this._displayableDescriptorTypes.push(new ListOfDescriptorTypesClass("Configuration Descriptor", SdkSample.Constants.descriptor.configurationDescriptor));
        this._displayableDescriptorTypes.push(new ListOfDescriptorTypesClass("All Interface Descriptors", SdkSample.Constants.descriptor.interfaceDescriptor));
        this._displayableDescriptorTypes.push(new ListOfDescriptorTypesClass("All Endpoint Descriptors", SdkSample.Constants.descriptor.endpointDescriptor));
        this._displayableDescriptorTypes.push(new ListOfDescriptorTypesClass("All Custom Descriptors", SdkSample.Constants.descriptor.customDescriptor));

        // The Product string is not a string descriptors, they are just strings
        // It is marked as String Descriptor because string descriptors are not available in this API explicitly like descriptors are; however, you may use control
        // transfers to obtain them. The recommended approach, at least to get the Product string, is to use the Windows.Devices.Enumeration API.
        //
        // To get the Manufacturer, the Windows.Devices.Enumeration.Pnp API must be used. The usage of PnpObject is similar to that of the Enumeration API. When 
        // creating the PnpObject with one of it's static methods, add "System.Devices.Manufacturer" to the list of properties to include. Note that the PnpObjectType
        // must be of type DeviceContainer, otherwise the manufacturer name cannot be obtained.
        // Once the PnpObject for the device has been obtained, use PnpObject.Properties dictionary where the key is "System.Devices.Manufacturer" and the value is
        // the manufacturer name.
        this._displayableDescriptorTypes.push(new ListOfDescriptorTypesClass("Product String", SdkSample.Constants.descriptor.stringDescriptor));
    }, {
        _displayableDescriptorTypes: new WinJS.Binding.List(),
        displayableDescriptorTypes: {
            get: function () {
                return this._displayableDescriptorTypes;
            }
        },
        /// <summary>
        /// Obtains the stringified form of the descriptors of the chosen type and prints it.
        /// </summary>
        /// <param name="descriptorType"></param>
        printDescriptor: function (descriptorType) {
            var descriptor = null;

            switch (descriptorType) {
                case SdkSample.Constants.descriptor.deviceDescriptor:
                    descriptor = this.getDeviceDescriptorAsString();
                    break;
                case SdkSample.Constants.descriptor.configurationDescriptor:
                    descriptor = this.getConfigurationDescriptorAsString();
                    break;
                case SdkSample.Constants.descriptor.interfaceDescriptor:
                    descriptor = this.getInterfaceDescriptorsAsString();
                    break;
                case SdkSample.Constants.descriptor.endpointDescriptor:
                    descriptor = this.getEndpointDescriptorsAsString();
                    break;
                case SdkSample.Constants.descriptor.stringDescriptor:
                    descriptor = this.getProductName();
                    break;
                case SdkSample.Constants.descriptor.customDescriptor:
                    descriptor = this.getCustomDescriptorsAsString();
                    break;
            }

            if (descriptor) {
                WinJS.log && WinJS.log(descriptor, "sample", "status");
            }
        },
        getDeviceDescriptorAsString: function () {
            var content = null;

            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                var deviceDescriptor = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.deviceDescriptor;

                content = "Device Descriptor\n"
                    + "\nUsb Spec Number : 0x" + deviceDescriptor.bcdUsb.toString(16)
                    + "\nMax Packet Size (Endpoint 0) : " + deviceDescriptor.maxPacketSize0
                    + "\nVendor ID : 0x" + deviceDescriptor.vendorId.toString(16)
                    + "\nProduct ID : 0x" + deviceDescriptor.productId.toString(16)
                    + "\nDevice Revision : 0x" + deviceDescriptor.bcdDeviceRevision.toString(16)
                    + "\nNumber of Configurations : " + deviceDescriptor.numberOfConfigurations;
            }

            return content;
        },
        getConfigurationDescriptorAsString: function () {
            var content = null;

            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                var usbConfiguration = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.configuration;
                var configurationDescriptor = usbConfiguration.configurationDescriptor;

                content = "Configuration Descriptor\n"
                    + "\nNumber of Interfaces : " + usbConfiguration.usbInterfaces.size
                    + "\nConfiguration Value : 0x" + configurationDescriptor.configurationValue.toString(16)
                    + "\nSelf Powered : " + configurationDescriptor.selfPowered
                    + "\nRemote Wakeup : " + configurationDescriptor.remoteWakeup
                    + "\nMax Power (milliAmps) : " + configurationDescriptor.maxPowerMilliamps;
            }

            return content;
        },
        /// <summary>
        /// Will convert all the interfaces to a human readable format
        /// </summary>
        getInterfaceDescriptorsAsString: function () {
            var content = null;

            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                var interfaces = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.configuration.usbInterfaces;

                content = "Interface Descriptors";

                interfaces.forEach(function (usbInterface) {
                    // Will use class/subclass/protocol values from the first interface setting (usually the selected one)
                    var usbInterfaceDescriptor = usbInterface.interfaceSettings[0].interfaceDescriptor;

                    content += "\n\nInterface Number : 0x" + usbInterface.interfaceNumber.toString(16)
                        + "\nClass Code : 0x" + usbInterfaceDescriptor.classCode.toString(16)
                        + "\nSubclass Code : 0x" + usbInterfaceDescriptor.subclassCode.toString(16)
                        + "\nProtocol Code: 0x" + usbInterfaceDescriptor.protocolCode.toString(16)
                        + "\nNumber of Interface Settings : " + usbInterface.interfaceSettings.size
                        + "\nNumber of open Bulk In Endpoints : " + usbInterface.bulkInPipes.size
                        + "\nNumber of open Bulk Out Endpoints : " + usbInterface.bulkOutPipes.size
                        + "\nNumber of open Interrupt In Endpoints : " + usbInterface.interruptInPipes.size
                        + "\nNumber of open Interrupt Out Endpoints : " + usbInterface.interruptOutPipes.size;
                });

            }

            return content;
        },
        getEndpointDescriptorsAsString: function () {
            var content = null;

            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                var usbInterface = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.defaultInterface;
                var bulkInPipes = usbInterface.bulkInPipes;
                var bulkOutPipes = usbInterface.bulkOutPipes;
                var interruptInPipes = usbInterface.interruptInPipes;
                var interruptOutPipes = usbInterface.interruptOutPipes;

                content = "Endpoint Descriptors for open pipes";

                // Print Bulk In Endpoint descriptors
                bulkInPipes.forEach(function (bulkInPipe) {
                    var bulkInEndpointDescriptor = bulkInPipe.endpointDescriptor;

                    content += "\n\nBulk In Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + bulkInEndpointDescriptor.endpointNumber.toString(16)
                        + "\nMax Packet Size : " + bulkInEndpointDescriptor.maxPacketSize;
                });

                // Print Bulk Out Endpoint descriptors
                bulkOutPipes.forEach(function (bulkOutPipe) {
                    var bulkOutEndpointDescriptor = bulkOutPipe.endpointDescriptor;

                    content += "\n\nBulk Out Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + bulkOutEndpointDescriptor.endpointNumber.toString(16)
                        + "\nMax Packet Size : " + bulkOutEndpointDescriptor.maxPacketSize;
                });

                // Print Interrupt In Endpoint descriptors
                interruptInPipes.forEach(function (interruptInPipe) {
                    var interruptInEndpointDescriptor = interruptInPipe.endpointDescriptor;

                    content += "\n\nInterrupt In Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + interruptInEndpointDescriptor.endpointNumber.toString(16)
                        + "\nMax Packet Size : " + interruptInEndpointDescriptor.maxPacketSize
                        + "\nInterval(milliseconds) : " + interruptInEndpointDescriptor.interval;
                });

                // Print Interrupt Out Endpoint descriptors
                interruptOutPipes.forEach(function (interruptOutPipe) {
                    var interruptOutEndpointDescriptor = interruptOutPipe.endpointDescriptor;

                    content += "\n\nInterrupt Out Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + interruptOutEndpointDescriptor.endpointNumber.toString(16)
                        + "\nMax Packet Size : " + interruptOutEndpointDescriptor.maxPacketSize
                        + "\nInterval(milliseconds) : " + interruptOutEndpointDescriptor.interval;
                });
            }

            return content;
        },
        /// <summary>
        /// There are no custom descriptors for the SuperMutt and OSRFX2 devices, however, this method will show you how to
        /// navigate through raw descriptors. The raw descriptors can be found in UsbConfiguration,
        /// UsbInterface, and UsbInterfaceSetting. All possible descriptors that you would find in the full Configuration Descriptor
        /// are found under UsbConfiguration.
        /// All raw descriptors under UsbInterface are constrained only to those that are found under the InterfaceDescriptor
        /// that is defined in the Usb spec.
        ///
        /// Usb descriptor header (first 2 bytes):
        /// bLength             Size of descriptor in bytes
        /// bDescriptorType     The type of descriptor (configuration, interface, endpoint)
        /// </summary>
        getCustomDescriptorsAsString: function () {
            var content = null;

            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                // Descriptor information will be appended to this string and then printed to UI
                content = "Raw Descriptors";

                var configuration = SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.device.configuration;
                var allRawDescriptors = configuration.descriptors;

                // Print first 2 bytes of all descriptors within the configuration descriptor    
                // because the first 2 bytes are always length and descriptor type
                // The most efficient and recommended way to get the descriptor type and length is through
                // the UsbDescriptor's DescriptorType and Length properties, but we will not use these properties
                // in order to demonstrate ReadDescriptorBuffer() and how to parse it.
                allRawDescriptors.forEach(function (descriptor) {
                    var descriptorBuffer = new Windows.Storage.Streams.Buffer(descriptor.length);
                    descriptor.readDescriptorBuffer(descriptorBuffer);

                    var reader = Windows.Storage.Streams.DataReader.fromBuffer(descriptorBuffer);

                    // USB data is Little Endian according to the USB spec.
                    reader.byteOrder = Windows.Storage.Streams.ByteOrder.littleEndian;

                    // ReadByte has a side effect where it consumes the current byte, so the next ReadByte will read the next character.
                    // Putting multiple ReadByte() on the same line (same variable assignment) may cause the bytes to be read out of order.
                    var descriptorLength = reader.readByte();
                    var descriptorType = "0x" + reader.readByte().toString(16);

                    content += "\n\nDescriptor"
                        + "\nLength : " + descriptorLength
                        + "\nDescriptorType : " + descriptorType;
                });
            }

            return content;
        },
        /// <summary>
        /// It is recommended to obtain the product string by using DeviceInformation::Name.
        /// </summary>
        getProductName: function () {
            var content = null;

            if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
                content = "Product name: " + SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.deviceInformation.name;
            }

            return content;
        },
    },
    null);

    var usbDescriptors = new UsbDescriptorsClass();

    // Bind the list of devices to the UI
    window.listOfDescriptorTypes = usbDescriptors.displayableDescriptorTypes;

    var page = WinJS.UI.Pages.define("/html/scenario5_usbDescriptors.html", {
        ready: function (element, options) {
            listOfDescriptorTypesControl = document.getElementById("listOfDescriptorTypesView").winControl;

            // Set up Button listeners before hiding scenarios in case the button is removed when hiding scenario
            listOfDescriptorTypesControl.addEventListener("iteminvoked", descriptorItemInvoked, false);

            // Available scenarios
            var deviceScenarios = {};
            deviceScenarios[SdkSample.Constants.deviceType.all] = document.querySelector(".generalScenario");

            SdkSample.CustomUsbDeviceAccess.utilities.setUpDeviceScenarios(deviceScenarios, document.querySelector(".deviceScenarioContainer"));
        }
    });

    /// <summary>
    /// When an entry is selected from the list of available descriptors, we will print out the contents of the descriptor.
    /// </summary>
    /// <param name="eventArgs"></param>
    function descriptorItemInvoked(eventArgs) {
        if (SdkSample.CustomUsbDeviceAccess.eventHandlerForDevice.current.isDeviceConnected) {
            var itemIndex = eventArgs.detail.itemIndex;
            var descriptorEntry = usbDescriptors.displayableDescriptorTypes.getAt(itemIndex);

            // Select the item on the UI
            listOfDescriptorTypesControl.selection.set(itemIndex);

            usbDescriptors.printDescriptor(descriptorEntry.descriptorType);
        } else {
            SdkSample.CustomUsbDeviceAccess.utilities.notifyDeviceNotConnected();
        }
    }
})();
