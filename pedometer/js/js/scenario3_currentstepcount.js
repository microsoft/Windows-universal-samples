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
    var page = WinJS.UI.Pages.define("/html/scenario3_CurrentStepCount.html", {
        ready: function (element, options) {
            document.getElementById("getStepCount").addEventListener("click", getCurrentStepCount, false);
        }
    });

    /// <summary>
    /// Invoked when 'getStepCount' button is clicked.
    /// 'ReadingChanged' will not be fired when there is no activity on the pedometer 
    /// and hence can't be reliably used to get the current step count. This handler makes
    /// use of pedometer history on the system to get the current step count of the parameter
    /// </summary>
    function getCurrentStepCount() {
        
        document.getElementById("getStepCount").disabled = true;
        var allHistory = new Date(0);
        WinJS.log && WinJS.log("Getting all available history", "sample", "status");
        Windows.Devices.Sensors.Pedometer.getSystemHistoryAsync(allHistory).then(function (records) {

            WinJS.log && WinJS.log("History retrieval completed", "sample", "status");

            // History always returns chronological list of step counts for all PedometerStepKinds
            // And each record represents cumulative step counts for that step kind.
            // So we will use the last set of records - that gives us the cumulative step count for each kind
            var lastIndex = records.length - 1;
            var totalStepsCount = 0;

            document.getElementById("readingTimeStamp").innerText = records[lastIndex].timestamp.toString();

            for(var stepKind = Windows.Devices.Sensors.PedometerStepKind.running;
                stepKind >=  Windows.Devices.Sensors.PedometerStepKind.unknown;
                stepKind--) {
                totalStepsCount += records[lastIndex].cumulativeSteps;
                lastIndex--;
            }
            document.getElementById("totalStepCount").innerText = totalStepsCount.toString();
        });

        document.getElementById("getStepCount").disabled = false;
    }

})();
