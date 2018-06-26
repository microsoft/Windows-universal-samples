//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    // Instance of WinRT component that implements
    // RPC metering service client
    var rpcClient = window.RpcClientRt && new RpcClientRt.RpcClient;
    var meteringOn = false;
    var meteringPageInForeground = false;
    var samplePeriod = 100;
    var meteringSuspended = false;
    var meteringIntervalId = 0;
    var meteringCallbackCount = 0;
    var lastMeteringCallbackTime = 0;

    function updateStartStopButtons() {
        document.getElementById("StartButton").disabled = meteringOn;
        document.getElementById("StopButton").disabled = !meteringOn;
    }

    var page = WinJS.UI.Pages.define("/html/Scenario1_MeteringData.html", {
        ready: function (element, options) {
            // Attach event handlers to html input elements
            document.getElementById("StartButton").addEventListener("click", onStartButtonClick, false);
            document.getElementById("StopButton").addEventListener("click", onStopButtonClick, false);
            document.getElementById("SamplePeriodSlider").addEventListener("change", onSampleSliderChange, false);

            // Set up initial state
            document.getElementById("SamplePeriodSlider").value = samplePeriod;
            updateStartStopButtons();
            document.getElementById("SamplePeriod").innerHTML = "Sample Period (ms): " + samplePeriod;

            // Attach handlers for suspension to stop the watcher when the App is suspended.
            Windows.UI.WebUI.WebUIApplication.addEventListener("suspending", onSuspending, false);
            Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", onResuming, false);
            meteringPageInForeground = true;
        },
        unload: function (element, options) {
            // Remove local suspension handlers from the App since this page is no longer active.
            Windows.UI.WebUI.WebUIApplication.removeEventListener("suspending", onSuspending);
            Windows.UI.WebUI.WebUIApplication.removeEventListener("resuming", onResuming);

            meteringPageInForeground = false;
        }
    });

    /// <summary>
    /// Invoked when application execution is being suspended.  Application state is saved
    /// without knowing whether the application will be terminated or resumed with the contents
    /// of memory still intact.
    /// </summary>
    /// <param name="args">Details about the suspend request.</param>
    function onSuspending(args) {
        // Make sure to stop metering if it was running and
        // set a flag that would indicate it
        if (meteringOn) {
            meteringSuspended = true;
            onStopButtonClick(null);
        }
    }

    /// <summary>
    /// Invoked when application execution is being resumed.
    /// </summary>
    /// <param name="args"></param>
    function onResuming(args) {
        // If metering was running during suspension
        // then restart it
        if (meteringSuspended) {
            meteringSuspended = false;
            onStartButtonClick(args);
        }
    }

    /// <summary>
    /// Returns a promise which checks for metering status 
    /// every 100 ms and updates the metering data in html
    /// </summary>
    function CheckMeteringStatusAsync() {
        return new WinJS.Promise(function (c) {
            lastMeteringCallbackTime = (new Date()).getTime();
            meteringIntervalId = setInterval(() => {
                var ret = rpcClient.getMeteringStatus();
                if (ret) {
                    meteringOn = false;

                    if (meteringPageInForeground) {
                        updateStartStopButtons();
                        WinJS.log && WinJS.log("Error occured while communicating with RPC server: " + ret, "sample", "error");
                    }
                    clearInterval(meteringIntervalId);
                    meteringIntervalId = 0;
                }
                var currentCallbackCount = rpcClient.getCallbackCount();
                if (currentCallbackCount !== meteringCallbackCount) {
                    
                    var now = (new Date()).getTime();
                    var diff = now - lastMeteringCallbackTime;
                    lastMeteringCallbackTime = now;

                    if (meteringPageInForeground) {
                        document.getElementById("SampleMessage").innerHTML = rpcClient.getMeteringData();
                        document.getElementById("ExpectedSampleRate").innerHTML = "Expected Sample Rate: " + 1000 / samplePeriod;
                        document.getElementById("ActualSampleRate").innerHTML = "Actual Sample Rate: " + (currentCallbackCount - meteringCallbackCount) * 1000 / diff;
                    }
                    meteringCallbackCount = currentCallbackCount;
                }
            }, 100);
        });
    }

    // Called when the user drags the scale factor slider control.
    function onSampleSliderChange(args) {
        samplePeriod = Math.floor(document.getElementById("SamplePeriodSlider").value);
        document.getElementById("SamplePeriod").innerHTML = "Sample Period (ms): " + samplePeriod;
        if (meteringOn) {
            var ret = rpcClient.setSampleRate(samplePeriod);
            if (ret) {
                WinJS.log && WinJS.log("Error occured while communicating with RPC server: " + ret, "sample", "error");
            }
            else {
                WinJS.log && WinJS.log("Sample period set to: " + samplePeriod, "sample", "status");
            }
        }
    }

    /// <summary>
    /// Invoked as an event handler when the Run button is pressed.
    /// </summary>
    /// <param name="args">Event data describing the conditions that led to the event.</param>
    function onStartButtonClick(args) {
        meteringOn = true;
        updateStartStopButtons();
        samplePeriod = Math.floor(document.getElementById("SamplePeriodSlider").value);
        if (rpcClient) {
            rpcClient.startMeteringAsync(samplePeriod);
            CheckMeteringStatusAsync();

            WinJS.log && WinJS.log("Metering start command sent successfully", "sample", "status");
        } else {
            WinJS.log && WinJS.log("NT service support has not been compiled for this architecture.", "sample", "error");
        }
    }

    /// <summary>
    /// Invoked as an event handler when the Stop button is pressed.
    /// </summary>
    /// <param name="args">Event data describing the conditions that led to the event.</param>
    function onStopButtonClick(args) {
        clearInterval(meteringIntervalId);
        meteringIntervalId = 0;
        if (rpcClient) {
            var ret = rpcClient.stopMeteringData();
            if (ret) {
                WinJS.log && WinJS.log("Error occured while communicating with RPC server: " + ret, "sample", "error");
            } else {
                WinJS.log && WinJS.log("Metering stop command sent successfully", "sample", "status");
            }
        }

        meteringOn = false;
        updateStartStopButtons();
    }
})();
