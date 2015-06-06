//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    // The Bluetooth LE advertisement publisher class is used to control and customize Bluetooth LE advertising.
    // Create and initialize a new publisher instance.
    var publisher = new Windows.Devices.Bluetooth.Advertisement.BluetoothLEAdvertisementPublisher();

    // We need to add some payload to the advertisement. A publisher without any payload
    // or with invalid ones cannot be started. We only need to configure the payload once
    // for any publisher.

    // Add a manufacturer-specific section:
    // First, let create a manufacturer data section.
    var manufacturerData = new Windows.Devices.Bluetooth.Advertisement.BluetoothLEManufacturerData();

    // Then, set the company ID for the manufacturer data. Here we picked an unused value: 0xFFFE
    manufacturerData.companyId = 0xFFFE;

    // Finally set the data payload within the manufacturer-specific section.
    // Here, use a 16-bit UUID: 0x1234 -> {0x34, 0x12} (little-endian).
    var writer = new Windows.Storage.Streams.DataWriter();
    var uuidData = 0x1234;
    writer.writeUInt16(uuidData);

    // Make sure that the buffer length can fit within an advertisement payload. Otherwise you will get an exception.
    manufacturerData.data = writer.detachBuffer();

    // Add the manufacturer data to the advertisement publisher:
    publisher.advertisement.manufacturerData.append(manufacturerData);

    var page = WinJS.UI.Pages.define("/html/scenario2_Publisher.html", {
        ready: function (element, options) {
            document.getElementById("runButton").addEventListener("click", onRunButtonClick, false);
            document.getElementById("stopButton").addEventListener("click", onStopButtonClick, false);

            // Attach a event handler to monitor the status of the publisher, which
            // can tell us whether the advertising has been serviced or is waiting to be serviced
            // due to lack of resources. It will also inform us of unexpected errors such as the Bluetooth
            // radio being turned off by the user.
            publisher.addEventListener("statuschanged", onPublisherStatusChanged, false);

            // Attach handlers for suspension to stop the publisher when the App is suspended.
            Windows.UI.WebUI.WebUIApplication.addEventListener("suspending", onSuspending, false);
            Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", onResuming, false);

            // Display the information about the published payload
            document.getElementById("publisherPayload").innerText = "Published payload information: CompanyId=0x" + 
                manufacturerData.companyId.toString(16) + ", ManufacturerData=0x" + uuidData.toString(16);

            // Display the current status of the publisher
            document.getElementById("publisherStatus").innerText = "Published Status: " + publisher.status.toString() + ", Error: Success";

            WinJS.log && WinJS.log("Press Run to start publisher.", "sample", "status");
        },
        unload: function (element, options) {
            // Remove local suspension handlers from the App since this page is no longer active.
            Windows.UI.WebUI.WebUIApplication.removeEventListener("suspending", onSuspending);
            Windows.UI.WebUI.WebUIApplication.removeEventListener("resuming", onResuming);

            // Make sure to stop the publisher when leaving the context. Even if the publisher is not stopped,
            // advertising will be stopped automatically if the publisher is destroyed.
            publisher.stop();

            // Always unregister the handlers to release the resources to prevent leaks.
            publisher.removeEventListener("statuschanged", onPublisherStatusChanged);

            WinJS.log && WinJS.log("Navigating away. Publisher stopped.", "sample", "status");
        }
    });

    /// <summary>
    /// Invoked when application execution is being suspended.  Application state is saved
    /// without knowing whether the application will be terminated or resumed with the contents
    /// of memory still intact.
    /// </summary>
    /// <param name="args">Details about the suspend request.</param>
    function onSuspending(args) {
        // Make sure to stop the publisher on suspend.
        publisher.stop();
        // Always unregister the handlers to release the resources to prevent leaks.
        publisher.removeEventListener("statuschanged", onPublisherStatusChanged);

        WinJS.log && WinJS.log("App suspending. Publisher stopped.", "sample", "status");
    }

    /// <summary>
    /// Invoked when application execution is being resumed.
    /// </summary>
    /// <param name="args"></param>
    function onResuming(args) {
        publisher.addEventListener("statuschanged", onPublisherStatusChanged, false);
    }

    /// <summary>
    /// Invoked as an event handler when the Run button is pressed.
    /// </summary>
    /// <param name="args">Event data describing the conditions that led to the event.</param>
    function onRunButtonClick(args) {
        // Calling publisher start will start the advertising if resources are available to do so
        publisher.start();

        WinJS.log && WinJS.log("Publisher started.", "sample", "status");
    }

    /// <summary>
    /// Invoked as an event handler when the Stop button is pressed.
    /// </summary>
    /// <param name="args">Event data describing the conditions that led to the event.</param>
    function onStopButtonClick(args) {
        // Stopping the publisher will stop advertising the published payload
        publisher.stop();

        WinJS.log && WinJS.log("Publisher stopped.", "sample", "status");
    }

    /// <summary>
    /// Invoked as an event handler when the status of the publisher changes.
    /// </summary>
    /// <param name="eventArgs">Event data containing information about the publisher status change event.</param>
    function onPublisherStatusChanged(eventArgs) {
        // This event handler can be used to monitor the status of the publisher.
        // We can catch errors if the publisher is aborted by the system.
        var status = eventArgs.status;
        var error = eventArgs.error;

        // Update the publisher status displayed in the sample.
        document.getElementById("publisherStatus").innerText = "Published Status: " + status.toString() + ", Error: " + error.toString();
    }
})();
