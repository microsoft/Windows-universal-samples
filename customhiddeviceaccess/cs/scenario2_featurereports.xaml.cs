//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Devices.HumanInterfaceDevice;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;

namespace CustomHidDeviceAccess
{
    /// <summary>
    /// This page demonstrates how to use FeatureReports to communicate with the device
    /// </summary>
    public sealed partial class FeatureReports : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        private MainPage rootPage = MainPage.Current;

        public FeatureReports()
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

        async void GetLedBlinkPattern_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonGetLedBlinkPattern.IsEnabled = false;

                await GetLedBlinkPatternAsync();

                ButtonGetLedBlinkPattern.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }

        }

        async void SetLedBlinkPattern_Click(Object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (EventHandlerForDevice.Current.IsDeviceConnected)
            {
                ButtonSetLedBlinkPattern.IsEnabled = false;

                Byte pattern = (Byte)LedBlinkPatternInput.SelectedIndex;

                await SetLedBlinkPatternAsync(pattern);

                ButtonSetLedBlinkPattern.IsEnabled = true;
            }
            else
            {
                Utilities.NotifyDeviceNotConnected();
            }
        }

        /// <summary>
        /// Uses a feature report to set the blink pattern on the SuperMutt's LED. 
        /// 
        /// Please note that when we create a FeatureReport, all data is nulled out in the report. Since we will only be modifying 
        /// data we care about, the other bits that we don't care about, will be zeroed out. Controls will effectively do the same thing (
        /// all bits are zeroed out except for the bits we care about).
        /// 
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should be handled 
        /// at the end of the task chain.
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
        async Task SetLedBlinkPatternAsync(Byte pattern)
        {
            var featureReport = EventHandlerForDevice.Current.Device.CreateFeatureReport(SuperMutt.LedPattern.ReportId);

            var dataWriter = new DataWriter();

            // First byte is always the report id
            dataWriter.WriteByte((Byte)featureReport.Id);
            dataWriter.WriteByte(pattern);

            featureReport.Data = dataWriter.DetachBuffer();

            await EventHandlerForDevice.Current.Device.SendFeatureReportAsync(featureReport);

            rootPage.NotifyUser("The Led blink pattern is set to " + pattern.ToString(), NotifyType.StatusMessage);
        }

        /// <summary>
        /// Uses feature report to get the LED blink pattern. 
        ///
        /// Any errors in async function will be passed down the task chain and will not be caught here because errors should 
        /// be handled at the end of the task chain.
        /// 
        /// The simplest way to obtain a byte from the buffer is by using a DataReader. DataReader provides a simple way
        /// to read from buffers (e.g. can return bytes, strings, ints).
        /// </summary>
        /// <returns>A task that can be used to chain more methods after completing the scenario</returns>
        async Task GetLedBlinkPatternAsync()
        {
            var featureReport = await EventHandlerForDevice.Current.Device.GetFeatureReportAsync(SuperMutt.LedPattern.ReportId);

            if (featureReport.Data.Length == 2)
            {
                var dataReader = DataReader.FromBuffer(featureReport.Data);

                var reportId = dataReader.ReadByte();
                var pattern = dataReader.ReadByte();

                rootPage.NotifyUser("The Led blink pattern is " + pattern.ToString(), NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("Expecting 2 bytes, but received " + featureReport.Data.Length, NotifyType.ErrorMessage);
            }
        }

    }
}
