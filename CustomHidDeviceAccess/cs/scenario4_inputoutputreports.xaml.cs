//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Threading.Tasks;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomHidDeviceAccess
{
    /// <summary>
    /// This scenario demonstrates how to get input reports and how to send output reports
    /// </summary>
    public sealed partial class InputOutputReports : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        public InputOutputReports()
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
            deviceScenarios.Add(DeviceType.SuperMutt, SuperMuttScenario);

            Utilities.SetUpDeviceScenarios(deviceScenarios, DeviceScenarioContainer);
        }

        private async void GetNumericInputReport_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                // Don't allow multiple reads at the same time
                ButtonGetNumericInputReport.IsEnabled = false;

                await GetNumericInputReportAsync();

                ButtonGetNumericInputReport.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private async void SendNumericOutputReport_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonSendNumericOutputReport.IsEnabled = false;

                Byte valueToWrite = (Byte)NumericValueToWrite.SelectedIndex;

                await SendNumericOutputReportAsync(valueToWrite);

                ButtonSendNumericOutputReport.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private async void GetBooleanInputReport_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                // Don't allow multiple reads at the same time
                ButtonGetBooleanInputReport.IsEnabled = false;

                // This sample will only demonstrate using the first button
                await GetBooleanInputReportAsync(1);

                ButtonGetBooleanInputReport.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        private async void SendBooleanOutputReport_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs eventArgs)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonSendBooleanOutputReport.IsEnabled = false;

                Boolean valueToWrite = BooleanValueToWrite.SelectedIndex == 1 ? true : false;

                // We will only demonstrate modifying button 1
                await SendBooleanOutputReportAsync(valueToWrite, 1);

                ButtonSendBooleanOutputReport.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Uses an input report to get the value that was previously written to the device with output report.
        /// 
        /// We will be using controls to modify the data buffer. See SendNumericOutputReportAsync for how to modify the data buffer directly.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
        /// at the end of the task chain.
        /// </summary>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        private async Task GetNumericInputReportAsync()
        {
            var inputReport = await EventHandlerForDevice.Current.Device.GetInputReportAsync(SuperMutt.ReadWriteBufferControlInformation.ReportId);

            var inputReportControl = inputReport.GetNumericControl(SuperMutt.ReadWriteBufferControlInformation.VolumeUsagePage, SuperMutt.ReadWriteBufferControlInformation.VolumeUsageId);

            var data = inputReportControl.Value;

            rootPage.NotifyUser("Value read: " + data.ToString(), NotifyType.StatusMessage);
        }

        /// <summary>
        /// Uses an output report to write data to the device so that it can be read back using input report.
        ///
        /// Please note that when we create an OutputReport, all data is nulled out in the report. Since we will only be modifying 
        /// data we care about, the other bits that we don't care about, will be zeroed out. Controls will effectively do the same thing (
        /// all bits are zeroed out except for the bits we care about).
        ///
        /// We will modify the data buffer directly. See GetNumericInputReportAsync for how to use controls.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
        /// at the end of the task chain.
        /// </summary>
        /// <param name="valueToWrite"></param>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        private async Task SendNumericOutputReportAsync(Byte valueToWrite)
        {
            var outputReport = EventHandlerForDevice.Current.Device.CreateOutputReport(SuperMutt.ReadWriteBufferControlInformation.ReportId);

            Byte dataByte = (Byte)(valueToWrite << SuperMutt.ReadWriteBufferControlInformation.VolumeDataStartBit);

            var dataWriter = new DataWriter();

            // First byte is always the report id
            dataWriter.WriteByte((Byte)outputReport.Id);
            dataWriter.WriteByte(dataByte);

            outputReport.Data = dataWriter.DetachBuffer();

            uint bytesWritten = await EventHandlerForDevice.Current.Device.SendOutputReportAsync(outputReport);

            rootPage.NotifyUser("Value written: " + valueToWrite, NotifyType.StatusMessage);
        }

        /// <summary>
        /// Uses an input report to get the value that was previously written to the device with output report.
        /// 
        /// We will be using controls to modify the data buffer. See SendBooleanOutputReportAsync for how to modify the data buffer directly.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
        /// at the end of the task chain.
        /// </summary>
        /// <param name="buttonNumber">Button to get the value of (1-4)</param>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        private async Task GetBooleanInputReportAsync(Byte buttonNumber)
        {
            var inputReport = await EventHandlerForDevice.Current.Device.GetInputReportAsync(SuperMutt.ReadWriteBufferControlInformation.ReportId);

            // Use the first button
            var inputReportControl = inputReport.GetBooleanControl(SuperMutt.ReadWriteBufferControlInformation.ButtonUsagePage, SuperMutt.ReadWriteBufferControlInformation.ButtonUsageId[buttonNumber - 1]);

            var value = inputReportControl.IsActive ? "1" : "0";

            rootPage.NotifyUser("Bit value: " + value, NotifyType.StatusMessage);
        }

        /// <summary>
        /// Uses an output report to write data to the device so that it can be read back using input report.
        /// 
        /// Please note that when we create an OutputReport, all data is nulled out in the report. Since we will only be modifying 
        /// data we care about, the other bits that we don't care about, will be zeroed out. Controls will effectively do the same thing (
        /// all bits are zeroed out except for the bits we care about).
        /// 
        /// We will modify the data buffer directly. See GetBooleanInputReportAsync for how to use controls.
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
        /// at the end of the task chain.
        /// </summary>
        /// <param name="bitValue"></param>
        /// <param name="buttonNumber">Button to change value of (1-4)</param>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        private async Task SendBooleanOutputReportAsync(Boolean bitValue, Byte buttonNumber)
        {
            var buttonIndex = buttonNumber - 1;

            var outputReport = EventHandlerForDevice.Current.Device.CreateOutputReport(SuperMutt.ReadWriteBufferControlInformation.ReportId);

            Byte byteToMask = (Byte)(bitValue ? 0xFF : 0x00);
            Byte dataByte = (Byte)(byteToMask & SuperMutt.ReadWriteBufferControlInformation.ButtonDataMask[buttonIndex]);

            var dataWriter = new DataWriter();

            // First byte is always the report id
            dataWriter.WriteByte((Byte)outputReport.Id);
            dataWriter.WriteByte(dataByte);

            outputReport.Data = dataWriter.DetachBuffer();

            UInt32 bytesWritten = await EventHandlerForDevice.Current.Device.SendOutputReportAsync(outputReport);

            var valueWritten = bitValue ? "1" : "0";
            rootPage.NotifyUser("Bit value written: " + valueWritten, NotifyType.StatusMessage);
        }
    }
}
