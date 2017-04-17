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
        // read the reports from the trigger details
        var reports = details.readReports();
        var values = Windows.Storage.ApplicationData.current.localSettings.values;

        // cache details from this trigger in application data
        values["ReportCount"] = reports.length.toString();
        if(reports.length > 0) {
            var lastReading = reports[reports.length - 1].reading;
            var options = {
                year: "numeric", month: "2-digit",
                day: "numeric", hour: "2-digit", minute: "2-digit", second: "2-digit"
            };
            for (var activity in Windows.Devices.Sensors.ActivityType) {
                if (Windows.Devices.Sensors.ActivityType[activity] == lastReading.activity) {
                    values["LastActivity"] = activity.toString();
                }
            }
            for (var confidence in Windows.Devices.Sensors.ActivitySensorReadingConfidence) {
                if (Windows.Devices.Sensors.ActivitySensorReadingConfidence[confidence] == lastReading.confidence) {
                    values["LastConfidence"] = confidence.toString();
                }
            }
            values["LastTimestamp"] = lastReading.timestamp.toLocaleTimeString("en-US", options);
        } else {
            values["LastActivity"] = "No data";
            values["LastConfidence"] = "No data";
            values["LastTimestamp"] = "No data";
        }

        var dt = new Date();
        values["TaskStatus"] = "Completed at " + dt.toLocaleTimeString("en-US", options);
        // A JavaScript background task must call close when done
        close();
    }

    run();
})();
