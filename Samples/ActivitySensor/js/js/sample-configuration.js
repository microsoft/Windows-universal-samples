//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Activity Sensor JS Sample";

    var scenarios = [
        { url: "/html/scenario1-currentActivity.html", title: "Current activity" },
        { url: "/html/scenario2-history.html", title: "History" },
        { url: "/html/scenario3-changeEvents.html", title: "Events" },
        { url: "/html/scenario4-backgroundActivity.html", title: "Background activity" }
    ];

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios)
    });

    /// <summary>
    /// Converts an ActivityType to string.
    /// </summary>
    /// <param name="activityType"></param>
    function activityTypeToString(activityType) {
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
    }

    /// <summary>
    /// Converts an ActivitySensorReadingConfidence to string.
    /// </summary>
    /// <param name="readingConfidence"></param>
    function readingConfidenceToString(readingConfidence) {
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
    }

    var dateTimeFormatter = new Windows.Globalization.DateTimeFormatting.DateTimeFormatter("shortdate longtime");

    WinJS.Namespace.define("ActivitySensorSdkSample", {
        /// <summary>
        /// Formats an activity reading into a series of named child elements.
        /// </summary>
        /// <param name="reading"></param>
        /// <param name="parentElement"></param>
        /// <param name="activitySelector"></param>
        /// <param name="confidenceSelector"></param>
        /// <param name="timestampSelector"></param>
        formatActivity: function formatActivity(reading, parentElement, activitySelector, confidenceSelector, timestampSelector) {
            parentElement.querySelector(activitySelector).innerText = activityTypeToString(reading.activity);
            parentElement.querySelector(confidenceSelector).innerText = readingConfidenceToString(reading.confidence);
            parentElement.querySelector(timestampSelector).innerText = dateTimeFormatter.format(reading.timestamp);
        },
    });
})();