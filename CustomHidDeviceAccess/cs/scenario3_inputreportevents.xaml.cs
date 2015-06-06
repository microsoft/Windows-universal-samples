//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using Windows.Devices.Enumeration;
using Windows.Devices.HumanInterfaceDevice;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomHidDeviceAccess
{
    /// <summary>
    /// This scenario demonstrates to how register and utilize input reports
    /// </summary>
    public sealed partial class InputReportEvents : Page
    {
        private TypedEventHandler<HidDevice, HidInputReportReceivedEventArgs> inputReportEventHandler;
        Boolean isRegisteredForInputReportEvents;
        private UInt32 numInputReportEventsReceived;
        private UInt32 totalNumberBytesReceived;

        // Device that we registered for events with
        private HidDevice registeredDevice;

        // Did we navigate away from this page?
        private Boolean navigatedAway;

        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        public InputReportEvents()
        {
            numInputReportEventsReceived = 0;
            isRegisteredForInputReportEvents = false;

            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        ///
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// 
        /// The SuperMutt requires an output report to tell the device to start sending interrupts, so we will start it
        /// just in case we register for the events. We want this to be transparent because it doesn't demonstrate how 
        /// to register for events.
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs eventArgs)
        {
            navigatedAway = false;

            // We will disable the scenario that is not supported by the device.
            // If no devices are connected, none of the scenarios will be shown and an error will be displayed
            Dictionary<DeviceType, UIElement> deviceScenarios = new Dictionary<DeviceType, UIElement>();
            deviceScenarios.Add(DeviceType.SuperMutt, SuperMuttScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);

            // Get notified when the device is about to be closed so we can unregister events
            EventHandlerForDevice.Current.OnDeviceClose = this.OnDeviceClosing;

            UpdateRegisterEventButtons();
        }

        /// <summary>
        /// Reset the device into a known state by undoing everything we did to the device (unregister interrupts and
        /// stop the device from creating interrupts)
        /// </summary>
        /// <param name="eventArgs"></param>
        protected override void OnNavigatedFrom(NavigationEventArgs eventArgs)
        {
            navigatedAway = true;

            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                UnregisterFromInputReportEvent();

                // Stop interrupts after unregistering so that device interrupts do not effect the scenario (stopping before unregistering
                // will cause events to stop being raised, which will be noticable by the app; we want this to be transparent)
                StopSuperMuttInputReports();
            }

            // We are leaving and no longer care about the device closing
            EventHandlerForDevice.Current.OnDeviceClose = null;
        }

        private async void OnDeviceClosing(EventHandlerForDevice sender, DeviceInformation deviceInformation)
        {
            UnregisterFromInputReportEvent();

            await rootPage.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
            {
                UpdateRegisterEventButtons();
            }));
        }

        private void RegisterForInputReportEvents_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                // Start the interrupts on the SuperMutt device (by default the device doesn't generate interrupts)
                StartSuperMuttInputReports();

                RegisterForInputReportEvents();

                UpdateRegisterEventButtons();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }

        }

        private void UnregisterFromInputReportEvents_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                UnregisterFromInputReportEvent();

                StopSuperMuttInputReports();

                UpdateRegisterEventButtons();
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Tells the SuperMutt device to start creating interrupts. The app won't get any interrupts until after we register for the event.
        /// 
        /// We are using an output report to start the interrupts, please go to Scenario4_InputOutputReports for more information on how to use
        /// output reports.
        /// </summary>
        private async void StartSuperMuttInputReports()
        {
            var outputReport = EventHandlerForDevice.Current.Device.CreateOutputReport(SuperMutt.DeviceInputReportControlInformation.ReportId);

            var dataWriter = new DataWriter();

            // First byte is always the report id
            dataWriter.WriteByte((Byte)outputReport.Id);
            dataWriter.WriteByte(SuperMutt.DeviceInputReportControlInformation.DataTurnOn);

            outputReport.Data = dataWriter.DetachBuffer();

            await EventHandlerForDevice.Current.Device.SendOutputReportAsync(outputReport);
        }

        /// <summary>
        /// Tells the SuperMutt device to stop creating interrupts. The app will stop getting event raises unless we start the interrupts again.
        /// 
        /// We are using an output report to start the interrupts, please go to Scenario4_InputOutputReports for more information on how to use
        /// output reports.
        /// </summary>
        private async void StopSuperMuttInputReports()
        {
            var outputReport = EventHandlerForDevice.Current.Device.CreateOutputReport(SuperMutt.DeviceInputReportControlInformation.ReportId);

            var dataWriter = new DataWriter();

            // First byte is always the report id
            dataWriter.WriteByte((Byte)outputReport.Id);
            dataWriter.WriteByte(SuperMutt.DeviceInputReportControlInformation.DataTurnOff);

            outputReport.Data = dataWriter.DetachBuffer();

            await EventHandlerForDevice.Current.Device.SendOutputReportAsync(outputReport);
        }

        /// <summary>
        /// This callback only increments the total number of events received and prints it
        ///
        /// This method is called whenever the device's state changes and sends a report. Since all input reports share the same event in 
        /// HidDevice, the app needs to get the HidInputReport from eventArgs.Report and compare report ids and usages with the desired
        /// report.
        /// </summary>
        /// <param name="sender">HidDevice that the event is being raised from</param> 
        /// <param name="eventArgs">Contains the HidInputReport that caused the event to raise</param> 
        private async void OnInputReportEvent(HidDevice sender, HidInputReportReceivedEventArgs eventArgs)
        {
            // If we navigated away from this page, we don't need to process this event
            // This also prevents output from spilling into another page
            if (!navigatedAway)
            {
                numInputReportEventsReceived++;

                // The data from the InputReport
                HidInputReport inputReport = eventArgs.Report;
                IBuffer buffer = inputReport.Data;

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
                            rootPage.NotifyUser(
                                "Total number of input report events received: " + numInputReportEventsReceived.ToString()
                                + "\nTotal number of bytes received: " + totalNumberBytesReceived.ToString(),
                                NotifyType.StatusMessage);
                        }
                    }));
            }
        }

        /// <summary>
        /// Register for the event that is triggered when the device sends a report to us.
        /// 
        /// All input reports share the same event in HidDevice, so once we register the event, all HidInputReports (regardless of the report id) will
        /// raise the event. Read the comment above OnInputReportEvent for more information on how to distinguish input reports.
        ///
        /// The function also saves the event token so that we can unregister from the event later on.
        /// </summary>
        private void RegisterForInputReportEvents()
        {
            if (!isRegisteredForInputReportEvents)
            {
                inputReportEventHandler = new TypedEventHandler<HidDevice, HidInputReportReceivedEventArgs>(this.OnInputReportEvent);

                // Remember which device we are registering the device with, in case there is a device disconnect and reconnect. We want to avoid unregistering
                // an event handler that is not registered with the device object.
                // Ideally, one should remove the event token (e.g. assign to null) upon the device removal to avoid using it again.
                registeredDevice = EventHandlerForDevice.Current.Device;

                registeredDevice.InputReportReceived += inputReportEventHandler;

                numInputReportEventsReceived = 0;
                totalNumberBytesReceived = 0;

                isRegisteredForInputReportEvents = true;
            }
        }

        /// <summary>
        /// Unregisters from the HidDevice's InputReportReceived event that was registered for in the RegisterForInputReportEvents();
        /// </summary>
        private void UnregisterFromInputReportEvent()
        {
            if (isRegisteredForInputReportEvents)
            {
                // Don't unregister event if the device was removed and reconnected because the endpoint event no longer contains our event handler
                if (registeredDevice == EventHandlerForDevice.Current.Device)
                {
                    registeredDevice.InputReportReceived -= inputReportEventHandler;
                }

                registeredDevice = null;
                isRegisteredForInputReportEvents = false;
            }
        }

        private void UpdateRegisterEventButtons()
        {
            ButtonRegisterForInputReportEvents.IsEnabled = !isRegisteredForInputReportEvents;
            ButtonUnregisterFromInputReportEvents.IsEnabled = isRegisteredForInputReportEvents;
        }
    }
}
