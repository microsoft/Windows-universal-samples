//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var accelerometer;

    // DOM elements
    var reportTypeComboBox;
    var optimizationGoalComboBox;

    var page = WinJS.UI.Pages.define("/html/scenario0_Choose.html", {
        ready: function (element, options) {
            reportTypeComboBox = document.getElementById("reportTypeComboBox");
            optimizationGoalComboBox = document.getElementById("optimizationGoalComboBox");

            reportTypeComboBox.selectedIndex = Array.prototype.findIndex.call(reportTypeComboBox.options, function (o) {
                return o.value == SdkSample.sensorReadingType;
            });
            optimizationGoalComboBox.selectedIndex = Array.prototype.findIndex.call(optimizationGoalComboBox.options, function (o) {
                return o.value == SdkSample.sensorOptimizationGoal;
            });
        },
        unload: function () {
            SdkSample.sensorReadingType = reportTypeComboBox.options[reportTypeComboBox.selectedIndex].value;
            SdkSample.sensorOptimizationGoal = optimizationGoalComboBox.options[optimizationGoalComboBox.selectedIndex].value;
        }
    });
})();
