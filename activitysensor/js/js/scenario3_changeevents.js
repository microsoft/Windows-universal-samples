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

(function () {
    "use strict";
    var activitySensor;

    var page = WinJS.UI.Pages.define("/html/scenario3_changeEvents.html", {
        ready: function (element, options) {
            document.getElementById("scenario3EnableReadingChanged").addEventListener("click", enableReadingChangedScenario, false);
            document.getElementById("scenario3DisableReadingChanged").addEventListener("click", disableReadingChangedScenario, false);

            document.getElementById("scenario3EnableReadingChanged").disabled = false;
            document.getElementById("scenario3DisableReadingChanged").disabled = true;
        },
        unload: function () {
            if (document.getElementById("scenario3EnableReadingChanged").disabled) {
                activitySensor.removeEventListener("readingchanged", onReadingChanged);
            }
        }
    });

    /// <summary>
    /// This is the event handler for ReadingChanged events.
    /// </summary>
    /// <param name="e"></param>
    function onReadingChanged(e) {
        var reading = e.reading;

        // The event can still be in queue after unload is called,
        // so check if elements are still loaded.
        if (document.getElementById("activityOutput")) {
            document.getElementById("activityOutput").innerText = ActivitySensorSdkSample.activityTypeToString(reading.activity);
        }
        if (document.getElementById("confidenceOutput")) {
            document.getElementById("confidenceOutput").innerText = ActivitySensorSdkSample.readingConfidenceToString(reading.confidence);
        }
        if (document.getElementById("readingTimestampOutput")) {
            document.getElementById("readingTimestampOutput").innerText = reading.timestamp;
        }
    }

    /// <summary>
    /// This is the click handler for the 'Reading Changed On' button.
    /// </summary>
    function enableReadingChangedScenario() {
        getActivitySensorAsync().done(function (sensor) {
            if (sensor) {
                activitySensor = sensor;

                activitySensor.subscribedActivities.append(Windows.Devices.Sensors.ActivityType.walking);
                activitySensor.subscribedActivities.append(Windows.Devices.Sensors.ActivityType.running);
                activitySensor.subscribedActivities.append(Windows.Devices.Sensors.ActivityType.inVehicle);
                activitySensor.subscribedActivities.append(Windows.Devices.Sensors.ActivityType.biking);
                activitySensor.subscribedActivities.append(Windows.Devices.Sensors.ActivityType.fidgeting);

                activitySensor.addEventListener("readingchanged", onReadingChanged);
                document.getElementById("scenario3EnableReadingChanged").disabled = true;
                document.getElementById("scenario3DisableReadingChanged").disabled = false;
            } else {
                WinJS.log && WinJS.log("No activity sensor found", "sample", "error");
            }
        },
        function (e) {
            WinJS.log && WinJS.log(e.message, "sample", "error");
        });
    }

    /// <summary>
    /// This is the click handler for the 'Reading Changed Off' button.
    /// </summary>
    function disableReadingChangedScenario() {
        activitySensor.removeEventListener("readingchanged", onReadingChanged);
        document.getElementById("scenario3EnableReadingChanged").disabled = false;
        document.getElementById("scenario3DisableReadingChanged").disabled = true;
    }

    /// <summary>
    /// Asynchronously returns the default sensor.
    /// </summary>
    function getActivitySensorAsync() {
        if (!activitySensor) {
            return Windows.Devices.Sensors.ActivitySensor.getDefaultAsync();
        }

        // Activity sensor is already initialized
        return new WinJS.Promise(function (success) {
            success(activitySensor);
        });
    }

})();
