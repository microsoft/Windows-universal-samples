//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var getHistoryButton;
    var historyTimeSpan;

    var fromDate;
    var fromTime;
    var toDate;
    var toTime;

    var page = WinJS.UI.Pages.define("/html/scenario2-history.html", {
        ready: function (element, options) {
            getHistoryButton = document.getElementById("getHistoryButton");
            historyTimeSpan = document.getElementById("historyTimeSpan");

            document.getElementById("allHistoryRadio").addEventListener("change", allHistorySelected, false);
            document.getElementById("spanHistoryRadio").addEventListener("change", spanHistorySelected, false);
            getHistoryButton.addEventListener("click", getHistory, false);

            // Read time and date picker controls for local access
            fromDate = document.getElementById("fromDate").winControl;
            fromTime = document.getElementById("fromTime").winControl;
            toDate = document.getElementById("toDate").winControl;
            toTime = document.getElementById("toTime").winControl;
        }
    });

    var historyRecords = new WinJS.Binding.List();

    var getAllHistory = true;

    /// <summary>
    /// Defines a class that represent a single history record and will be consumed by data template
    /// </summary>
    var HistoryRecord = WinJS.Class.define(
        function (timestamp, stepKind, stepsCount, stepsDuration) {
            this.timestamp = SdkSample.formatTimestamp(timestamp);
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
                            string = "Unknown step kind";
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
        historyTimeSpan.style.display = "none";
        getAllHistory = true;
    }

    function spanHistorySelected() {
        // display the time span div
        historyTimeSpan.style.display = "block";
        getAllHistory = false;
    }

    function getTimeFromElements(date, time) {
        return new Date(date.current.getFullYear(), date.current.getMonth(), date.current.getDate(), time.current.getHours(), time.current.getMinutes(), time.current.getSeconds(), time.current.getMilliseconds());
    }
    /// <summary>
    /// click handler for the "Get History" button.
    /// Depending on the user selection, this handler uses one of the overloaded
    /// 'GetSystemHistoryAsync' APIs to retrieve the pedometer history of the user.
    /// </summary>
    function getHistory() {
        // disable the button before we start getting history
        getHistoryButton.disabled = true;

        // remove all previous content from the list
        historyRecords.splice(0, historyRecords.length);

        var promise;

        // Use right version of the API based on user choice
        if (getAllHistory) {
            var allHistory = new Date(0);
            WinJS.log && WinJS.log("Getting all available history", "sample", "status");
            promise = Windows.Devices.Sensors.Pedometer.getSystemHistoryAsync(allHistory);
        } else {
            var startTime = getTimeFromElements(fromDate, fromTime);
            var endTime = getTimeFromElements(toDate, toTime);

            var spanMs = endTime.getTime() - startTime.getTime();
            if (spanMs < 0) {
                WinJS.log && WinJS.log("Invalid history span. 'To' time should be greater than 'From' time", "sample", "status");
            } else {
                WinJS.log && WinJS.log("Getting history from: " + startTime.toString() + " to " + endTime.toString(), "sample", "status");
                promise = Windows.Devices.Sensors.Pedometer.getSystemHistoryAsync(startTime, spanMs);
            }
        }

        if (promise) {
            promise = promise.then(function (records) {
                records.forEach(function (reading) {
                    var singleRecord = new HistoryRecord(reading.timestamp, reading.stepKind, reading.cumulativeSteps, 0);
                    historyRecords.push(singleRecord);
                });
                WinJS.log && WinJS.log("Records retrieved: " + records.length, "sample", "status");
            }, function error(e) {
                WinJS.log && WinJS.log("Error when opening default pedometer. " + e.message, "sample", "error");
            });
        } else {
            promise = WinJS.Promise.wrap();
        }
        promise.done(function () {
            // enable the button for subsequent retrievals
            getHistoryButton.disabled = false;
        });
    }
})();
