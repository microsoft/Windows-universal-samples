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

    var page = WinJS.UI.Pages.define("/html/scenario2_History.html", {
        ready: function (element, options) {
            document.getElementById("allHistoryRadio").addEventListener("change", allHistorySelected, false);
            document.getElementById("spanHistoryRadio").addEventListener("change", spanHistorySelected, false);
            document.getElementById("getHistoryButton").addEventListener("click", getHistory, false);

            // Read time and date picker controls for local access
            fromDate = document.getElementById("fromDate").winControl;
            fromTime = document.getElementById("fromTime").winControl;
            toDate = document.getElementById("toDate").winControl;
            toTime = document.getElementById("toTime").winControl;

        }
    });

    var historyRecords = new WinJS.Binding.List();

    var getAllHistory = true;
    var fromDate;
    var fromTime;
    var toDate;
    var toTime;

    /// <summary>
    /// Defines a class that represent a single history record and will be consumed by data template
    /// </summary>
    var HistoryRecord = WinJS.Class.define(
        function (timeStamp, stepKind, stepsCount, stepsDuration) {
            this.timeStamp = timeStamp;
            this.stepKind = stepKind;
            this.stepsCount = stepsCount;
            this.stepsDuration = stepsDuration;
        },
        {
            stepKindString: {
                get: function () {
                    var string = "";
                    switch (this.stepKind) {
                        case Windows.Devices.Sensors.PedometerStepKind.unknown:
                            string = "Unknown";
                            break;
                        case Windows.Devices.Sensors.PedometerStepKind.walking:
                            string = "Walking";
                            break;
                        case Windows.Devices.Sensors.PedometerStepKind.running:
                            string = "Running";
                            break;
                        default:
                            string = "Invalid Step Kind";
                            break;
                    }
                    return string;
                }
            }
        },
        {
        });

    // Define a namespace for exposing the history records list as data source to list view
    WinJS.Namespace.define("HistoryRecordCollection", {
        itemList: historyRecords
    });

    function allHistorySelected() {
        // hide the time span div
        document.getElementById("historyTimeSpan").style.display = "none";
        getAllHistory = true;
    }

    function spanHistorySelected() {
        // display the time span div
        document.getElementById("historyTimeSpan").style.display = "block";
        getAllHistory = false;
    }


    /// <summary>
    /// click handler for the "Get History" button.
    /// Depending on the user selection, this handler uses one of the overloaded
    /// 'GetSystemHistoryAsync' APIs to retrieve the pedometer history of the user.
    /// </summary>
    function getHistory() {
        // disable the button before we start getting history
        document.getElementById("getHistoryButton").disabled = true;

        // remove all previous content from the list
        historyRecords.splice(0, historyRecords.length);

        // Use right version of the API based on user choice
        if (getAllHistory) {
            var allHistory = new Date(0);
            WinJS.log && WinJS.log("Getting all available history", "sample", "status");
            Windows.Devices.Sensors.Pedometer.getSystemHistoryAsync(allHistory).then(function (records) {
                WinJS.log && WinJS.log("History retrieval completed", "sample", "status");
                displayRecords(records);
            });
        }
        else {
            var startTime = new Date(fromDate.current.getFullYear(), fromDate.current.getMonth(), fromDate.current.getDate(), fromTime.current.getHours(), fromTime.current.getMinutes(), fromTime.current.getSeconds(), fromTime.current.getMilliseconds());
            var endTime = new Date(toDate.current.getFullYear(), toDate.current.getMonth(), toDate.current.getDate(), toTime.current.getHours(), toTime.current.getMinutes(), toTime.current.getSeconds(), toTime.current.getMilliseconds());

            var spanMs = endTime.getTime() - startTime.getTime();
            if(spanMs < 0) {
                WinJS.log && WinJS.log("Invalid history span. 'To' time should be greater than 'From' time", "sample", "status");
            }
            else {
                WinJS.log && WinJS.log("Getting history from: " + startTime.toString() + " to " + endTime.toString(), "sample", "status");
                Windows.Devices.Sensors.Pedometer.getSystemHistoryAsync(startTime, spanMs).then(function (records) {
                    WinJS.log && WinJS.log("History retrieval completed", "sample", "status");
                    displayRecords(records);
                });
            }
        }
        // enable the button for subsequent retrievals
        document.getElementById("getHistoryButton").disabled = false;
    }



    /// <summary>
    /// Helper function that bidns the retrieved history records to the List.
    /// </summary>
    function displayRecords(records) {
        for (var index = 0; index < records.length; index++) {
            WinJS.log && WinJS.log(records.length.toString() + " index = " + index.toString(), "sample", "status");
            var reading = records[index];
            var singleRecord = new HistoryRecord(reading.timestamp, reading.stepKind, reading.cumulativeSteps, 0);
            historyRecords.push(singleRecord);
        }
        WinJS.log && WinJS.log("Retrieved " + index.toString() + " records", "sample", "status");
    }
})();
