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

        // read the proximity reports from the trigger if the trigger is of the proximity sensor
        if (Windows.Devices.Sensors.SensorType.proximitySensor == details.sensorType) {
            var reports = Windows.Devices.Sensors.ProximitySensor.getReadingsFromTriggerDetails(details);
            if (reports.length > 0) {
                var values = Windows.Storage.ApplicationData.current.localSettings.values;
                var lastReading = reports[reports.length - 1];
                var options = {
                    year: "numeric", month: "2-digit",
                    day: "numeric", hour: "2-digit", minute: "2-digit", second: "2-digit"
                };

                // cache details from this trigger in application data
                values["ReportCount"] = reports.length.toString();
                values["LastTimestamp"] = lastReading.timestamp.toLocaleTimeString("en-US", options);
                values["Detected"] = lastReading.isDetected.toString();
                var dt = new Date();
                values["TaskStatus"] = "Completed at " + dt.toLocaleTimeString("en-US", options);
            }
        }
        // A JavaScript background task must call close when done
        close();
    }

    run();
})();
