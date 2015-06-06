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
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using Windows.Devices.Bluetooth.Advertisement;

using SDKTemplate;

namespace BluetoothAdvertisement
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario1_Watcher : Page
    {
        // The Bluetooth LE advertisement watcher class is used to control and customize Bluetooth LE scanning.
        private BluetoothLEAdvertisementWatcher watcher;

        // A pointer back to the main page is required to display status messages.
        private MainPage rootPage;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public Scenario1_Watcher()
        {
            this.InitializeComponent();

            // Create and initialize a new watcher instance.
            watcher = new BluetoothLEAdvertisementWatcher();

            // Begin of watcher configuration. Configure the advertisement filter to look for the data advertised by the publisher 
            // in Scenario 2 or 4. You need to run Scenario 2 on another Windows platform within proximity of this one for Scenario 1 to 
            // take effect. The APIs shown in this Scenario are designed to operate only if the App is in the foreground. For background
            // watcher operation, please refer to Scenario 3.

            // Please comment out this following section (watcher configuration) if you want to remove all filters. By not specifying
            // any filters, all advertisements received will be notified to the App through the event handler. You should comment out the following
            // section if you do not have another Windows platform to run Scenario 2 alongside Scenario 1 or if you want to scan for 
            // all LE advertisements around you.

            // For determining the filter restrictions programatically across APIs, use the following properties:
            //      MinSamplingInterval, MaxSamplingInterval, MinOutOfRangeTimeout, MaxOutOfRangeTimeout

            // Part 1A: Configuring the advertisement filter to watch for a particular advertisement payload

            // First, let create a manufacturer data section we wanted to match for. These are the same as the one 
            // created in Scenario 2 and 4.
            var manufacturerData = new BluetoothLEManufacturerData();

            // Then, set the company ID for the manufacturer data. Here we picked an unused value: 0xFFFE
            manufacturerData.CompanyId = 0xFFFE;

            // Finally set the data payload within the manufacturer-specific section
            // Here, use a 16-bit UUID: 0x1234 -> {0x34, 0x12} (little-endian)
            var writer = new DataWriter();
            writer.WriteUInt16(0x1234);

            // Make sure that the buffer length can fit within an advertisement payload. Otherwise you will get an exception.
            manufacturerData.Data = writer.DetachBuffer();

            // Add the manufacturer data to the advertisement filter on the watcher:
            watcher.AdvertisementFilter.Advertisement.ManufacturerData.Add(manufacturerData);


            // Part 1B: Configuring the signal strength filter for proximity scenarios

            // Configure the signal strength filter to only propagate events when in-range
            // Please adjust these values if you cannot receive any advertisement 
            // Set the in-range threshold to -70dBm. This means advertisements with RSSI >= -70dBm 
            // will start to be considered "in-range".
            watcher.SignalStrengthFilter.InRangeThresholdInDBm = -70;

            // Set the out-of-range threshold to -75dBm (give some buffer). Used in conjunction with OutOfRangeTimeout
            // to determine when an advertisement is no longer considered "in-range"
            watcher.SignalStrengthFilter.OutOfRangeThresholdInDBm = -75;

            // Set the out-of-range timeout to be 2 seconds. Used in conjunction with OutOfRangeThresholdInDBm
            // to determine when an advertisement is no longer considered "in-range"
            watcher.SignalStrengthFilter.OutOfRangeTimeout = TimeSpan.FromMilliseconds(2000);

            // By default, the sampling interval is set to zero, which means there is no sampling and all
            // the advertisement received is returned in the Received event

            // End of watcher configuration. There is no need to comment out any code beyond this point.
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        ///
        /// We will enable/disable parts of the UI if the device doesn't support it.
        /// </summary>
        /// <param name="eventArgs">Event data that describes how this page was reached. The Parameter
        /// property is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage = MainPage.Current;

            // Attach a handler to process the received advertisement. 
            // The watcher cannot be started without a Received handler attached
            watcher.Received += OnAdvertisementReceived;

            // Attach a handler to process watcher stopping due to various conditions,
            // such as the Bluetooth radio turning off or the Stop method was called
            watcher.Stopped += OnAdvertisementWatcherStopped;

            // Attach handlers for suspension to stop the watcher when the App is suspended.
            App.Current.Suspending += App_Suspending;
            App.Current.Resuming += App_Resuming;

            rootPage.NotifyUser("Press Run to start watcher.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked immediately before the Page is unloaded and is no longer the current source of a parent Frame.
        /// </summary>
        /// <param name="e">
        /// Event data that can be examined by overriding code. The event data is representative
        /// of the navigation that will unload the current Page unless canceled. The
        /// navigation can potentially be canceled by setting Cancel.
        /// </param>
        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // Remove local suspension handlers from the App since this page is no longer active.
            App.Current.Suspending -= App_Suspending;
            App.Current.Resuming -= App_Resuming;

            // Make sure to stop the watcher when leaving the context. Even if the watcher is not stopped,
            // scanning will be stopped automatically if the watcher is destroyed.
            watcher.Stop();
            // Always unregister the handlers to release the resources to prevent leaks.
            watcher.Received -= OnAdvertisementReceived;
            watcher.Stopped -= OnAdvertisementWatcherStopped;

            rootPage.NotifyUser("Navigating away. Watcher stopped.", NotifyType.StatusMessage);
            base.OnNavigatingFrom(e);
        }

        /// <summary>
        /// Invoked when application execution is being suspended.  Application state is saved
        /// without knowing whether the application will be terminated or resumed with the contents
        /// of memory still intact.
        /// </summary>
        /// <param name="sender">The source of the suspend request.</param>
        /// <param name="e">Details about the suspend request.</param>
        private void App_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            // Make sure to stop the watcher on suspend.
            watcher.Stop();
            // Always unregister the handlers to release the resources to prevent leaks.
            watcher.Received -= OnAdvertisementReceived;
            watcher.Stopped -= OnAdvertisementWatcherStopped;

            rootPage.NotifyUser("App suspending. Watcher stopped.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked when application execution is being resumed.
        /// </summary>
        /// <param name="sender">The source of the resume request.</param>
        /// <param name="e"></param>
        private void App_Resuming(object sender, object e)
        {
            watcher.Received += OnAdvertisementReceived;
            watcher.Stopped += OnAdvertisementWatcherStopped;
        }

        /// <summary>
        /// Invoked as an event handler when the Run button is pressed.
        /// </summary>
        /// <param name="sender">Instance that triggered the event.</param>
        /// <param name="e">Event data describing the conditions that led to the event.</param>
        private void RunButton_Click(object sender, RoutedEventArgs e)
        {
            // Calling watcher start will start the scanning if not already initiated by another client
            watcher.Start();

            rootPage.NotifyUser("Watcher started.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked as an event handler when the Stop button is pressed.
        /// </summary>
        /// <param name="sender">Instance that triggered the event.</param>
        /// <param name="e">Event data describing the conditions that led to the event.</param>
        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            // Stopping the watcher will stop scanning if this is the only client requesting scan
            watcher.Stop();

            rootPage.NotifyUser("Watcher stopped.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked as an event handler when an advertisement is received.
        /// </summary>
        /// <param name="watcher">Instance of watcher that triggered the event.</param>
        /// <param name="eventArgs">Event data containing information about the advertisement event.</param>
        private async void OnAdvertisementReceived(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementReceivedEventArgs eventArgs)
        {
            // We can obtain various information about the advertisement we just received by accessing 
            // the properties of the EventArgs class

            // The timestamp of the event
            DateTimeOffset timestamp = eventArgs.Timestamp;

            // The type of advertisement
            BluetoothLEAdvertisementType advertisementType = eventArgs.AdvertisementType;

            // The received signal strength indicator (RSSI)
            Int16 rssi = eventArgs.RawSignalStrengthInDBm;

            // The local name of the advertising device contained within the payload, if any
            string localName = eventArgs.Advertisement.LocalName;

            // Check if there are any manufacturer-specific sections.
            // If there is, print the raw data of the first manufacturer section (if there are multiple).
            string manufacturerDataString = "";
            var manufacturerSections = eventArgs.Advertisement.ManufacturerData;
            if (manufacturerSections.Count > 0)
            {
                // Only print the first one of the list
                var manufacturerData = manufacturerSections[0];
                var data = new byte[manufacturerData.Data.Length];
                using (var reader = DataReader.FromBuffer(manufacturerData.Data))
                {
                    reader.ReadBytes(data);
                }
                // Print the company ID + the raw data in hex format
                manufacturerDataString = string.Format("0x{0}: {1}",
                    manufacturerData.CompanyId.ToString("X"),
                    BitConverter.ToString(data));
            }

            // Serialize UI update to the main UI thread
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                // Display these information on the list
                ReceivedAdvertisementListBox.Items.Add(string.Format("[{0}]: type={1}, rssi={2}, name={3}, manufacturerData=[{4}]",
                    timestamp.ToString("hh\\:mm\\:ss\\.fff"),
                    advertisementType.ToString(),
                    rssi.ToString(),
                    localName,
                    manufacturerDataString));
            });
        }

        /// <summary>
        /// Invoked as an event handler when the watcher is stopped or aborted.
        /// </summary>
        /// <param name="watcher">Instance of watcher that triggered the event.</param>
        /// <param name="eventArgs">Event data containing information about why the watcher stopped or aborted.</param>
        private async void OnAdvertisementWatcherStopped(BluetoothLEAdvertisementWatcher watcher, BluetoothLEAdvertisementWatcherStoppedEventArgs eventArgs)
        {
            // Notify the user that the watcher was stopped
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                rootPage.NotifyUser(string.Format("Watcher stopped or aborted: {0}", eventArgs.Error.ToString()), NotifyType.StatusMessage);
            });
        }
    }
}
