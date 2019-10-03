//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    function getTimeStamp() {
        var d = new Date();
        var ts = String("0" + d.getYear()).slice(-2) +
                 String("0" + d.getMonth()).slice(-2) +
                 String("0" + d.getDay()).slice(-2) +
                 "-" +
                 String("0" + d.getHours()).slice(-2) +
                 String("0" + d.getMinutes()).slice(-2) +
                 String("0" + d.getSeconds()).slice(-2) +
                 String("00" + d.getMilliseconds()).slice(-3);
        return ts;
    }

    var FileLoggingSessionScenario = WinJS.Class.define(
        function () {
            this._channel = new Windows.Foundation.Diagnostics.LoggingChannel("FileLoggingSession_Channel");
            this._channel.addEventListener("loggingenabled", this._onLoggingEnabled);

            Windows.UI.WebUI.WebUIApplication.addEventListener("suspending", this._onAppSuspension);
            Windows.UI.WebUI.WebUIApplication.addEventListener("resuming", this._onAppResume);
        },
        {
            doScenarioAsync: function () {
                this._setBusyStatus(true);
                try {

                    var data = { scenario: this, messageIndex: 0, startFileCount: this._logFileGeneratedCount };

                    function logMessagesAndPauseAsync(data) {

                        var totalFilesCreatedSoFar = data.scenario._logFileGeneratedCount - data.startFileCount;
                        if (totalFilesCreatedSoFar >= 3) {
                            // At least 3 files created, stop.
                            return new WinJS.Promise(function (completionFunction, errorFunction, progressFunction) {
                                data.scenario._setBusyStatus(false);
                                completionFunction();
                            });
                        } else {

                            var channel = data.scenario._channel;

                            for (var index = 0; index < 50; index++) {

                                // Since the channel is added to the session at level Warning,
                                // the following is logged because it is logged at level LoggingLevel.Critical.
                                var messageToLog =
                                    "Message=" +
                                    (++data.messageIndex) +
                                    ": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum " +
                                    "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
                                channel.logMessage(messageToLog, Windows.Foundation.Diagnostics.LoggingLevel.critical);

                                // Since the channel is added to the session at level Warning,
                                // the following is *not* logged because it is logged at LoggingLevel.Information.
                                messageToLog =
                                    "Message=" +
                                    (++data.messageIndex) +
                                    ": Lorem ipsum dolor sit amet, consectetur adipiscing elit.In ligula nisi, vehicula nec eleifend vel, rutrum non dolor.Vestibulum ante ipsum " +
                                    "primis in faucibus orci luctus et ultrices posuere cubilia Curae; Curabitur elementum scelerisque accumsan. In hac habitasse platea dictumst.";
                                channel.logMessage(messageToLog, Windows.Foundation.Diagnostics.LoggingLevel.information);

                                var value = 1000000; // 1 million, 7 digits, 4-bytes as an integer, 14 bytes as a wide character string.
                                channel.logMessage("Value #" + (++data.messageIndex).toString() + "  " + value, Windows.Foundation.Diagnostics.LoggingLevel.critical); // 'value' is logged as 14 byte wide character string.
                                channel.logValuePair("Value #" + (++data.messageIndex).toString(), value, Windows.Foundation.Diagnostics.LoggingLevel.critical); // 'value' is logged as a 4-byte integer.
                            }

                            // Pause every once in a while to simulate application
                            // activity outside of logging.
                            return WinJS.Promise.timeout(25).then(function () {

                                return logMessagesAndPauseAsync(data);
                            });
                        }
                    }

                    // Log messages until at least 3 log files are created.
                    return logMessagesAndPauseAsync(data);

                } catch (e) {
                    this._setBusyStatus(false);
                    throw e;
                }
            },
            toggleLoggingEnabledDisabledAsync: function () {
                this._setBusyStatus(true);
                try {
                    if (this._session !== null) {
                        var data = { scenario: this };
                        return this._closeSessionSaveFinalLogFile().then(function (finalLogFilePath) {
                            data.scenario._session.close();
                            data.scenario._session = null;
                            data.scenario._dispatchStatusChanged({ type: "LogFileGeneratedAtDisable", logFilePath: finalLogFilePath });
                            Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LoggingEnabled"] = false;
                            data.scenario._dispatchStatusChanged({ type: "LoggingEnabledDisabled", enabled: false });
                            data.scenario._setBusyStatus(false);
                            return false;
                        });
                    } else {
                        this._startLogging();
                        Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LoggingEnabled"] = true;
                        this._dispatchStatusChanged({ type: "LoggingEnabledDisabled", enabled: true });
                        this._setBusyStatus(false);
                        return new WinJS.Promise(function (completionFunction, errorFunction, progressFunction) {
                            completionFunction(true);
                        });
                    }
                } catch (e) {
                    this._setBusyStatus(false);
                    throw e;
                }
            },
            isLoggingEnabled: {
                get: function () {
                    return this._session !== null;
                }
            },
            _session: null,
            _channel: null,
            _isChannelEnabled: false,
            _channelLoggingLevel: Windows.Foundation.Diagnostics.LoggingLevel.verbose,
            _logFileGeneratedCount: 0,
            _isPreparingForSuspending: false,
            _isBusy: false,
            _setBusyStatus: function (busy) {
                if (this._isBusy === busy) {
                    return;
                }
                this._isBusy = busy;
                if (busy) {
                    this._dispatchStatusChanged({ type: "BusyStatusChanged" });
                }
            },
            getBusyStatus: function () {
                return this._isBusy;
            },            
            _onAppSuspension: function (suspendingEventArgs) {

                var scenario = FileLoggingSessionScenario.instance;

                // Get a deferral before performing any async operations
                // to avoid suspension prior to FileLoggingSessionScenario completing 
                // PrepareToSuspendAsync().
                var deferral = suspendingEventArgs.suspendingOperation.getDeferral();
                scenario._prepareToSuspendAsync().then(function () {
                    // From FileLoggingSessionScenario's perspective, it's now okay to 
                    // suspend, so release the deferral. 
                    deferral.complete();
                });
            },
            _onAppResume: function (resumingEventArgs) {
                var scenario = FileLoggingSessionScenario.instance;
                scenario.resumeLoggingIfApplicable();
            },
            _startLogging: function () {

                if (this._session === null) {
                    this._session = new Windows.Foundation.Diagnostics.FileLoggingSession("FileLoggingSession_Session");
                    this._session.addEventListener("logfilegenerated", this._onLogFileGenerated);
                }

                // This sample adds the channel at level "warning" to 
                // demonstrated how messages logged at more verbose levels
                // are ignored by the session. 
                this._session.addLoggingChannel(this._channel, Windows.Foundation.Diagnostics.LoggingLevel.warning);
            },
            _onLoggingEnabled: function (args) {

                // This method is called when the channel is informing us of channel-related state changes.
                // Save new channel state. These values can be used for advanced logging scenarios where, 
                // for example, it's desired to skip blocks of logging code if the channel is not being
                // consumed by any sessions. 

                var scenario = FileLoggingSessionScenario.instance;
                scenario._isChannelEnabled = args.target.enabled;
                scenario._channelLoggingLevel = args.target.level;
            },
            _onLogFileGenerated: function (args) {

                // When the log file gets large, the system closes it and starts a new one.
                // The LogFileGenerated event is raised to give the app a chance to move
                // the recently-generated log file to a safe place. When the handler returns,
                // the FileLoggingSession may reuse the file name for a new log file.
                // This function moves the log file to an app-defined location.

                var data = { scenario: FileLoggingSessionScenario.instance, finalLogFileOfSession: args.file, ourAppLogFolder: null, newLogFileName: null };
                data.scenario._logFileGeneratedCount++;

                Windows.Storage.ApplicationData.current.localFolder.createFolderAsync(
                    "FileLoggingSession_LogFiles",
                    Windows.Storage.CreationCollisionOption.openIfExists)
                .then(function (ourAppLogFolder) {
                    data.ourAppLogFolder = ourAppLogFolder;
                    data.newLogFileName = "Log-" + getTimeStamp() + ".etl";
                    return data.finalLogFileOfSession.moveAsync(data.ourAppLogFolder, data.newLogFileName, Windows.Storage.NameCollisionOption.failIfExists);
                })
                .then(function () {
                    var fullPath = data.ourAppLogFolder.path + "\\" + data.newLogFileName;
                    if (!data.scenario._isPreparingForSuspending) {
                        data.scenario._dispatchStatusChanged({ type: "LogFileGenerated", logFilePath: fullPath });
                    }
                });
            },
            resumeLoggingIfApplicable: function () {

                // If logging is already enabled, nothing to do.
                if (this.isLoggingEnabled) {
                    return true;
                }

                var loggingEnabled;
                if (Windows.Storage.ApplicationData.current.localSettings.values.hasKey("FileLoggingSession_LoggingEnabled")) {
                    loggingEnabled = Windows.Storage.ApplicationData.current.localSettings.values.hasKey("FileLoggingSession_LoggingEnabled");
                } else {
                    Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LoggingEnabled"] = true;
                    loggingEnabled = true;
                }

                if (loggingEnabled) {
                    this._startLogging();
                }
                
                if (Windows.Storage.ApplicationData.current.localSettings.values.hasKey("FileLoggingSession_LogFileGeneratedBeforeSuspend")) {

                    var logFileGeneratedBeforeSuspend = Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LogFileGeneratedBeforeSuspend"];
                    if (logFileGeneratedBeforeSuspend !== null) {
                        this._dispatchStatusChanged( { type: "LogFileGeneratedAtSuspend", logFilePath: logFileGeneratedBeforeSuspend });
                    }
                    Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LogFileGeneratedBeforeSuspend"] = null;
                }
            },
            _closeSessionSaveFinalLogFile: function () {
                /// <summary>Call the CloseAndSaveToFileAsync() method of a FileLoggingSession, and move the log file to this app's log file repository.</summary>
                /// <returns type="WinJS.Promise">A Promise to return the final path of the log file, or null if there was no final log file.</returns>
                
                var data = { scenario: this, finalLogFileOfSession: null, ourAppLogFolder: null, newLogFileName: null };

                return this._session.closeAndSaveToFileAsync().then(function (finalLogFileOfSession) {

                    // FileLoggingSession.closeAndSaveToFileAsync can return null if nothing
                    // has been logged to the session since the last log file was saved.
                    if (finalLogFileOfSession !== null) {

                        data.finalLogFileOfSession = finalLogFileOfSession;
                        return Windows.Storage.ApplicationData.current.localFolder.createFolderAsync(
                                   "FileLoggingSession_LogFiles",
                                   Windows.Storage.CreationCollisionOption.openIfExists)
                        .then(function (ourAppLogFolder) {
                            data.ourAppLogFolder = ourAppLogFolder;
                            data.newLogFileName = "Log-" + getTimeStamp() + ".etl";
                            return data.finalLogFileOfSession.moveAsync(data.ourAppLogFolder, data.newLogFileName, Windows.Storage.NameCollisionOption.failIfExists);
                        })
                        .then(function () {
                            return data.ourAppLogFolder.path + "\\" + data.newLogFileName;
                        });
                    }

                    // There was no log file, so simply return the 'null' path to indicate there was no log file.
                    return new WinJS.Promise(function (completionFunction, errorFunction, progressFunction) {
                        completionFunction(null);
                    });
                });
            },
            _prepareToSuspendAsync: function () {
                /// <summary>Save any final log file and save app state before suspending.</summary>
                /// <returns type="WinJS.Promise">A Promise.</returns>
                if (this._session !== null) {
                    this._isPreparingForSuspending = true;
                    var data = { scenario: this };
                    return this._closeSessionSaveFinalLogFile().then(function (finalLogFilePath) {
                        data.scenario._session = null;
                        Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LoggingEnabled"] = true;
                        Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LogFileGeneratedBeforeSuspend"] = finalLogFilePath;
                    }).then(function () {
                        data.scenario._isPreparingForSuspending = false;
                    });
                } else {
                    Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LoggingEnabled"] = false;
                    Windows.Storage.ApplicationData.current.localSettings.values["FileLoggingSession_LogFileGeneratedBeforeSuspend"] = null;
                    return new WinJS.Promise(function (completionFunction, errorFunction, progressFunction) {
                        completionFunction(); 
                    });
                }
            },
            _dispatchStatusChanged: function (statusArgs) {
                this.dispatchEvent("statusChanged", statusArgs);
            }
        },
        {
            // FileLoggingSessionScenario static members:

            _loggingScenarioSingleton: null,
            instance: {
                get: function () {
                    if (!this._loggingScenarioSingleton) {
                        this._loggingScenarioSingleton = new FileLoggingSessionScenario();
                    }
                    return this._loggingScenarioSingleton;
                }
            }
        });
    
    WinJS.Class.mix(FileLoggingSessionScenario, WinJS.Utilities.eventMixin);

    // Export public methods & controls
    WinJS.Namespace.define("FileLoggingSessionScenario", {
        FileLoggingSessionScenario: FileLoggingSessionScenario
    });

})();
