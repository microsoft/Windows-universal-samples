//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var allJoyn = Windows.Devices.AllJoyn;
    var onboarding = org.alljoyn.Onboarding;
    var wifi = Windows.Devices.WiFi;

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            scenarioInit();
        },
        unload: function (element, options) {
            scenarioCleanup();
        }
    });

    var onboardeeScanResult, onboarderScanResult;
    var isCredentialRequested, isAuthenticated;
    var showOnboardeeSsidList, showOnboarderSsidList;
    var wifiAdapter, wifiAdapterList;
    var savedProfileName;
    var busAttachment = null;
    var watcher = null;
    var consumer = null;

    function scenarioInit() {
        resetScenario();
        Radio1.onclick = onboardeeChoicesClickHandler;
        Radio2.onclick = onboardeeChoicesClickHandler;
        scanButton.onclick = scanButtonClickHandler;
        adapterSelect.onchange = adapterSelectChangeHandler;
        softAPNetworkSelect.onchange = softAPNetworkSelectChangeHandler;
        connectButton.onclick = connectButtonClickHandler;
        onboardeeNetworkSelect.onchange = onboardeeNetworkSelectChangeHandler;
        manualSsidCheckbox.onclick = manualSsidCheckboxClickHandler;
        onboardButton.onclick = onboardButtonClickHandler;

        for (var index in OnboardingEnumTypes.AuthenticationType.properties) {
            var option = document.createElement("option");
            option.text = OnboardingEnumTypes.AuthenticationType.properties[index].name;
            authenticationSelect.add(option);
        }
    }

    function scenarioCleanup() {
        disposeConsumer();
        disposeWatcher();

        if (busAttachment != null) {
            busAttachment.oncredentialsrequested = null;
            busAttachment.onauthenticationcomplete = null;
            busAttachment.onstatechanged = null;
            busAttachment.disconnect();
            busAttachment = null;
        }
    }

    function onboardeeChoicesClickHandler() {
        resetScenario();
        scenarioCleanup();
        scanOptions.style.display = "block";
    }

    function scanButtonClickHandler() {
        if (Radio1.checked) {
            scanForOnboardingInterfaces();
        } else if (Radio2.checked) {
            scanForWiFiAdapters();
        }
    }

    function scanForOnboardingInterfaces() {
        scenarioCleanup();

        busAttachment = new allJoyn.AllJoynBusAttachment();
        busAttachment.onstatechanged = stateChangedHandler;
        busAttachment.authenticationMechanisms.clear();

        // EcdhePsk authentication is deprecated as of the AllJoyn 16.04 release.
        // Newly added EcdheSpeke should be used instead. EcdhePsk authentication is
        // added here to maintain compatibility with devices running older AllJoyn versions.
        busAttachment.authenticationMechanisms.append(allJoyn.AllJoynAuthenticationMechanism.ecdhePsk);
        busAttachment.authenticationMechanisms.append(allJoyn.AllJoynAuthenticationMechanism.ecdheSpeke);
        busAttachment.onauthenticationcomplete = authenticationCompleteHandler;
        busAttachment.oncredentialsrequested = credentialsRequestedHandler;
        watcher = new allJoyn.AllJoynBusAttachment.getWatcher(["org.alljoyn.Onboarding"]);
        watcher.onadded = watcherAddedHandler;
        reportStatus("Searching for onboarding interface...");
        watcher.start();
    }

    function stateChangedHandler(args) {
        if (args.detail[0].state === allJoyn.AllJoynBusAttachmentState.disconnected) {
            reportStatus("AllJoyn bus attachment has disconnected with AllJoyn error: 0x" + args.detail[0].status.toString(16));
            resetScenario();
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
        authenticateOptions.style.display = "none";
        authenticateButton.onclick = null;
    }

    function credentialsRequestedHandler(args) {
        var credentialsDeferral = args.detail[0].getDeferral();
        isCredentialRequested = true;

        if ((args.detail[0].credentials.authenticationMechanism == allJoyn.AllJoynAuthenticationMechanism.ecdhePsk) ||
            (args.detail[0].credentials.authenticationMechanism == allJoyn.AllJoynAuthenticationMechanism.ecdheSpeke)) {
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
            authenticateOptions.style.display = "block";
        } else {
            reportError("Unexpected authentication mechanism.");
            credentialsDeferral.complete();
        }
    }

    function watcherAddedHandler(args) {
        reportStatus("Joining session...");
        onboarding.OnboardingConsumer.fromIdAsync(args.id, busAttachment)
            .then(function (onboardingConsumer) {
                if (onboardingConsumer != null) {
                    disposeConsumer();
                    consumer = onboardingConsumer;
                    consumer.session.onlost = sessionLostHandler;

                    if (!isCredentialRequested || isAuthenticated) {
                        getOnboardeeNetworkList();
                    }
                } else {
                    reportError("Attempt to join session failed.");
                }
            });
    }

    function sessionLostHandler(args) {
        reportStatus("AllJoyn session with the producer lost due to AllJoynSessionLostReason = " + args.detail[0].reason.toString());
        disposeConsumer();
        resetScenario();
    }

    function getOnboardeeNetworkList() {
        reportStatus("Requesting network list from the onboardee...");
        consumer.getScanInfoAsync()
            .then(function (getScanResult) {
                if (getScanResult.status === allJoyn.AllJoynStatus.ok) {
                    if (getScanResult.scanList.size > 0) {
                        onboardeeScanResult = getScanResult.scanList;
                        for (var i = 0; i < getScanResult.scanList.size; i++) {
                            var option = document.createElement("option");
                            option.text = getScanResult.scanList[i].value1;
                            onboardeeNetworkSelect.add(option);
                        }
                        displayOnboardeeSsidList();
                        reportStatus(getScanResult.scanList.size.toString() + " network(s) returned. Scan age: " + getScanResult.age.toString() + " minutes.");
                    } else {
                        // No networks returned by the onboardee. Provide the user option to configure onboardee with networks visible from onboarder.
                        reportStatus("Please select a network or enter one manually.");
                        displayOnboarderSsidList();
                    }
                } else {
                    // Unable to get scan information from the onboardee. Provide the user option to configure onboardee with networks visible from onboarder.
                    reportStatus("Please select a network or enter one manually.");
                    displayOnboarderSsidList();
                }
                configurationOptions.style.display = "block";
            });
    }

    function scanForWiFiAdapters() {
        if (wifiAdapter == null) {
            reportStatus("Requesting WiFi access...");
            wifi.WiFiAdapter.requestAccessAsync()
                .then(function (accessStatus) {
                    if (accessStatus != wifi.WiFiAccessStatus.allowed) {
                        reportError("WiFi access denied.");
                    } else {
                        wifi.WiFiAdapter.findAllAdaptersAsync()
                            .then(function (adapterList) {
                                wifiAdapterList = adapterList;
                                if (adapterList.size > 0) {
                                    for (var i = 0; i < adapterList.size; i++) {
                                        var option = document.createElement("option");
                                        option.text = "Adapter " + (i + 1).toString();
                                        adapterSelect.add(option);
                                    }
                                    adapterSelect.style.display = "inline";
                                    reportStatus("Please select a WiFi adapter.");
                                } else {
                                    reportError("No WiFi adapters detected on this machine.");
                                }
                            }, function (error) {
                                reportError(error.message);
                            });
                    }
                }, function (error) {
                    reportError(error.message);
                });
        }
    }

    function adapterSelectChangeHandler() {
        if (adapterSelect.selectedIndex > 0) {
            wifiAdapter = wifiAdapterList[adapterSelect.selectedIndex - 1];
        }
        if (wifiAdapter != null) {
            networkStatusChangedHandler();
            Windows.Networking.Connectivity.NetworkInformation.onnetworkstatuschanged = networkStatusChangedHandler;
            reportStatus("Scanning for available WiFi networks...");
            wifiAdapter.scanAsync()
                .then(function () {
                    onboarderScanResult = wifiAdapter.networkReport.availableNetworks;
                    if (onboarderScanResult.size > 0) {
                        for (var i = 0; i < onboarderScanResult.size; i++) {
                            var option = document.createElement("option");
                            option.text = onboarderScanResult[i].ssid;
                            softAPNetworkSelect.add(option);
                        }
                        onboarderNetworkSelect.innerHTML = softAPNetworkSelect.innerHTML;
                        connectOptions.style.display = "block";
                        reportStatus("Scan complete. Please select a network to connect.");
                    } else {
                        reportError("Scan complete. No WiFi networks found.");
                    }
                }, function (error) {
                    reportError(error.message);
                });
        }
    }

    function networkStatusChangedHandler() {
        if (wifiAdapter != null) {
            wifiAdapter.networkAdapter.getConnectedProfileAsync()
                .then(function (connectedProfile) {
                    if ((connectedProfile != null) && (connectedProfile.profileName !== savedProfileName)) {
                        savedProfileName = connectedProfile.profileName;
                    } else if ((connectedProfile == null) && (savedProfileName != null)) {
                        reportStatus("WiFi adapter disconnected from: " + savedProfileName);
                        savedProfileName = null;
                    }
                }, function (error) {
                    reportError(error.message);
                });
        }
    }

    function softAPNetworkSelectChangeHandler() {
        if (softAPNetworkSelect.selectedIndex > 0) {
            if (onboarderScanResult[softAPNetworkSelect.selectedIndex - 1].securitySettings.networkAuthenticationType ===
                Windows.Networking.Connectivity.NetworkAuthenticationType.open80211) {
                softAPPasswordInput.style.display = "none";
            } else {
                softAPPasswordInput.style.display = "inline";
            }
        } else {
            softAPPasswordInput.style.display = "none";
        }
        clearPasswords();
    }

    function connectButtonClickHandler() {
        if (wifiAdapter != null) {
            if (softAPNetworkSelect.selectedIndex > 0) {
                reportStatus("Attempting to connect to " + softAPNetworkSelect[softAPNetworkSelect.selectedIndex].text + "...");
                var result = null;
                if (onboarderScanResult[softAPNetworkSelect.selectedIndex - 1].securitySettings.networkAuthenticationType ===
                    Windows.Networking.Connectivity.NetworkAuthenticationType.open80211) {
                    wifiAdapter.connectAsync(onboarderScanResult[softAPNetworkSelect.selectedIndex - 1], wifi.WiFiReconnectionKind.manual)
                        .then(function (connectionResult) {
                            validateConnectionResult(connectionResult);
                        }, function (error) {
                            reportError(error.message);
                        });
                } else {
                    if (isNullOrWhitespace(softAPPasswordInput.value)) {
                        reportError("No password entered.");
                    } else {
                        var credential = new Windows.Security.Credentials.PasswordCredential();
                        credential.password = softAPPasswordInput.value;
                        wifiAdapter.connectAsync(onboarderScanResult[softAPNetworkSelect.selectedIndex - 1], wifi.WiFiReconnectionKind.manual, credential)
                            .then(function (connectionResult) {
                                validateConnectionResult(connectionResult);
                            }, function (error) {
                                reportError(error.message);
                            });
                    }
                    clearPasswords();
                }
            } else {
                reportError("Network not selected. Please select a network.");
            }
        }
    }

    function validateConnectionResult(result) {
        if (result != null) {
            if (result.connectionStatus === wifi.WiFiConnectionStatus.success) {
                reportStatus("Successfully connected to " + onboarderScanResult[softAPNetworkSelect.selectedIndex - 1].ssid);
                scanForOnboardingInterfaces();
            } else {
                reportError("Failed to connect to " + onboarderScanResult[softAPNetworkSelect.selectedIndex - 1].ssid + " with error: " +
                    result.connectionStatus.toString());
            }
        }
    }

    function onboardeeNetworkSelectChangeHandler() {
        if (onboardeeNetworkSelect.selectedIndex > 0) {
            authenticationSelect.selectedIndex = onboardeeScanResult[onboardeeNetworkSelect.selectedIndex - 1].value2 +
                OnboardingEnumTypes.authenticationIndexOffset + 1;
        }
    }

    function manualSsidCheckboxClickHandler() {
        if (manualSsidCheckbox.checked) {
            onboardeeNetworkSelect.style.display = "none";
            onboarderNetworkSelect.style.display = "none";
            ssidInput.style.display = "inline";
        } else {
            if (showOnboardeeSsidList) {
                onboardeeNetworkSelect.style.display = "inline";
            } else {
                onboarderNetworkSelect.style.display = "inline";
            }
            ssidInput.style.display = "none";
        }
    }

    function onboardButtonClickHandler() {
        var configureSsid = null;
        if (manualSsidCheckbox.checked) {
            configureSsid = ssidInput.value;
        } else {
            if (showOnboardeeSsidList) {
                if (onboardeeNetworkSelect.selectedIndex > 0) {
                    configureSsid = onboardeeScanResult[onboardeeNetworkSelect.selectedIndex - 1].value1;
                }
            } else {
                if (onboarderNetworkSelect.selectedIndex > 0) {
                    configureSsid = onboarderScanResult[onboarderNetworkSelect.selectedIndex - 1].ssid;
                }
            }
        }

        if (isNullOrWhitespace(configureSsid)) {
            reportError("No SSID selected. Please select a network or manually enter SSID.");
        } else {
            if (authenticationSelect.selectedIndex > 0) {
                attemptOnboarding(configureSsid, configurationPasswordInput.value,
                    OnboardingEnumTypes.AuthenticationType[authenticationSelect[authenticationSelect.selectedIndex].text]);
            } else {
                reportError("No authentication type selected.");
            }
        }
    }

    function attemptOnboarding(ssid, password, authenticationType) {
        reportStatus("Attempting to configure onboardee...");

        // WiFi password must be converted to hex representation of the UTF-8 string.
        consumer.configureWiFiAsync(ssid, convertUtf8ToHex(password), authenticationType)
            .then(function (configureWifiResult) {
                if (configureWifiResult.status === allJoyn.AllJoynStatus.ok) {
                    reportStatus("Onboardee sucessfully configured.");
                    if (configureWifiResult.status2 === OnboardingEnumTypes.ConfigureWiFiResultStatus.Concurrent) {
                        // The Onboardee has indicated that it will attempt to connect to the desired AP concurrently, while the SoftAP is enabled. In this case, 
                        // the Onboarder application must wait for the ConnectionResult signal to arrive over the AllJoyn session established over the SoftAP link.
                        // If the Onboardee does not connect to the desired AP concurrently, then there is no guaranteed way for the Onboarder application to find
                        // out if the connection attempt was successful or not. In the NotConcurrent connection attempt, if the Onboardee fails to connect to
                        // the desired AP, the Onboarder application will have to again start over with scanning and connecting to the Onboardee SoftAP.
                        // For more information please visit http://go.microsoft.com/fwlink/?LinkId=817239
                        consumer.signals.onconnectionresultreceived = connectionResultReceivedHandler;
                    }
                    attemptConnection();
                } else {
                    reportError("Attempt to configure WiFi failed with AllJoyn error: 0x" + configureWifiResult.status.toString(16));
                }
            });
        clearPasswords();
    }

    function connectionResultReceivedHandler(args) {
        if (args.detail[0].arg.value1 === OnboardingEnumTypes.ConnectionResultCode.Validated) {
            reportStatus("Onboarding successful.");
        } else {
            reportError("Connection attempt failed with result code: " + OnboardingEnumTypes.ConnectionResultCode.properties[args.detail[0].arg.value1] +
                " and message: " + args.detail[0].arg.value2 + ".");
        }
    }

    function attemptConnection() {
        consumer.connectAsync()
            .then(function () {
                reportStatus("Connect request sent.");
            });
    }

    function displayOnboardeeSsidList() {
        if (manualSsidCheckbox.checked) {
            showOnboardeeSsidList = true;
            showOnboarderSsidList = false;
        } else {
            onboardeeNetworkSelect.style.display = "inline";
            onboarderNetworkSelect.style.display = "none";
        }
    }

    function displayOnboarderSsidList() {
        if (manualSsidCheckbox.checked) {
            showOnboardeeSsidList = false;
            showOnboarderSsidList = true;
        } else {
            onboardeeNetworkSelect.style.display = "none";
            onboarderNetworkSelect.style.display = "inline";
        }
    }

    function clearPasswords() {
        softAPPasswordInput.value = "";
        configurationPasswordInput.value = "";
    }

    function convertUtf8ToHex(str) {
        var tempBuffer = Windows.Security.Cryptography.CryptographicBuffer.convertStringToBinary(str, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);
        return Windows.Security.Cryptography.CryptographicBuffer.encodeToHexString(tempBuffer);
    }

    function disposeConsumer() {
        if (consumer != null) {
            consumer.session.onlost = null;
            consumer.close();
            consumer = null;
        }
    }

    function disposeWatcher() {
        if (watcher != null) {
            watcher.onadded = null;
            watcher.stop();
            watcher.close();
            watcher = null;
        }
    }

    function resetScenario() {
        scanOptions.style.display = "none";
        adapterSelect.style.display = "none";
        connectOptions.style.display = "none";
        softAPPasswordInput.style.display = "none";
        authenticateOptions.style.display = "none";
        configurationOptions.style.display = "none";
        ssidInput.style.display = "none";
        manualSsidCheckbox.checked = false;
        isCredentialRequested = false;
        isAuthenticated = false;
        showOnboardeeSsidList = true;
        showOnboarderSsidList = false;
        wifiAdapter = null
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
