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

using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.Advertisement;

using SDKTemplate;

namespace BluetoothAdvertisement
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario2_Publisher : Page
    {
        // The Bluetooth LE advertisement publisher class is used to control and customize Bluetooth LE advertising.
        private BluetoothLEAdvertisementPublisher publisher;

        // A pointer back to the main page is required to display status messages.
        private MainPage rootPage;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public Scenario2_Publisher()
        {
            this.InitializeComponent();

            // Create and initialize a new publisher instance.
            publisher = new BluetoothLEAdvertisementPublisher();

            // We need to add some payload to the advertisement. A publisher without any payload
            // or with invalid ones cannot be started. We only need to configure the payload once
            // for any publisher.

            // Add a manufacturer-specific section:
            // First, let create a manufacturer data section
            var manufacturerData = new BluetoothLEManufacturerData();

            // Then, set the company ID for the manufacturer data. Here we picked an unused value: 0xFFFE
            manufacturerData.CompanyId = 0xFFFE;

            // Finally set the data payload within the manufacturer-specific section
            // Here, use a 16-bit UUID: 0x1234 -> {0x34, 0x12} (little-endian)
            var writer = new DataWriter();
            UInt16 uuidData = 0x1234;
            writer.WriteUInt16(uuidData);

            // Make sure that the buffer length can fit within an advertisement payload. Otherwise you will get an exception.
            manufacturerData.Data = writer.DetachBuffer();

            // Add the manufacturer data to the advertisement publisher:
            publisher.Advertisement.ManufacturerData.Add(manufacturerData);

            // Display the information about the published payload
            PublisherPayloadBlock.Text = string.Format("Published payload information: CompanyId=0x{0}, ManufacturerData=0x{1}",
                manufacturerData.CompanyId.ToString("X"),
                uuidData.ToString("X"));

            // Display the current status of the publisher
            PublisherStatusBlock.Text = string.Format("Published Status: {0}, Error: {1}",
                publisher.Status,
                BluetoothError.Success);
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

            // Attach a event handler to monitor the status of the publisher, which
            // can tell us whether the advertising has been serviced or is waiting to be serviced
            // due to lack of resources. It will also inform us of unexpected errors such as the Bluetooth
            // radio being turned off by the user.
            publisher.StatusChanged += OnPublisherStatusChanged;

            // Attach handlers for suspension to stop the publisher when the App is suspended.
            App.Current.Suspending += App_Suspending;
            App.Current.Resuming += App_Resuming;

            rootPage.NotifyUser("Press Run to start publisher.", NotifyType.StatusMessage);
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

            // Make sure to stop the publisher when leaving the context. Even if the publisher is not stopped,
            // advertising will be stopped automatically if the publisher is destroyed.
            publisher.Stop();
            // Always unregister the handlers to release the resources to prevent leaks.
            publisher.StatusChanged -= OnPublisherStatusChanged;

            rootPage.NotifyUser("Navigating away. Publisher stopped.", NotifyType.StatusMessage);
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
            // Make sure to stop the publisher on suspend.
            publisher.Stop();
            // Always unregister the handlers to release the resources to prevent leaks.
            publisher.StatusChanged -= OnPublisherStatusChanged;

            rootPage.NotifyUser("App suspending. Publisher stopped.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked when application execution is being resumed.
        /// </summary>
        /// <param name="sender">The source of the resume request.</param>
        /// <param name="e"></param>
        private void App_Resuming(object sender, object e)
        {
            publisher.StatusChanged += OnPublisherStatusChanged;
        }

        /// <summary>
        /// Invoked as an event handler when the Run button is pressed.
        /// </summary>
        /// <param name="sender">Instance that triggered the event.</param>
        /// <param name="e">Event data describing the conditions that led to the event.</param>
        private void RunButton_Click(object sender, RoutedEventArgs e)
        {
            // Calling publisher start will start the advertising if resources are available to do so
            publisher.Start();

            rootPage.NotifyUser("Publisher started.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked as an event handler when the Stop button is pressed.
        /// </summary>
        /// <param name="sender">Instance that triggered the event.</param>
        /// <param name="e">Event data describing the conditions that led to the event.</param>
        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            // Stopping the publisher will stop advertising the published payload
            publisher.Stop();

            rootPage.NotifyUser("Publisher stopped.", NotifyType.StatusMessage);
        }

        /// <summary>
        /// Invoked as an event handler when the status of the publisher changes.
        /// </summary>
        /// <param name="publisher">Instance of publisher that triggered the event.</param>
        /// <param name="eventArgs">Event data containing information about the publisher status change event.</param>
        private async void OnPublisherStatusChanged(
            BluetoothLEAdvertisementPublisher publisher,
            BluetoothLEAdvertisementPublisherStatusChangedEventArgs eventArgs)
        {
            // This event handler can be used to monitor the status of the publisher.
            // We can catch errors if the publisher is aborted by the system
            BluetoothLEAdvertisementPublisherStatus status = eventArgs.Status;
            BluetoothError error = eventArgs.Error;

            // Update the publisher status displayed in the sample
            // Serialize UI update to the main UI thread
            await this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                PublisherStatusBlock.Text = string.Format("Published Status: {0}, Error: {1}",
                    status.ToString(),
                    error.ToString());
            });
        }
    }
}
