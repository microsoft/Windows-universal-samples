//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var sampleTitle = "Orientation Sensor";

    var MagnetometerAccuracy = Windows.Devices.Sensors.MagnetometerAccuracy;

    var scenarios = [
        { url: "/html/scenario0_Choose.html", title: "Choose orientation sensor" },
        { url: "/html/scenario1_DataEvents.html", title: "Data Events" },
        { url: "/html/scenario2_Polling.html", title: "Polling" },
        { url: "/html/scenario3_Calibration.html", title: "Calibration" }
    ];

    function setReadingText(e, reading) {
        var quaternion = reading.quaternion;    // get a reference to the object to avoid re-creating it for each access

        var quaternionReport = "W: " + quaternion.w.toFixed(2) +
            ", X: " + quaternion.x.toFixed(2) +
            ", Y: " + quaternion.y.toFixed(2) +
            ", Z: " + quaternion.z.toFixed(2);

        var rotationMatrix = reading.rotationMatrix;
        var rotationMatrixReport = "M11: " + rotationMatrix.m11.toFixed(2) +
            ", M12: " + rotationMatrix.m12.toFixed(2) +
            ", M13: " + rotationMatrix.m13.toFixed(2) +
            "\nM21: " + rotationMatrix.m21.toFixed(2) +
            ", M22: " + rotationMatrix.m22.toFixed(2) +
            ", M23: " + rotationMatrix.m23.toFixed(2) +
            "\nM31: " + rotationMatrix.m31.toFixed(2) +
            ", M32: " + rotationMatrix.m32.toFixed(2) +
            ", M33: " + rotationMatrix.m33.toFixed(2);

        var yawAccuracyReport;
        switch (reading.yawAccuracy) {
            case MagnetometerAccuracy.unknown:
                yawAccuracyReport = "unknown";
                break;
            case MagnetometerAccuracy.unreliable:
                yawAccuracyReport = "unreliable";
                break;
            case MagnetometerAccuracy.approximate:
                yawAccuracyReport = "approximate";
                break;
            case MagnetometerAccuracy.high:
                yawAccuracyReport = "high";
                break;
            default:
                yawAccuracyReport = "other";
                break;
        }

        e.innerText = "Quaternion:\n" + quaternionReport + "\n\n" +
            "Rotation Matrix:\n" + rotationMatrixReport + "\n\n" +
            "Yaw Accuracy:\n" + yawAccuracyReport;
    } 

    function sensorDescription() {
        return SdkSample.sensorReadingType + " orientation sensor with " + SdkSample.sensorOptimizationGoal + " optimization goal";
    }

    WinJS.Namespace.define("SdkSample", {
        sampleTitle: sampleTitle,
        scenarios: new WinJS.Binding.List(scenarios),
        setReadingText: setReadingText,
        sensorReadingType: "absolute",
        sensorOptimizationGoal: "precision",
        sensorDescription: sensorDescription
    });
})();