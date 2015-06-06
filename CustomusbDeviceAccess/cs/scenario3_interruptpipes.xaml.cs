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
using System.Globalization;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Devices.Usb;
using Windows.Devices.Enumeration;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomUsbDeviceAccess
{
    /// <summary>
    /// This page demonstrates how to use interrupts pipes on a UsbDevice
    /// </summary>
    public sealed partial class Scenario3_InterruptPipes : Page
    {
        private TypedEventHandler<UsbInterruptInPipe, UsbInterruptInEventArgs> interruptEventHandler;
        private UInt32 registeredInterruptPipeIndex; // Pipe index of the pipe we that we registered for. Only valid if registeredInterrupt is true
        private Boolean registeredInterrupt;

        private UInt32 numInterruptsReceived;
        private UInt32 totalNumberBytesReceived;
        private UInt32 totalNumberBytesWritten;

        // Indicate if we navigate away from this page or not.
        private Boolean navigatedAway;

        // Only valid for the OSRFX2 device
        private List<Boolean> previousSwitchStates;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        public Scenario3_InterruptPipes()
        {
            registeredInterruptPipeIndex = 0;
            totalNumberBytesWritten = 0;
            registeredInterrupt = false;
            numInterruptsReceived = 0;
            totalNumberBytesReceived = 0;
            previousSwitchStates = null;

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
            navigatedAway = false;
            totalNumberBytesWritten = 0;

            // We will disable the scenario that is not supported by the device.
            // If no devices are connected, none of the scenarios will be shown and an error will be displayed
            Dictionary<DeviceType, UIElement> deviceScenarios = new Dictionary<DeviceType, UIElement>();
            deviceScenarios.Add(DeviceType.OsrFx2, OsrFx2Scenario);
            deviceScenarios.Add(DeviceType.SuperMutt, SuperMuttScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);

            // Get notified when the device is about to be closed so we can unregister events
            EventHandlerForDevice.Current.OnDeviceClose = this.OnDeviceClosing;

            ClearSwitchStateTable();
            UpdateRegisterEventButton();
        }

        protected override void OnNavigatedFrom(NavigationEventArgs eventArgs)
        {
            navigatedAway = true;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                UnregisterFromInterruptEvent();
            }

            // We are leaving and no longer care about the device closing
            EventHandlerForDevice.Current.OnDeviceClose = null;
        }

        private async void OnDeviceClosing(EventHandlerForDevice sender, DeviceInformation deviceInformation)
        {
            UnregisterFromInterruptEvent();

            await rootPage.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal,
                new DispatchedHandler(() =>
                {
                    UpdateRegisterEventButton();
                    ClearSwitchStateTable();
                }));
        }

        /// <summary>
        /// This method will register for events on the correct interrupt in pipe index on the OsrFx2 device.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="eventArgs"></param>
        private void RegisterOsrFx2InterruptEvent_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                UInt32 interruptPipeIndex = OsrFx2.Pipe.InterruptInPipeIndex;
                var interruptEventHandler = new TypedEventHandler<UsbInterruptInPipe, UsbInterruptInEventArgs>(this.OnOsrFx2SwitchStateChangeEvent);

                RegisterForInterruptEvent(interruptPipeIndex, interruptEventHandler);

                UpdateRegisterEventButton();
                ClearSwitchStateTable();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }

        }

        /// <summary>
        /// This method will register for events on the correct interrupt in pipe index on the SuperMutt device.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="eventArgs"></param>
        private void RegisterSuperMuttInterruptEvent_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                UInt32 interruptPipeIndex = SuperMutt.Pipe.InterruptInPipeIndex;

                var interruptEventHandler = new TypedEventHandler<UsbInterruptInPipe, UsbInterruptInEventArgs>(this.OnGeneralInterruptEvent);

                RegisterForInterruptEvent(interruptPipeIndex, interruptEventHandler);

                UpdateRegisterEventButton();
                ClearSwitchStateTable();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }

        }

        private void UnregisterInterruptEvent_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                UnregisterFromInterruptEvent();

                UpdateRegisterEventButton();
                ClearSwitchStateTable();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// This scenario is only available on the SuperMutt device because the OSRFX2 doesn't have Out Interrupt Endpoints.
        /// 
        /// This method will determine the right InterruptOutPipe to use and will call another method to do the actual writing.
        /// <summary>
        /// <param name="sender"></param>
        /// <param name="eventArgs"></param>
        private void WriteSuperMuttInterruptOut_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                var interruptOutPipeIndex = SuperMutt.Pipe.InterruptOutPipeIndex;
                var interruptOutPipes = EventHandlerForDevice.Current.Device.DefaultInterface.InterruptOutPipes;

                if (interruptOutPipeIndex < interruptOutPipes.Count)
                {
                    // We will write the maximum number of bytes we can per interrupt
                    var bytesToWrite = interruptOutPipes[(int)interruptOutPipeIndex].EndpointDescriptor.MaxPacketSize;

                    WriteToInterruptOut(interruptOutPipeIndex, bytesToWrite);
                }
                else
                {
                    MainPage.Current.NotifyUser("Pipe index provided (" + interruptOutPipeIndex.ToString(NumberFormatInfo.InvariantInfo) + ") is out of range", NotifyType.ErrorMessage);
                }
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Register for the interrupt that is triggered when the device sends an interrupt to us (e.g. OSRFX2's device's switch state changes
        /// or SuperMUTT's software generated interrupts).
        /// The DefaultInterface on the the device is the first interface on the device. We navigate to
        /// the InterruptInPipes because that collection contains all the interrupt in pipes for the
        /// selected interface setting.
        ///
        /// Each pipe has a property that links to an EndpointDescriptor. This descriptor can be used to find information about
        /// the pipe (e.g. type, id, etc...). The EndpointDescriptor trys to mirror the EndpointDescriptor that is defined in the Usb Spec.
        ///
        /// The function also saves the event token so that we can unregister from the even later on.
        /// </summary>
        /// <param name="pipeIndex">The index of the pipe found in UsbInterface.InterruptInPipes. It is not the endpoint number</param>
        /// <param name="eventHandler">Event handler that will be called when the event is raised</param>
        private void RegisterForInterruptEvent(UInt32 pipeIndex, TypedEventHandler<UsbInterruptInPipe, UsbInterruptInEventArgs> eventHandler)
        {
            var interruptInPipes = EventHandlerForDevice.Current.Device.DefaultInterface.InterruptInPipes;

            if (!registeredInterrupt && (pipeIndex < interruptInPipes.Count))
            {
                var interruptInPipe = interruptInPipes[(int)pipeIndex];

                registeredInterrupt = true;
                registeredInterruptPipeIndex = pipeIndex;

                // Save the interrupt handler so we can use it to unregister
                interruptEventHandler = eventHandler;

                interruptInPipe.DataReceived += interruptEventHandler;

                numInterruptsReceived = 0;
                totalNumberBytesReceived = 0;
            }
        }

        /// <summary>
        /// Unregisters from the interrupt event that was registered for in the RegisterForInterruptEvent();
        /// </summary>
        private void UnregisterFromInterruptEvent()
        {
            if (registeredInterrupt)
            {
                // Search for the correct pipe that we know we used to register events
                var interruptInPipe = EventHandlerForDevice.Current.Device.DefaultInterface.InterruptInPipes[(int)registeredInterruptPipeIndex];
                interruptInPipe.DataReceived -= interruptEventHandler;

                registeredInterrupt = false;
            }
        }

        /// <summary>
        /// This method is called whenever the device sends an interrupt on the pipe we registered this callback on.
        ///
        /// We will read a byte from the buffer that the device sent to us and then look at each bit to determine state of
        /// each switch. AFter determining the state of each switch, we will print out a table with each row representing a 
        /// switch and its state.
        /// </summary>
        /// <param name="sender">The interrupt pipe that raised the event (the one that received the interrupt)</param>
        /// <param name="eventArgs">Contains the data, in an IBuffer, that was received through the interrupts</param> 
        private async void OnOsrFx2SwitchStateChangeEvent(UsbInterruptInPipe sender, UsbInterruptInEventArgs eventArgs)
        {
            // If we navigated away from this page, we don't need to process this event
            // This also prevents output from spilling into another page
            if (!navigatedAway)
            {
                numInterruptsReceived++;

                // The OSRFX2 gives us 1 byte, each bit representing the state of a switch
                const Byte numberOfSwitches = 8;

                var switchStateList = new List<Boolean>(numberOfSwitches);

                IBuffer buffer = eventArgs.InterruptData;

                if (buffer.Length > 0)
                {
                    totalNumberBytesReceived += buffer.Length;

                    DataReader reader = DataReader.FromBuffer(buffer);

                    byte switchStates = reader.ReadByte();

                    // Loop through each bit of what the device sent us and determine the state of each switch
                    for (int switchIndex = 0; switchIndex < numberOfSwitches; switchIndex++)
                    {
                        int result = (1 << switchIndex) & switchStates;

                        if (result != 0)
                        {
                            switchStateList.Add(true);
                        }
                        else
                        {
                            switchStateList.Add(false);
                        }
                    }

                    // Create a DispatchedHandler for the because we are printing the table to the UI directly and the
                    // thread that this function is running on may not be the UI thread; if a non-UI thread modifies
                    // the UI, an exception is thrown
                    await rootPage.Dispatcher.RunAsync(
                        CoreDispatcherPriority.Normal,
                        new DispatchedHandler(() =>
                        {
                            // If we navigated away from this page, do not print anything. The dispatch may be handled after
                            // we move to a different page.
                            if (!navigatedAway)
                            {
                                rootPage.NotifyUser(
                                    "Total number of interrupt events received: " + numInterruptsReceived.ToString()
                                    + "\nTotal number of bytes received: " + totalNumberBytesReceived.ToString(),
                                    NotifyType.StatusMessage);

                                // Print the switch state table
                                UpdateSwitchStateTable(switchStateList);
                            }
                        }));
                }
                else
                {
                    await rootPage.Dispatcher.RunAsync(
                        CoreDispatcherPriority.Normal,
                        new DispatchedHandler(() =>
                        {
                            // If we navigated away from this page, do not print anything. The dispatch may be handled after
                            // we move to a different page.
                            if (!navigatedAway)
                            {
                                rootPage.NotifyUser("Received 0 bytes from interrupt", NotifyType.ErrorMessage);
                            }
                        }));
                }
            }
        }

        /// <summary>
        /// This callback only increments the total number of interrupts received and prints it
        ///
        /// This method is called whenever the device sends an interrupt on the pipe we registered this callback on.
        /// </summary>
        /// <param name="sender">The interrupt pipe that raised the event (the one that received the interrupt)</param>
        /// <param name="eventArgs">Contains the data, in an IBuffer, that was received through the interrupts</param> 
        private async void OnGeneralInterruptEvent(UsbInterruptInPipe sender, UsbInterruptInEventArgs eventArgs)
        {
            // If we navigated away from this page, we don't need to process this event
            // This also prevents output from spilling into another page
            if (!navigatedAway)
            {
                numInterruptsReceived++;

                // The data from the interrupt
                IBuffer buffer = eventArgs.InterruptData;

                totalNumberBytesReceived += buffer.Length;

                // Create a DispatchedHandler for the because we are interracting with the UI directly and the
                // thread that this function is running on may not be the UI thread; if a non-UI thread modifies
                // the UI, an exception is thrown
                await rootPage.Dispatcher.RunAsync(
                    CoreDispatcherPriority.Normal,
                    new DispatchedHandler(() =>
                    {
                        // If we navigated away from this page, do not print anything. The dispatch may be handled after
                        // we move to a different page.
                        if (!navigatedAway)
                        {
                            MainPage.Current.NotifyUser(
                                "Total number of interrupt events received: " + numInterruptsReceived.ToString()
                                + "\nTotal number of bytes received: " + totalNumberBytesReceived.ToString(),
                                NotifyType.StatusMessage);
                        }
                    }));
            }
        }

        /// <summary>
        /// Will write garbage data to the specified output pipe
        /// </summary>
        /// <param name="pipeIndex">Index of pipe in the list of Device.DefaultInterface.InterruptOutPipes</param>
        /// <param name="bytesToWrite">Bytes of garbage data to write</param>
        private async void WriteToInterruptOut(UInt32 pipeIndex, UInt32 bytesToWrite)
        {
            var interruptOutPipes = EventHandlerForDevice.Current.Device.DefaultInterface.InterruptOutPipes;

            if (pipeIndex < interruptOutPipes.Count)
            {
                // Create an array, all default initialized to 0, and write it to the buffer
                // The data inside the buffer will be garbage
                var arrayBuffer = new Byte[bytesToWrite];

                var pipe = interruptOutPipes[(int)pipeIndex];

                var pipeWriter = new DataWriter(pipe.OutputStream);
                pipeWriter.WriteBytes(arrayBuffer);

                // This is where the data is flushed out to the device.
                UInt32 bytesWritten = await pipeWriter.StoreAsync();

                totalNumberBytesWritten += bytesWritten;

                MainPage.Current.NotifyUser("Total Bytes Written: " + totalNumberBytesWritten.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                MainPage.Current.NotifyUser("Pipe index provided (" + pipeIndex.ToString() + ") is out of range", NotifyType.ErrorMessage);
            }
        }

        private void ClearSwitchStateTable()
        {
            SwitchStates.Inlines.Clear();
            previousSwitchStates = null;
        }

        /// <summary>
        /// Prints a table in the UI representing the current state of each of the switches on the OSRFX2 board.
        /// The bolded states/switches are the switches that have their states changed.
        ///
        /// Note that the OSRFX2 board has the switch states reversed (1 is off and 0 is on)
        /// </summary>
        /// <param name="states">Switch states where 0 is off and 1 is on</param>
        private void UpdateSwitchStateTable(List<Boolean> states)
        {
            CreateBooleanChartInTable(
                SwitchStates.Inlines,  // The table will be written directly to the UI's textbox
                states,
                previousSwitchStates,
                "off",
                "on");

            previousSwitchStates = states;
        }

        private void UpdateRegisterEventButton()
        {
            ButtonRegisterSuperMuttInterruptEvent.IsEnabled = !registeredInterrupt;
            ButtonRegisterOsrFx2InterruptEvent.IsEnabled = !registeredInterrupt;
            ButtonUnregisterInterruptEvent.IsEnabled = registeredInterrupt;
        }

        /// <summary>
        /// Clears and populates the provided table with rows that have the following syntax:
        /// [row #]    [state of new value]
        ///
        /// If the state of a value has changed, the value will be bolded.
        /// </summary>
        /// <param name="table">Table will be cleared and new rows will populate this table.</param>
        /// <param name="newValues">The new boolean values</param>
        /// <param name="oldValues">The boolean values that the new ones are being compared to</param>
        /// <param name="trueValue">Text if the new value is TRUE</param>
        /// <param name="falseValue">Text if the new vlaue is FALSE</param>
        public static Boolean CreateBooleanChartInTable(
            InlineCollection table,
            List<Boolean> newValues,
            List<Boolean> oldValues,
            String trueValue,
            String falseValue)
        {
            Boolean isBooleanChartCreated = false;

            // Make sure there are at least newValues to check or that there are the same number of old values as new values
            if ((newValues != null) && ((oldValues == null) || (newValues.Count == oldValues.Count)))
            {
                table.Clear();

                // Each new value has it's own row
                for (int i = 0; i < (int)newValues.Count; i += 1)
                {
                    var line = new Span();

                    // Row #
                    var block = new Run();
                    block.Text = (i + 1).ToString("D", NumberFormatInfo.InvariantInfo);
                    line.Inlines.Add(block);

                    // Space between row # and the value (simulates a column)
                    block = new Run();
                    block.Text = "    ";
                    line.Inlines.Add(block);

                    // Print the textual form of TRUE/FALSE values
                    block = new Run();
                    block.Text = newValues[i] ? trueValue : falseValue;

                    // Bold values that have changed
                    if ((oldValues != null) && (oldValues[i] != newValues[i]))
                    {
                        var bold = new Bold();
                        bold.Inlines.Add(block);
                        line.Inlines.Add(bold);
                    }
                    else
                    {
                        line.Inlines.Add(block);
                    }

                    line.Inlines.Add(new LineBreak());

                    table.Add(line);
                }

                isBooleanChartCreated = true;
            }

            return isBooleanChartCreated;
        }
    }
}
