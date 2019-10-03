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

        // read the pedometer reports from the trigger, if the trigger is one from pedometer
        if (Windows.Devices.Sensors.SensorType.pedometer == details.sensorType) {
            var reports = Windows.Devices.Sensors.Pedometer.getReadingsFromTriggerDetails(details);
            if (reports.length > 0) {
                var values = Windows.Storage.ApplicationData.current.localSettings.values;

                // start from the last record and access all readings that were reported at that timestamp
                // This gives readings of different step kinds that were last reported 
                var nthFromLast = 1;
                var lastReading = reports[reports.length - nthFromLast];
                var lastReadingTimestamp = lastReading.timestamp;

                var options = {
                    year: "numeric", month: "2-digit",
                    day: "numeric", hour: "2-digit", minute: "2-digit", second: "2-digit"
                };

                // cache details from this trigger in application data
                values["ReportCount"] = reports.length.toString();
                values["LastTimestamp"] = lastReadingTimestamp.toLocaleTimeString("en-US", options);


                // All reports that have the same timestamp corresponds to step counts 
                // of different step kinds supported by the sensor - Parse each one of 
                // them and save in the application data
                do {
                    values["StepKind" + lastReading.stepKind] = lastReading.cumulativeSteps.toString();
                    nthFromLast++;
                    lastReading = reports[reports.length - nthFromLast];
                } while (nthFromLast <= reports.length && lastReading.timestamp.getTime() === lastReadingTimestamp.getTime());

                var dt = new Date();
                values["TaskStatus"] = "Completed at " + dt.toLocaleTimeString("en-US", options);
            }
        }
        // A JavaScript background task must call close when done
        close();
    }

    run();
})();
