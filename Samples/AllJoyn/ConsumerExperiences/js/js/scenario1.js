//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var allJoyn = Windows.Devices.AllJoyn;
    var secureInterface = com.microsoft.Samples.SecureInterface;
    var notifications = Windows.UI.Notifications;

    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            scenarioInit();
        },
        unload: function (element, options) {
            scenarioCleanup();
        }
    });

    var busAttachment = null;
    var watcher = null;
    var consumer = null;
    var isAuthenticated = false;
    var isCredentialsRequested = false;
    var callSetProperty = true;

    function scenarioInit() {
        authenticationOptions.style.display = "none";
        consumerOptions.style.display = "none";
        connectButton.onclick = connectButtonClickHandler;
        concatenateButton.onclick = concatenateButtonClickHandler;
        upperCaseSwitch.onchange = upperCaseSwitchChangeHandler;
    }

    function scenarioCleanup() {
        disposeConsumer();
        disposeWatcher();

        if (busAttachment != null) {
            busAttachment.onstatechanged = null;
            busAttachment.onauthenticationcomplete = null;
            busAttachment.oncredentialsrequested = null;
            busAttachment.disconnect();
            busAttachment = null;
        }
    }

    function connectButtonClickHandler() {
        scenarioCleanup();

        busAttachment = new allJoyn.AllJoynBusAttachment();
        busAttachment.onstatechanged = stateChangedHandler;
        busAttachment.authenticationMechanisms.clear();
        busAttachment.authenticationMechanisms.append(allJoyn.AllJoynAuthenticationMechanism.ecdheSpeke);
        busAttachment.onauthenticationcomplete = authenticationCompleteHandler;
        busAttachment.oncredentialsrequested = credentialsRequestedHandler;

        // Initialize a watcher object to listen for about interfaces.
        watcher = new allJoyn.AllJoynBusAttachment.getWatcher(["com.microsoft.Samples.SecureInterface"]);

        // Subscribing to the added event that will be raised whenever a producer for this service is found.
        watcher.onadded = watcherAddedHandler;

        reportStatus("Searching...");

        // Start watching for producers advertising this service.
        watcher.start();
    }

    function stateChangedHandler(args) {
        if (args.detail[0].state === allJoyn.AllJoynBusAttachmentState.disconnected) {
            reportStatus("AllJoyn bus attachment has disconnected with AllJoyn error: 0x" + args.detail[0].status.toString(16));
            consumerOptions.style.display = "block";
        }
    }

    function authenticationCompleteHandler(args) {
        if (args.detail[0].succeeded) {
            reportStatus("Authentication was successful.");
        } else {
            reportError("Authentication failed.");
        }

        isAuthenticated = args.detail[0].succeeded;
        keyInputText.value = "";
        authenticationOptions.style.display = "none";
        authenticateButton.onclick = null;
    }

    function credentialsRequestedHandler(args) {
        var credentialsDeferral = args.detail[0].getDeferral();
        isCredentialsRequested = true;

        if (args.detail[0].credentials.authenticationMechanism == allJoyn.AllJoynAuthenticationMechanism.ecdheSpeke) {
            // Wait for the user to provide key and click authenticate.
            reportStatus("Please enter the key.");

            authenticateButton.onclick = function () {
                if (isNullOrWhitespace(keyInputText.value)) {
                    reportError("Please enter a key.");
                } else {
                    reportStatus("Authenticating...");

                    args.detail[0].credentials.passwordCredential.password = keyInputText.value;
                    credentialsDeferral.complete();
                }
            };
            authenticationOptions.style.display = "block";
        } else {
            reportError("Unexpected authentication mechanism.");
            credentialsDeferral.complete();
        }
    }

    function watcherAddedHandler(args) {
        // Optional - Get the About data of the producer.
        allJoyn.AllJoynServiceInfo.fromIdAsync(args.id)
            .then(function (alljoynServiceInfo) {
                busAttachment.getAboutDataAsync(alljoynServiceInfo)
                    .then(function (aboutData) {
                        if (aboutData != null) {
                            // Check to see if device name is populated in the about data, since device name is not a mandatory field.
                            if (aboutData.deviceName != null) {
                                reportStatus("Found " + aboutData.appName + " on " + aboutData.deviceName + " from manufacturer: " + aboutData.manufacturer + ". Connecting...");
                            } else {
                                reportStatus("Found " + aboutData.appName + " from manufacturer: " + aboutData.manufacturer + ". Connecting...");
                            }
                        } else {
                            reportError("Unable to get About data.");
                        }
                    })
                    .then(function () {
                        reportStatus("Joining session...");
                        // Attempt to join the session when a producer is discovered.
                        secureInterface.SecureInterfaceConsumer.fromIdAsync(args.id, busAttachment)
                            .then(function (secureInterfaceConsumer) {
                                if (secureInterfaceConsumer != null) {
                                    disposeConsumer();
                                    consumer = secureInterfaceConsumer;
                                    consumer.onisuppercaseenabledchanged = isUpperCaseEnabledChangedHandler;
                                    consumer.signals.ontextsentreceived = textSentReceivedHandler;
                                    consumer.session.onlost = sessionLostHandler;

                                    // At the time of connection, the request credentials callback not being invoked is an
                                    // indication that the consumer and producer are already authenticated from a previous session.
                                    if (!isCredentialsRequested) {
                                        reportStatus("Connected and already authenticated from previous session.");
                                        isUpperCaseEnabledChangedHandler();
                                    } else {
                                        if (isAuthenticated) {
                                            reportStatus("Connected with authentication.");
                                            isUpperCaseEnabledChangedHandler();
                                        } else {
                                            reportError("Connected but authentication failed.");
                                        }
                                    }
                                    consumerOptions.style.display = "block";
                                } else {
                                    reportError("Attempt to join session failed.");
                                }
                            });
                    });
            });
    }

    function isUpperCaseEnabledChangedHandler() {
        consumer.getIsUpperCaseEnabledAsync()
            .then(function (getIsUpperCaseEnabledResult) {
                if (getIsUpperCaseEnabledResult.status === allJoyn.AllJoynStatus.ok) {
                    if (upperCaseSwitch.winControl.checked !== getIsUpperCaseEnabledResult.isUpperCaseEnabled) {
                        callSetProperty = false;
                        upperCaseSwitch.winControl.checked = getIsUpperCaseEnabledResult.isUpperCaseEnabled;
                    }
                } else {
                    reportError("Get property failed with AllJoyn error: 0x" + getIsUpperCaseEnabledResult.status.toString(16));
                }
            });
    }

    function textSentReceivedHandler(args) {
        // Show UI Toast.
        var toastTemplate = notifications.ToastTemplateType.toastText02;
        var toastXml = notifications.ToastNotificationManager.getTemplateContent(toastTemplate);

        // Populate UI Toast.
        var toastTextElements = toastXml.getElementsByTagName("text");
        toastTextElements[0].appendChild(toastXml.createTextNode("Signal Received - " + args.detail[0].message));

        // Create and Send UI Toast.
        var toast = new notifications.ToastNotification(toastXml);
        notifications.ToastNotificationManager.createToastNotifier().show(toast);

        reportStatus("Signal Received - " + args.detail[0].message);
    }

    function sessionLostHandler(args) {
        reportStatus("AllJoyn session with the producer lost due to AllJoynSessionLostReason = " + args.detail[0].reason.toString());
        consumerOptions.style.display = "none";
        disposeConsumer();
    }

    function concatenateButtonClickHandler() {
        if ((isNullOrWhitespace(inputText1.value)) || (isNullOrWhitespace(inputText2.value))) {
            reportError("Input strings cannot be empty.");
        } else {
            // Call the Concatenate method with the input strings arguments.
            consumer.concatenateAsync(inputText1.value, inputText2.value)
                .then(function (catResult) {
                    if (catResult.status === allJoyn.AllJoynStatus.ok) {
                        reportStatus("Concatenation output : " + catResult.outStr);
                    } else {
                        reportError("AllJoyn Error : 0x" + catResult.status.toString(16));
                    }
                });
        }
    }

    function upperCaseSwitchChangeHandler(args) {
        if (callSetProperty) {
            consumer.setIsUpperCaseEnabledAsync(args.target.winControl.checked)
                .then(function (setIsUpperCaseEnabledResult) {
                    if (setIsUpperCaseEnabledResult.status === allJoyn.AllJoynStatus.ok) {
                        reportStatus("\"IsUpperCaseEnabled\" property successfully set.");
                    } else {
                        reportError("Set property failed with AllJoyn error: 0x" + setIsUpperCaseEnabledResult.status.toString(16));
                        callSetProperty = false;
                        upperCaseSwitch.winControl.checked = !args.target.winControl.checked;
                    }
                });
        } else {
            callSetProperty = true;
        }
    }

    function disposeConsumer() {
        if (consumer != null) {
            consumer.onisuppercaseenabledchanged = null;
            consumer.signals.ontextsentreceived = null;
            consumer.session.onlost = null;
            consumer.close();
            consumer = null;
        }
    }

    function disposeWatcher() {
        if (watcher != null) {
            watcher.onadded = null;
            watcher.stop();
            watcher = null;
        }
    }

    function reportStatus(message) {
        WinJS.log && WinJS.log(message, "sample", "status");
    }

    function reportError(message) {
        WinJS.log && WinJS.log(message, "sample", "error");
    }

    function isNullOrWhitespace(str) {
        if ((typeof str === 'undefined') || (str == null) || (!/\S/.test(str))) {
            return true;
        } else {
            return false;
        }
    }
})();
