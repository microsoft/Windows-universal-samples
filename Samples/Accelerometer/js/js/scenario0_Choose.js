//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var accelerometer;

    // DOM elements
    var reportTypeComboBox;

    var page = WinJS.UI.Pages.define("/html/scenario0_Choose.html", {
        ready: function (element, options) {
            reportTypeComboBox = document.getElementById("reportTypeComboBox");
            
            reportTypeComboBox.selectedIndex = Array.prototype.findIndex.call(reportTypeComboBox.options, function (o) {
                return o.value == SdkSample.accelerometerReadingType;
            });
        },
        unload: function () {
            SdkSample.accelerometerReadingType = reportTypeComboBox.options[reportTypeComboBox.selectedIndex].value;
        }
    });
})();
