//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
//// This code is licensed under the MIT License (MIT).

(function () {
    "use strict";

    // This var is used to get information about the current instance of the background task.
    var backgroundTaskInstance = Windows.UI.WebUI.WebUIBackgroundTaskInstance.current;

    // Trigger details contain the information surrounding the reason why the this task was triggered
    var details = backgroundTaskInstance.triggerDetails;

    function run() {
        // If the background watcher stopped unexpectedly, an error will be available here.
        var error = details.error;

        // The Advertisements property is a list of all advertisement events received
        // since the last task triggered. The list of advertisements here might be valid even if
        // the Error status is not Success since advertisements are stored until this task is triggered.
        var advertisements = details.advertisements;

        // The signal strength filter configuration of the trigger is returned such that further 
        // processing can be performed here using these values if necessary. They are read-only here.
        var rssiFilter = details.signalStrengthFilter;

        // In this example, the background task simply constructs a message communicated
        // to the App. For more interesting applications, a notification can be sent from here instead.
        var eventMessage = "ErrorStatus: " + error.toString() + ", EventCount: " + advertisements.size.toString();
        eventMessage += ", HighDBm: " + rssiFilter.inRangeThresholdInDBm.toString() + ", LowDBm: " + rssiFilter.outOfRangeThresholdInDBm.toString();
        eventMessage += ", Timeout: " + rssiFilter.outOfRangeTimeout.toString() + ", Sampling: " + rssiFilter.samplingInterval.toString();

        // Advertisements can contain multiple events that were aggregated, each represented by 
        // a BluetoothLEAdvertisementReceivedEventArgs object.
        var iter = advertisements.first();
        while (iter.hasCurrent) {
            var eventArgs = iter.current;

            // Check if there are any manufacturer-specific sections.
            // If there is, print the raw data of the first manufacturer section (if there are multiple).
            var manufacturerDataString = "";
            var manufacturerSections = eventArgs.advertisement.manufacturerData;
            if (manufacturerSections.size > 0) {
                var manufacturerData = manufacturerSections[0];
                var data = new Array(manufacturerData.data.length);
                var reader = Windows.Storage.Streams.DataReader.fromBuffer(manufacturerData.data);
                reader.readBytes(data);

                // Print the company ID + the raw data in hex format
                manufacturerDataString += "0x" + manufacturerData.companyId.toString(16) + ": ";
                data.forEach(
                    function buildString(value) { manufacturerDataString += value.toString(16) + " "; }
                );
            }
            eventMessage += "\n[" + eventArgs.timestamp.getHours() + ":" + eventArgs.timestamp.getMinutes() +
                ":" + eventArgs.timestamp.getSeconds() + "]: type=" + eventArgs.advertisementType.toString() + 
                ", rssi=" + eventArgs.rawSignalStrengthInDBm.toString() + ", name=" + eventArgs.advertisement.localName + 
                ", manufacturerData=[" + manufacturerDataString + "]";

            iter.moveNext();
        }

        // Store the message in a local settings indexed by this task's name so that the foreground App
        // can display this message.
        Windows.Storage.ApplicationData.current.localSettings.values[backgroundTaskInstance.task.name]  = eventMessage;

        // A JavaScript background task must call close when done
        close();
    }

    run();
})();
