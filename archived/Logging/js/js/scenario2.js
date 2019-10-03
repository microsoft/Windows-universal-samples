//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("doScenarioButton").addEventListener("click", doScenario, false);
            document.getElementById("enableDisableLoggingButton").addEventListener("click", enableDisableLogging, false);

            var scenario = LoggingSessionScenario.LoggingSessionScenario.instance;
            // This sample UI is interested in events from
            // the LoggingSessionScenario class so the UI can be updated.
            scenario.addEventListener("statusChanged", onStatusChanged);
            scenario.resumeLoggingIfApplicable();
            updateControls();
        }
    });

    function resizeTextAreaWidth(ta) {
        var lines = ta.value.split("\n");
        var widestLine = 0;
        for (var i = 0; i < lines.length; i++) {
            if (lines[i].length > widestLine) {
                widestLine = lines[i].length;
            }
        }
        ta.cols = widestLine;
    }

    function getJustFileName(path) {
        var extractJustFileNamePattern = new RegExp("[\\\\\\/]([^\\\\\\/]+)$", "i");
        var matches = extractJustFileNamePattern.exec(path);
        if (matches == null) {
            return "";
        }
        return matches[1];
    }

    function getJustDirectoryName(path) {
        var extractJustFileNamePattern = new RegExp("(.*)[\\\\\\/]([^\\\\\\/]+)$", "i");
        var matches = extractJustFileNamePattern.exec(path);
        if (matches == null) {
            return "";
        }
        return matches[1];
    }

    // Add a message to the UI control which displays status while the sample is running.
    function addMessage(message) {
        statusMessageList.value += message + "\n";
        statusMessageList.scrollTop = statusMessageList.scrollHeight;
    }

    // Updates the UI with status information when a new log file is created.
    function addLogFileMessage(message, logFilePath) {
        var finalMessage;
        if (logFilePath !== null && logFilePath.length > 0) {

            finalMessage = message + ": " + getJustFileName(logFilePath);

            appLogFolder.hidden = false;
            appLogFolder.value = getJustDirectoryName(logFilePath);
            resizeTextAreaWidth(appLogFolder);

            viewLogInfo.hidden = false;
            viewLogInfo.value =
                "- View with tracerpt: tracerpt.exe \"" +
                logFilePath + "\" -of XML -o LogFile.xml\r\n" +
                "- View with Windows Performance Toolkit (WPT): wpa.exe \"" +
                logFilePath + "\"";
            resizeTextAreaWidth(viewLogInfo);
        } else {
            finalMessage = message + ": none, nothing logged since saving the last file.";
        }
        addMessage(finalMessage);
    }

    // For this sample, the logging sample code is in LoggingSessionScenario.
    // The following method handles status events from LoggingSessionScenario as
    // it runs the scenario.
    function onStatusChanged(args) {
        var eventData = args.detail;
        switch (eventData.type) {
            case "BusyStatusChanged":
                updateControls();
                break;
            case "LogFileGenerated":
                addLogFileMessage("LogFileGenerated", eventData.logFilePath);
                break;
            case "LoggingEnabledDisabled":
                addMessage("Logging has been " + (eventData.enabled ? "enabled" : "disabled") + ".");
                break;
        }
    }

    // Adjust UI controls based on what the sample is doing.
    function updateControls() {

        var scenario = LoggingSessionScenario.LoggingSessionScenario.instance;

        if (scenario.isLoggingEnabled) {

            inputText.innerText = "Logging is enabled. Click 'Disable Logging' to disable logging. With logging enabled, you can click 'Log Messages' to use the logging API to generate log files.";
            enableDisableLoggingButton.innerText = "Disable Logging";
            if (scenario.getBusyStatus()) {
                doScenarioButton.disabled = true;
                enableDisableLoggingButton.disabled = true;
            } else {
                doScenarioButton.disabled = false;
                enableDisableLoggingButton.disabled = false;
            }
        } else {

            inputText.innerText = "Logging is disabled. Click 'Enable Logging' to enable logging. After you enable logging you can click 'Log Messages' to use the logging API to generate log files.";
            enableDisableLoggingButton.innerText = "Enable Logging";
            doScenarioButton.disabled = true;
            if (scenario.getBusyStatus()) {
                enableDisableLoggingButton.disabled = true;
            } else {
                enableDisableLoggingButton.disabled = false;
            }
        }
    }

    // Enabled/disabled logging.
    function enableDisableLogging() {

        var scenario = LoggingSessionScenario.LoggingSessionScenario.instance;

        if (scenario.isLoggingEnabled) {
            WinJS.log && WinJS.log("Disabling logging...", "sample", "status");
        } else {
            WinJS.log && WinJS.log("Enabling logging...", "sample", "status");
        }

        scenario.toggleLoggingEnabledDisabled();

        if (scenario.isLoggingEnabled) {
            WinJS.log && WinJS.log("Logging enabled.", "sample", "status");
        } else {
            WinJS.log && WinJS.log("Logging disabled.", "sample", "status");
        }

        updateControls();
    }

    // Run a sample scenario which logs lots of messages to produce several log files.
    function doScenario() {
        WinJS.log && WinJS.log("The scenario is running.", "sample", "status");
        LoggingSessionScenario.LoggingSessionScenario.instance.doScenarioAsync().then(function () {
            WinJS.log && WinJS.log("The scenario is finished.", "sample", "status");
            updateControls();
        });
    }

})();
