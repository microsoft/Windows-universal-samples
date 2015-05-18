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
using System.Threading.Tasks;
using Windows.Devices.Usb;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using CustomUsbDeviceAccess;

namespace CustomUsbDeviceAccess
{
    /// <summary>
    /// This page demonstrates how to use control transfers to communicate with the device.
    /// </summary>
    public sealed partial class Scenario2_ControlTransfer : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        public Scenario2_ControlTransfer()
        {
            this.InitializeComponent();
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
            deviceScenarios.Add(DeviceType.OsrFx2, OsrFx2Scenario);
            deviceScenarios.Add(DeviceType.SuperMutt, SuperMuttScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);
        }

        private async void GetOsrFx2SevenSegmentSetting_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonGetOsrFx2SevenSegment.IsEnabled = false;

                await GetOsrFx2SevenSegmentAsync();

                ButtonGetOsrFx2SevenSegment.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }

        }

        async void SetOsrFx2SevenSegmentSetting_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonSetOsrFx2SevenSegment.IsEnabled = false;

                Byte numericValue = (Byte)OsrFx2SevenSegmentSettingInput.SelectedIndex;

                await SetOsrFx2SevenSegmentAsync(numericValue);

                ButtonSetOsrFx2SevenSegment.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }

        }

        async void GetSuperMuttLedBlinkPattern_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonGetSuperMuttLedBlinkPattern.IsEnabled = false;

                await GetSuperMuttLedBlinkPatternAsync();

                ButtonGetSuperMuttLedBlinkPattern.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }

        }

        async void SetSuperMuttLedBlinkPattern_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonSetSuperMuttLedBlinkPattern.IsEnabled = false;

                Byte pattern = (Byte)SuperMuttLedBlinkPatternInput.SelectedIndex;

                await SetSuperMuttLedBlinkPatternAsync(pattern);

                ButtonSetSuperMuttLedBlinkPattern.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

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
        /// 
        /// The Buffer is used to hold data that is meant to be sent over during the data phase.
        /// The easiest way to write data to an IBuffer is to use a DataWriter. The DataWriter, when instantiated, 
        /// creates a buffer internally. The buffer is of type IBuffer and can be detached from the writer, which gives us
        /// the internal IBuffer.
        /// </summary>
        /// <param name="numericValue"></param>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        async Task SetOsrFx2SevenSegmentAsync(Byte numericValue)
        {
            DataWriter writer = new DataWriter();

            // Convert the numeric value into a 7 segment LED hex value and write it to a buffer.
            writer.WriteByte(OsrFx2.SevenLedSegmentMask[numericValue]);

            // The buffer with the data
            var bufferToSend = writer.DetachBuffer();

            UsbSetupPacket setupPacket = new UsbSetupPacket
            {
                RequestType = new UsbControlRequestType
                {
                    Direction = UsbTransferDirection.Out,
                    Recipient = UsbControlRecipient.Device,
                    ControlTransferType = UsbControlTransferType.Vendor
                },
                Request = OsrFx2.VendorCommand.SetSevenSegment,
                Value = 0,
                Length = bufferToSend.Length
            };

            UInt32 bytesTransferred = await EventHandlerForDevice.Current.Device.SendControlOutTransferAsync(setupPacket, bufferToSend);

            // Make sure we sent the correct number of bytes
            if (bytesTransferred == bufferToSend.Length)
            {
                MainPage.Current.NotifyUser("The segment display value is set to " + numericValue.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                MainPage.Current.NotifyUser(
                    "Error sending data. Sent bytes: " + bytesTransferred.ToString() + "; Tried to send : " + bufferToSend.Length,
                    NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Gets the current value of the seven segment display on the OSRFX2 device via control transfer. 
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled at the 
        /// end of the task chain.
        ///
        /// When the seven segment hex value is received, we attempt to match it with a known (has a numerical value 0-9) hex value.
        ///
        /// Getting Seven Segment LED require setup packet:
        /// bmRequestType:   type:       VENDOR
        ///                  recipient:  DEVICE
        /// bRequest:        0xD4
        /// wLength:         1
        /// </summary>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        async Task GetOsrFx2SevenSegmentAsync()
        {
            // We expect to receive 1 byte of data with our control transfer, which is the state of the seven segment
            uint receiveDataSize = 1;

            // The Seven Segment display value is only 1 byte
            IBuffer dataBuffer = await SendVendorControlTransferInToDeviceRecipientAsync(OsrFx2.VendorCommand.GetSevenSegment, receiveDataSize);

            if (dataBuffer.Length == receiveDataSize)
            {
                DataReader reader = DataReader.FromBuffer(dataBuffer);

                // The raw hex value representing the seven segment display
                Byte rawValue = reader.ReadByte();

                // The LED can't display the value of 255, so if the value
                // we receive is not a number, we'll know by seeing if this value changed
                Byte readableValue = 255;

                // Find which numeric value has the segment hex value
                for (int i = 0; i < OsrFx2.SevenLedSegmentMask.Length; i += 1)
                {
                    if (rawValue == OsrFx2.SevenLedSegmentMask[i])
                    {
                        readableValue = (Byte)i;

                        break;
                    }
                }

                // Seven Segment cannot display value 255, the value we received isn't a number
                if (readableValue == 255)
                {
                    MainPage.Current.NotifyUser("The segment display is not yet initialized", NotifyType.ErrorMessage);
                }
                else
                {
                    MainPage.Current.NotifyUser("The segment display value is " + readableValue.ToString(), NotifyType.StatusMessage);
                }
            }
            else
            {
                MainPage.Current.NotifyUser(
                    "Expected to read " + receiveDataSize.ToString() + " bytes, but got " + dataBuffer.Length.ToString(),
                    NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Initiates a control transfer to set the blink pattern on the SuperMutt's LED. 
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
        /// at the end of the task chain.
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
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        async Task SetSuperMuttLedBlinkPatternAsync(Byte pattern)
        {
            UsbSetupPacket setupPacket = new UsbSetupPacket
            {
                RequestType = new UsbControlRequestType
                {
                    Direction = UsbTransferDirection.Out,
                    Recipient = UsbControlRecipient.Device,
                    ControlTransferType = UsbControlTransferType.Vendor
                },
                Request = SuperMutt.VendorCommand.SetLedBlinkPattern,
                Value = pattern,
                Length = 0
            };

            UInt32 bytesTransferred = await EventHandlerForDevice.Current.Device.SendControlOutTransferAsync(setupPacket);

            MainPage.Current.NotifyUser("The Led blink pattern is set to " + pattern.ToString(), NotifyType.StatusMessage);
        }

        /// <summary>
        /// Sets up a UsbSetupPacket that will let the device know that we are trying to get the Led blink pattern on the SuperMutt via
        /// control transfer. 
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should 
        /// be handled at the end of the task chain.
        /// 
        /// The simplest way to obtain a byte from the buffer is by using a DataReader. DataReader provides a simple way
        /// to read from buffers (e.g. can return bytes, strings, ints).
        ///
        /// Do note the method that is used to send a control transfer. There are two methods to send a control transfer. 
        /// One is to send data and the other is to get data.
        /// </summary>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        async Task GetSuperMuttLedBlinkPatternAsync()
        {
            // The blink pattern is 1 byte long, so we only need to retrieve 1 byte of data
            IBuffer buffer = await SendVendorControlTransferInToDeviceRecipientAsync(SuperMutt.VendorCommand.GetLedBlinkPattern, 1);

            if (buffer.Length > 0)
            {
                // Easiest way to read from a buffer
                DataReader dataReader = DataReader.FromBuffer(buffer);

                Byte pattern = dataReader.ReadByte();

                MainPage.Current.NotifyUser("The Led blink pattern is " + pattern.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                MainPage.Current.NotifyUser("ControlInTransfer returned 0 bytes", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Sets up a UsbSetupPacket and sends control transfer that will let the device know that we are trying to retrieve data from the device.
        /// This method only supports vendor commands because the scenario only uses vendor commands.
        ///
        /// Do note the method that is used to send a control transfer. There are two methods to send a control transfer. 
        /// One involves receiving buffer data in the Data stage of the control transfer, and the other involves transmiting the buffer data.
        /// 
        /// The simplest way to obtain a byte from the buffer is by using a DataReader. DataReader provides a simple way
        /// to read from buffers (e.g. can return bytes, strings, ints).
        /// </summary>
        /// <param name="vendorCommand">Command to put into SetupPacket's Request property</param>
        /// <param name="dataPacketLength">Number of bytes in the data packet that is sent in the Data Stage</param>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        async Task<IBuffer> SendVendorControlTransferInToDeviceRecipientAsync(Byte vendorCommand, UInt32 dataPacketLength)
        {
            // Data will be written to this buffer when we receive it
            var buffer = new Windows.Storage.Streams.Buffer(dataPacketLength);

            UsbSetupPacket setupPacket = new UsbSetupPacket
            {
                RequestType = new UsbControlRequestType
                {
                    Direction = UsbTransferDirection.In,
                    Recipient = UsbControlRecipient.Device,
                    ControlTransferType = UsbControlTransferType.Vendor,
                },
                Request = vendorCommand,
                Length = dataPacketLength
            };
            return await EventHandlerForDevice.Current.Device.SendControlInTransferAsync(setupPacket, buffer);
        }
    }
}
