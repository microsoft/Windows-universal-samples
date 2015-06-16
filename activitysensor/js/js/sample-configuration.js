//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Sample Title here";

    var scenarios = [
        { url: "/html/scenario1_currentActivity.html", title: "Current Activity" },
        { url: "/html/scenario2_history.html", title: "History" },
        { url: "/html/scenario3_changeEvents.html", title: "Events" },
        { url: "/html/scenario4_backgroundActivity.html", title: "Background Activity" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });

    WinJS.Namespace.define("ActivitySensorSdkSample", {
        /// <summary>
        /// Converts an ActivityType to string.
        /// </summary>
        /// <param name="activityType"></param>
        activityTypeToString: function (activityType) {
            var result = "";
            switch (activityType) {
                case Windows.Devices.Sensors.ActivityType.unknown:
                    result = "Unknown";
                    break;
                case Windows.Devices.Sensors.ActivityType.stationary:
                    result = "Stationary";
                    break;
                case Windows.Devices.Sensors.ActivityType.fidgeting:
                    result = "Fidgeting";
                    break;
                case Windows.Devices.Sensors.ActivityType.walking:
                    result = "Walking";
                    break;
                case Windows.Devices.Sensors.ActivityType.running:
                    result = "Running";
                    break;
                case Windows.Devices.Sensors.ActivityType.inVehicle:
                    result = "In vehicle";
                    break;
                case Windows.Devices.Sensors.ActivityType.biking:
                    result = "Biking";
                    break;
                default:
                    result = "Unrecognized activity type: " + activityType;
                    break;
            }
            return result;
        },

        /// <summary>
        /// Converts an ActivitySensorReadingConfidence to string.
        /// </summary>
        /// <param name="readingConfidence"></param>
        readingConfidenceToString: function (readingConfidence) {
            var result = "";
            switch (readingConfidence) {
                case Windows.Devices.Sensors.ActivitySensorReadingConfidence.high:
                    result = "High";
                    break;
                case Windows.Devices.Sensors.ActivitySensorReadingConfidence.low:
                    result = "Low";
                    break;
                default:
                    result = "Unrecognized reading confidence: " + readingConfidence;
                    break;
            }
            return result;
        },
    });
})();