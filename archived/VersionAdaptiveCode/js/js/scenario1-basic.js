//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ApiInformation = Windows.Foundation.Metadata.ApiInformation;
    var OrientationSensor = Windows.Devices.Sensors.OrientationSensor;
    var BackgroundExecutionManager = Windows.ApplicationModel.Background.BackgroundExecutionManager;
    var BackgroundAccessStatus = Windows.ApplicationModel.Background.BackgroundAccessStatus;

    var cameraButtonPressCount = 0;

    // Determining presence requires scanning the system metadata.
    // Instead of repeating the query, calculate the result once and cache it.
    // Note that ApiInformation uses the native (capitalized) names.
    //
    // Does the HardwareButtons class have a cameraPressed event?
    var hasCameraButton =
        ApiInformation.isEventPresent("Windows.Phone.UI.Input.HardwareButtons", "CameraPressed");

    // Does the OrientationSensor class have a getDefault method that takes two parameters?
    var hasOrientationWithOptimizationGoal =
        ApiInformation.isMethodPresent("Windows.Devices.Sensors.OrientationSensor", "GetDefault", 2);

    // Does the BackgroundAccessStatus enum have an alwaysAllowed value?
    var hasAlwaysAllowedAccess =
        ApiInformation.isEnumNamedValuePresent("Windows.ApplicationModel.Background.BackgroundAccessStatus", "AlwaysAllowed");

    // We create abbreviations for types only after verifying that the types exist.
    var HardwareButtons = hasCameraButton && Windows.Phone.UI.Input;
    var SensorReadingType = hasOrientationWithOptimizationGoal && Windows.Devices.Sensors.SensorReadingType;
    var SensorOptimizationGoal = hasOrientationWithOptimizationGoal && Windows.Devices.Sensors.SensorOptimizationGoal;

    var page = WinJS.UI.Pages.define("/html/scenario1-basic.html", {
        ready: function (element, options) {
            // If the HardwareButtons.CameraPressed event exists, then register a handler for it.
            if (hasCameraButton) {
                HardwareButtons.addEventListener("camerapressed", onCameraPressed);
            }

            getOrientationButton.addEventListener("click", getOrientationReport);
            checkBackgroundAccessButton.addEventListener("click", checkBackgroundAccess);
        },

        unload: function () {
            // If the HardwareButtons.CameraPressed event exists, then unregister our handler.
            if (hasCameraButton) {
                Windows.Phone.UI.Input.HardwareButtons.removeEventListener("camerapressed", onCameraPressed);
            }
        }
    });

    function onCameraPressed() {
        cameraButtonPressCount++;
        WinJS.log && WinJS.log("Camera button press #" + cameraButtonPressCount, "samples", "status");
    }
    
    // For more information on using orientation sensors, see the OrientationSensor sample.
    function getOrientationReport() {
        var sensor;
        // If there is a version of GetDefault that lets us specify the optimization goal,
        // then use it. Otherwise, use the regular version.
        if (hasOrientationWithOptimizationGoal) {
            sensor = OrientationSensor.getDefault(SensorReadingType.absolute, SensorOptimizationGoal.powerEfficiency);
        } else {
            sensor = OrientationSensor.getDefault();
        }

        if (sensor) {
            var reading = sensor.getCurrentReading();
            var quaternion = reading.quaternion;
            WinJS.log && WinJS.log("X: " + quaternion.x + ", Y: " + quaternion.y +
                ", Z: " + quaternion.z + ", W: " + quaternion.w, "samples", "status");

            // Restore the default report interval to release resources while the sensor is not in use.
            sensor.reportInterval = 0;
        } else {
            WinJS.log && WinJS.log("Sorry, no sensor found", "samples", "error");
        }
    }

    // For more information on background activity, see the BackgroundActivity sample.
    function checkBackgroundAccess() {
        BackgroundExecutionManager.requestAccessAsync().done(function (status) {
            // The Windows 10 Anniversary Update introduced a new background access model.
            if (hasAlwaysAllowedAccess) {
                switch (status) {
                    case BackgroundAccessStatus.alwaysAllowed:
                    case BackgroundAccessStatus.allowedSubjectToSystemPolicy:
                        WinJS.log && WinJS.log("Background activity is allowed.", "samples", "status");
                        break;

                    case BackgroundAccessStatus.unspecified:
                    case BackgroundAccessStatus.deniedByUser:
                    case BackgroundAccessStatus.deniedBySystemPolicy:
                        WinJS.log && WinJS.log("Background activity is not allowed.", "samples", "error");
                        break;
                }
            } else {
                // Use the background access model from Windows 10 November 2015 Update and earlier.
                switch (status) {
                    case BackgroundAccessStatus.allowedMayUseActiveRealTimeConnectivity:
                    case BackgroundAccessStatus.allowedWithAlwaysOnRealTimeConnectivity:
                        WinJS.log && WinJS.log("Background activity is allowed.", "samples", "status");
                        break;

                    case BackgroundAccessStatus.unspecified:
                    case BackgroundAccessStatus.denied:
                        WinJS.log && WinJS.log("Background activity is not allowed.", "samples", "error");
                        break;
                }
            }
        });
    }

})();
