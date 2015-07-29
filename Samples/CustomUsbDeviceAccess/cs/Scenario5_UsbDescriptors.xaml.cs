//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
using Windows.Devices.Usb;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomUsbDeviceAccess
{
    /// <summary>
    /// DescriptorTypeEntry is used so that the UI can bind to the list of these entries and display them.
    /// </summary>
    public sealed class DescriptorTypeEntry
    {
        private String entryName;
        private Descriptor descriptorType;

        public String EntryName
        {
            get
            {
                return entryName;
            }
        }

        public Descriptor DescriptorType
        {
            get
            {
                return descriptorType;
            }
        }

        public DescriptorTypeEntry(String name, Descriptor type)
        {
            entryName = name;
            descriptorType = type;
        }
    };

    /// <summary>
    /// This scenario demonstrates how get various descriptors from UsbDevice. 
    /// 
    /// This scenario can work for any device as long as it is "added" into the sample. For more information on how to add a 
    /// device to make it compatible with this scenario, please see Scenario1_DeviceConnect.
    /// </summary>
    public sealed partial class Scenario5_UsbDescriptors : Page
    {
        private ObservableCollection<DescriptorTypeEntry> listOfDescriptorTypes;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        /// <summary>
        /// Initializes the list of descriptors that can be displayed in the UI
        /// </summary>
        public Scenario5_UsbDescriptors()
        {
            this.InitializeComponent();

            listOfDescriptorTypes = new ObservableCollection<DescriptorTypeEntry>();
            listOfDescriptorTypes.Add(new DescriptorTypeEntry("Device Descriptor", Descriptor.Device));
            listOfDescriptorTypes.Add(new DescriptorTypeEntry("Configuration Descriptor", Descriptor.Configuration));
            listOfDescriptorTypes.Add(new DescriptorTypeEntry("All Interface Descriptors", Descriptor.Interface));
            listOfDescriptorTypes.Add(new DescriptorTypeEntry("All Endpoint Descriptors", Descriptor.Endpoint));
            listOfDescriptorTypes.Add(new DescriptorTypeEntry("All Custom Descriptors", Descriptor.Custom));

            // The Product string is not a string descriptors, they are just strings
            // It is marked as String Descriptor because string descriptors are not available in this API explicitly like descriptors are; however, you may use control
            // transfers to obtain them. The recommended approach, at least to get the Product string, is to use the Windows.Devices.Enumeration API.
            //
            // To get the Manufacturer, the Windows.Devices.Enumeration.Pnp API must be used. The usage of PnpObject is similar to that of the Enumeration API. When 
            // creating the PnpObject with one of it's static methods, add "System.Devices.Manufacturer" to the list of properties to include. Note that the PnpObjectType
            // must be of type DeviceContainer, otherwise the manufacturer name cannot be obtained.
            // Once the PnpObject for the device has been obtained, use PnpObject.Properties dictionary where the key is "System.Devices.Manufacturer" and the value is
            // the manufacturer name.
            listOfDescriptorTypes.Add(new DescriptorTypeEntry("Product String", Descriptor.String));
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        ///
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs eventArgs)
        {
            // We will disable the scenario that is not supported by the device.
            // If no devices are connected, none of the scenarios will be shown and an error will be displayed
            Dictionary<DeviceType, UIElement> deviceScenarios = new Dictionary<DeviceType, UIElement>();
            deviceScenarios.Add(DeviceType.All, GenericScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);

            // Display list of descriptors that can be printed
            if ((EventHandlerForDevice.Current.IsDeviceConnected) &&
               (Utilities.GetDeviceType(EventHandlerForDevice.Current.Device) != DeviceType.None))
            {
                ListOfDescriptorTypesSource.Source = listOfDescriptorTypes;
            }
        }

        /// <summary>
        /// When an entry is selected from the list of available descriptors, we will print out the contents of the descriptor.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="eventArgs"></param>
        private void Descriptor_SelectChanged(Object sender, RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var listOfDescriptorTypes = (ListBox)sender;
                var selection = (DescriptorTypeEntry)listOfDescriptorTypes.SelectedValue;
                var descriptorType = selection.DescriptorType;

                PrintDescriptor(descriptorType);
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Obtains the stringified form of the chosen descriptor and prints it.
        /// </summary>
        /// <param name="descriptorType"></param>
        private void PrintDescriptor(Descriptor descriptorType)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                String descriptor = null;

                switch (descriptorType)
                {
                    case Descriptor.Device:
                        descriptor = GetDeviceDescriptorAsString();
                        break;
                    case Descriptor.Configuration:
                        descriptor = GetConfigurationDescriptorAsString();
                        break;
                    case Descriptor.Interface:
                        descriptor = GetInterfaceDescriptorsAsString();
                        break;
                    case Descriptor.Endpoint:
                        descriptor = GetEndpointDescriptorsAsString();
                        break;
                    case Descriptor.String:
                        descriptor = GetProductName();
                        break;
                    case Descriptor.Custom:
                        descriptor = GetCustomDescriptorsAsString();
                        break;
                }

                if (descriptor != null)
                {
                    DescriptorOutput.Text = descriptor;
                }
            }
        }

        private String GetDeviceDescriptorAsString()
        {
            String content = null;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var deviceDescriptor = EventHandlerForDevice.Current.Device.DeviceDescriptor;

                content = "Device Descriptor\n"
                    + "\nUsb Spec Number : 0x" + deviceDescriptor.BcdUsb.ToString("X4", NumberFormatInfo.InvariantInfo)
                    + "\nMax Packet Size (Endpoint 0) : " + deviceDescriptor.MaxPacketSize0.ToString("D", NumberFormatInfo.InvariantInfo)
                    + "\nVendor ID : 0x" + deviceDescriptor.VendorId.ToString("X4", NumberFormatInfo.InvariantInfo)
                    + "\nProduct ID : 0x" + deviceDescriptor.ProductId.ToString("X4", NumberFormatInfo.InvariantInfo)
                    + "\nDevice Revision : 0x" + deviceDescriptor.BcdDeviceRevision.ToString("X4", NumberFormatInfo.InvariantInfo)
                    + "\nNumber of Configurations : " + deviceDescriptor.NumberOfConfigurations.ToString("D", NumberFormatInfo.InvariantInfo);
            }

            return content;
        }

        private String GetConfigurationDescriptorAsString()
        {
            String content = null;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var usbConfiguration = EventHandlerForDevice.Current.Device.Configuration;
                var configurationDescriptor = usbConfiguration.ConfigurationDescriptor;

                content = "Configuration Descriptor\n"
                    + "\nNumber of Interfaces : " + usbConfiguration.UsbInterfaces.Count.ToString("D", NumberFormatInfo.InvariantInfo)
                    + "\nConfiguration Value : 0x" + configurationDescriptor.ConfigurationValue.ToString("X2", NumberFormatInfo.InvariantInfo)
                    + "\nSelf Powered : " + configurationDescriptor.SelfPowered.ToString()
                    + "\nRemote Wakeup : " + configurationDescriptor.RemoteWakeup.ToString()
                    + "\nMax Power (milliAmps) : " + configurationDescriptor.MaxPowerMilliamps.ToString("D", NumberFormatInfo.InvariantInfo);
            }

            return content;
        }

        /// <summary>
        /// Will convert all the interfaces to a human readable format
        /// </summary>
        private String GetInterfaceDescriptorsAsString()
        {
            String content = null;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var interfaces = EventHandlerForDevice.Current.Device.Configuration.UsbInterfaces;

                content = "Interface Descriptors";

                foreach (UsbInterface usbInterface in interfaces)
                {
                    // Will use class/subclass/protocol values from the first interface setting (usually the selected one)
                    UsbInterfaceDescriptor usbInterfaceDescriptor = usbInterface.InterfaceSettings[0].InterfaceDescriptor;

                    content += "\n\nInterface Number : 0x" + usbInterface.InterfaceNumber.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nClass Code : 0x" + usbInterfaceDescriptor.ClassCode.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nSubclass Code : 0x" + usbInterfaceDescriptor.SubclassCode.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nProtocol Code: 0x" + usbInterfaceDescriptor.ProtocolCode.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nNumber of Interface Settings : " + usbInterface.InterfaceSettings.Count.ToString("D", NumberFormatInfo.InvariantInfo)
                        + "\nNumber of open Bulk In Endpoints : " + usbInterface.BulkInPipes.Count.ToString("D", NumberFormatInfo.InvariantInfo)
                        + "\nNumber of open Bulk Out Endpoints : " + usbInterface.BulkOutPipes.Count.ToString("D", NumberFormatInfo.InvariantInfo)
                        + "\nNumber of open Interrupt In Endpoints : " + usbInterface.InterruptInPipes.Count.ToString("D", NumberFormatInfo.InvariantInfo)
                        + "\nNumber of open Interrupt Out Endpoints : " + usbInterface.InterruptOutPipes.Count.ToString("D", NumberFormatInfo.InvariantInfo);
                }

            }

            return content;
        }

        /// <summary>
        /// We will print all the endpoint descriptors of the default interface
        /// </summary>
        private String GetEndpointDescriptorsAsString()
        {
            String content = null;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var usbInterface = EventHandlerForDevice.Current.Device.DefaultInterface;
                var bulkInPipes = usbInterface.BulkInPipes;
                var bulkOutPipes = usbInterface.BulkOutPipes;
                var interruptInPipes = usbInterface.InterruptInPipes;
                var interruptOutPipes = usbInterface.InterruptOutPipes;

                content = "Endpoint Descriptors for open pipes";

                // Print Bulk In Endpoint descriptors
                foreach (UsbBulkInPipe bulkInPipe in bulkInPipes)
                {
                    var endpointDescriptor = bulkInPipe.EndpointDescriptor;

                    content += "\n\nBulk In Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + endpointDescriptor.EndpointNumber.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nMax Packet Size : " + endpointDescriptor.MaxPacketSize.ToString("D", NumberFormatInfo.InvariantInfo);
                }

                // Print Bulk Out Endpoint descriptors
                foreach (UsbBulkOutPipe bulkOutPipe in bulkOutPipes)
                {
                    var endpointDescriptor = bulkOutPipe.EndpointDescriptor;

                    content += "\n\nBulk Out Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + endpointDescriptor.EndpointNumber.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nMax Packet Size : " + endpointDescriptor.MaxPacketSize.ToString("D", NumberFormatInfo.InvariantInfo);
                }

                // Print Interrupt In Endpoint descriptors
                foreach (UsbInterruptInPipe interruptInPipe in interruptInPipes)
                {
                    var endpointDescriptor = interruptInPipe.EndpointDescriptor;

                    content += "\n\nInterrupt In Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + endpointDescriptor.EndpointNumber.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nMax Packet Size : " + endpointDescriptor.MaxPacketSize.ToString("D", NumberFormatInfo.InvariantInfo)
                        + "\nInterval(milliseconds) : " + endpointDescriptor.Interval.Milliseconds.ToString("D", NumberFormatInfo.InvariantInfo);
                }

                // Print Interrupt Out Endpoint descriptors
                foreach (UsbInterruptOutPipe interruptOutPipe in interruptOutPipes)
                {
                    var endpointDescriptor = interruptOutPipe.EndpointDescriptor;

                    content += "\n\nInterrupt Out Endpoint Descriptor"
                        + "\nEndpoint Number : 0x" + endpointDescriptor.EndpointNumber.ToString("X2", NumberFormatInfo.InvariantInfo)
                        + "\nMax Packet Size : " + endpointDescriptor.MaxPacketSize.ToString("D", NumberFormatInfo.InvariantInfo)
                        + "\nInterval(milliseconds) : " + endpointDescriptor.Interval.Milliseconds.ToString("D", NumberFormatInfo.InvariantInfo);
                }
            }

            return content;
        }

        /// <summary>
        /// There are no custom descriptors for the SuperMutt and OSRFX2 devices, however, this method will show you how to
        /// navigate through raw descriptors. The raw descriptors can be found in UsbConfiguration and
        /// UsbInterface, and UsbInterfaceSetting. All possible descriptors that you would find in the full Configuration Descriptor
        /// are found under UsbConfiguration.
        /// All raw descriptors under UsbInterface are constrained only to those that are found under the InterfaceDescriptor
        /// that is defined in the Usb spec.
        ///
        /// Usb descriptor header (first 2 bytes):
        /// bLength             Size of descriptor in bytes
        /// bDescriptorType     The type of descriptor (configuration, interface, endpoint)
        /// </summary>
        private String GetCustomDescriptorsAsString()
        {
            String content = null;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                // Descriptor information will be appended to this string and then printed to UI
                content = "Raw Descriptors";

                var configuration = EventHandlerForDevice.Current.Device.Configuration;
                var allRawDescriptors = configuration.Descriptors;

                // Print first 2 bytes of all descriptors within the configuration descriptor    
                // because the first 2 bytes are always length and descriptor type
                // the UsbDescriptor's DescriptorType and Length properties, but we will not use these properties
                // in order to demonstrate ReadDescriptorBuffer() and how to parse it.
                foreach (UsbDescriptor descriptor in allRawDescriptors)
                {
                    var descriptorBuffer = new Windows.Storage.Streams.Buffer(descriptor.Length);
                    descriptor.ReadDescriptorBuffer(descriptorBuffer);

                    DataReader reader = DataReader.FromBuffer(descriptorBuffer);

                    // USB data is Little Endian according to the USB spec.
                    reader.ByteOrder = ByteOrder.LittleEndian;

                    // ReadByte has a side effect where it consumes the current byte, so the next ReadByte will read the next character.
                    // Putting multiple ReadByte() on the same line (same variable assignment) may cause the bytes to be read out of order.
                    var length = reader.ReadByte().ToString("D", NumberFormatInfo.InvariantInfo);
                    var type = "0x" + reader.ReadByte().ToString("X2", NumberFormatInfo.InvariantInfo);

                    content += "\n\nDescriptor"
                        + "\nLength : " + length
                        + "\nDescriptorType : " + type;
                }
            }

            return content;
        }

        /// <summary>
        /// It is recommended to obtain the product string by using DeviceInformation.Name.
        /// </summary>
        private String GetProductName()
        {
            String content = null;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                content = "Product name: " + EventHandlerForDevice.Current.DeviceInformation.Name;
            }

            return content;
        }
    }
}
