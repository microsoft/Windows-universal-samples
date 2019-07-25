//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var DevEnum = Windows.Devices.Enumeration;
    var resultCollection = new WinJS.Binding.List([]);
    var resultsListView;
    var deviceWatcherHelper = new SdkSample.DeviceWatcherHelper(resultCollection);
    var providePinTaskSrc;
    var confirmPinTaskSrc;
    var providePasswordCredentialTaskSrc;

    var page = WinJS.UI.Pages.define("/html/scenario9_custompairdevice.html", {
        ready: function (element, options) {

            // Hook up button event handlers
            document.getElementById("startWatcherButton").addEventListener("click", startWatcher, false);
            document.getElementById("stopWatcherButton").addEventListener("click", stopWatcher, false);
            document.getElementById("pairButton").addEventListener("click", pairDevice, false);
            document.getElementById("unpairButton").addEventListener("click", unpairDevice, false);
            document.getElementById("okButton").addEventListener("click", okButton, false);
            document.getElementById("yesButton").addEventListener("click", yesButton, false);
            document.getElementById("noButton").addEventListener("click", noButton, false);
            document.getElementById("verifyButton").addEventListener("click", verifyButton, false);

            // Hook up result list selection changed event handler
            resultsListView = element.querySelector("#resultsListView").winControl;
            resultsListView.addEventListener("selectionchanged", onSelectionChanged);
            // Hook up result list data binding
            resultsListView.itemDataSource = resultCollection.dataSource;

            // We need to do some extra work when an item changes.
            // This event is raised by the DeviceWatcherHelper.
            resultCollection.addEventListener("deviceupdated", onDeviceUpdated);

            // Manually bind selector options
            DisplayHelpers.pairingSelectors.forEach(function each(item) {
                var option = document.createElement("option");
                option.textContent = item.displayName;
                selectorComboBox.appendChild(option);
            });
            selectorComboBox.selectedIndex = 0;

            // Process any data bindings
            WinJS.UI.processAll();
        },
        unload: function () {
            deviceWatcherHelper.reset();
            resultCollection.splice(0, resultCollection.length);
        }
    });

    function startWatcher() {
        startWatcherButton.disabled = true;
        resultCollection.splice(0, resultCollection.length);

        // Get the device selector chosen by the UI then add additional constraints for devices that 
        // can be paired or are already paired.
        var selectedItem = DisplayHelpers.pairingSelectors.getAt(selectorComboBox.selectedIndex);
        var selector = "(" + selectedItem.selector + ")" + " AND (System.Devices.Aep.CanPair:=System.StructuredQueryType.Boolean#True OR System.Devices.Aep.IsPaired:=System.StructuredQueryType.Boolean#True)";

        var deviceWatcher;
        if (selectedItem.kind == DevEnum.DeviceInformationKind.unknown) {
            // Kind will be determined by the selector
            deviceWatcher = DevEnum.DeviceInformation.createWatcher(
                selector,
                null // don't request additional properties for this sample
                );
        }
        else {
            // Kind will be determined by the selector
            deviceWatcher = DevEnum.DeviceInformation.createWatcher(
                selector,
                null, // don't request additional properties for this sample
                selectedItem.kind);
        }

        WinJS.log && WinJS.log("Starting watcher...", "sample", "status");
        deviceWatcherHelper.startWatcher(deviceWatcher);
        stopWatcherButton.disabled = false;
    }

    function stopWatcher() {
        stopWatcherButton.disabled = true;
        deviceWatcherHelper.stopWatcher();
        startWatcherButton.disabled = false;
    }

    function getSelectedItem() {
        var selectedIndices = resultsListView.selection.getIndices();
        if (selectedIndices.length > 0) {
            return resultCollection.getAt(selectedIndices[0]);
        }
    }

    function onDeviceUpdated(e) {
        var id = e.detail.id;
        // If the item being updated is currently "selected", then update the pairing buttons
        var selectedItem = getSelectedItem();
        if (selectedItem && selectedItem.id === id) {
            UpdatePairingButtons();
        }
    }

    function pairDevice() {
        // Gray out the pair button and results view while pairing is in progress.
        pairButton.disabled = true;
        WinJS.log && WinJS.log("Pairing started. Please wait...", "sample", "status");

        var deviceDispInfo = getSelectedItem();
        if (deviceDispInfo) {
            var ceremoniesSelected = GetSelectedCeremonies();
            var protectionLevel = protectionLevelComboBox.selectedIndex;

            var customPairing = deviceDispInfo.deviceInfo.pairing.custom;
            customPairing.addEventListener("pairingrequested", pairingRequestHandler);
            
            customPairing.pairAsync(ceremoniesSelected, protectionLevel).done(
                function (pairingResult) {
                    var statusType = (pairingResult.status == DevEnum.DevicePairingResultStatus.paired) ? "status" : "error";
                    var message = (pairingResult.status == DevEnum.DevicePairingResultStatus.paired) ? "Paired" : "NotPaired";
                    WinJS.log && WinJS.log("Pairing result = " + message, "sample", statusType);

                    customPairing.removeEventListener("pairingrequested", pairingRequestHandler);

                    hidePairingPanel();
                    updatePairingButtons();
                });
        }
    }

    function unpairDevice() {
        // Gray out the pair button and results view while pairing is in progress.
        unpairButton.disabled = true;
        WinJS.log && WinJS.log("Unpairing started. Please wait...", "sample", "status");

        var deviceDispInfo = getSelectedItem();
        if (deviceDispInfo) {
            unpairButton.disabled = true;
            deviceDispInfo.deviceInfo.pairing.unpairAsync().done(
                function (unpairingResult) {
                    var message = (unpairingResult.status == DevEnum.DeviceUnpairingResultStatus.unpaired) ? "Unpaired" : "Failed";
                    var statusType = (unpairingResult.status == DevEnum.DeviceUnpairingResultStatus.unpaired) ? "status" : "error";
                    WinJS.log && WinJS.log("Unpairing result = " + message, "sample", statusType);

                    updatePairingButtons();
                });
        }
    }

    function pairingRequestHandler(args) {
        if (args.pairingKind == DevEnum.DevicePairingKinds.confirmOnly) {
            // Windows itself will pop the confirmation dialog as part of "consent" if this is running on Desktop or Mobile
            // If this is an App for 'Windows IoT Core' where there is no Windows Consent UX, you may want to provide your own confirmation.
            args.accept();
        }
        else if (args.pairingKind == DevEnum.DevicePairingKinds.displayPin) {
            // We just show the PIN on this side. The ceremony is actually completed when the user enters the PIN
            // on the target device. We automatically accept here since we can't really "cancel" the operation
            // from this side.
            args.accept();

            showPairingPanel("Please enter this PIN on the device you are pairing with: " + args.pin, args.pairingKind);
        }
        else if (args.pairingKind == DevEnum.DevicePairingKinds.providePin) {
            // A PIN may be shown on the target device and the user needs to enter the matching PIN on 
            // this Windows device. Get a deferral so we can perform the async request to the user.
            var collectPinDeferral = args.getDeferral();

            getPinFromUserAsync().then(function (pin) {
                if (pin) {
                    args.accept(pin);
                }
                collectPinDeferral.complete();
            });
        }
        else if (args.pairingKind == DevEnum.DevicePairingKinds.confirmPinMatch) {
            // We show the PIN here and the user responds with whether the PIN matches what they see
            // on the target device. Response comes back and we set it on the PinComparePairingRequestedData
            // then complete the deferral.
            var displayMessageDeferral = args.getDeferral();

            getUserConfirmationAsync(args.pin).then(function (accept) {
                if (accept) {
                    args.accept();
                }
                displayMessageDeferral.complete();
            });
        }
        else if (args.pairingKind == DevEnum.DevicePairingKinds.providePasswordCredential) {
            var collectCredentialDeferral = args.getDeferral();
            getPasswordCredentialFromUserAsync().then(function (credential) {
                if (credential) {
                    args.acceptWithPasswordCredential(credential);
                }
                collectCredentialDeferral.complete();
            });
        }
    }

    function showPairingPanel(text, pairingKind) {
        document.getElementById("pairingPanel").style.display = "block";
        document.getElementById("pinEntryTextBox").style.display = "none";
        document.getElementById("okButton").style.display = "none";
        document.getElementById("yesButton").style.display = "none";
        document.getElementById("noButton").style.display = "none";
        document.getElementById("usernameEntryTextBox").style.display = "none";
        document.getElementById("passwordEntryTextBox").style.display = "none";
        document.getElementById("verifyButton").style.display = "none";
        pairingTextBlock.innerHTML = text;

        switch (pairingKind) {
            case DevEnum.DevicePairingKinds.confirmOnly:
            case DevEnum.DevicePairingKinds.displayPin:
                // Don't need any buttons
                break;
            case DevEnum.DevicePairingKinds.providePin:
                pinEntryTextBox.value = "";
                document.getElementById("pinEntryTextBox").style.display = "inline";
                document.getElementById("okButton").style.display = "inline";
                break;
            case DevEnum.DevicePairingKinds.confirmPinMatch:
                document.getElementById("yesButton").style.display = "inline";
                document.getElementById("noButton").style.display = "inline";
                break;
            case DevEnum.DevicePairingKinds.providePasswordCredential:
                document.getElementById("usernameEntryTextBox").style.display = "inline";
                document.getElementById("passwordEntryTextBox").style.display = "inline";
                document.getElementById("verifyButton").style.display = "inline";
                break;
        }
    }

    function hidePairingPanel() {
        document.getElementById("pairingPanel").style.display = "none";
        pairingTextBlock.innerHTML = "";
    }

    function getPinFromUserAsync() {
        hidePairingPanel();
        completeProvidePinTask();

        showPairingPanel(
            "Please enter the PIN shown on the device you're pairing with",
            DevEnum.DevicePairingKinds.providePin);

        return new WinJS.Promise(function (c) {
            providePinTaskSrc = c;
        });
    }

    // If pin is not provided, then any pending pairing request is abandoned.
    function completeProvidePinTask(pin) {
        if (providePinTaskSrc) {
            providePinTaskSrc(pin);
            providePinTaskSrc = null;
        }
    }

    function getPasswordCredentialFromUserAsync() {
        hidePairingPanel();
        completePasswordCredential(); // Abandon any previous pin request.

        showPairingPanel(
            "Please enter the username and password",
            DevEnum.DevicePairingKinds.providePasswordCredential);

        return new WinJS.Promise(function (c) {
            providePasswordCredentialTaskSrc = c;
        });
    }

    function  completePasswordCredential(username, password)
    {
        if (providePasswordCredentialTaskSrc) {
            if (username) {
                var passwordCredential = new Windows.Security.Credentials.PasswordCredential();
                passwordCredential.userName = username;
                passwordCredential.password = password;
                providePasswordCredentialTaskSrc(passwordCredential);
            } else {
                providePasswordCredentialTaskSrc(null);
            }
            providePasswordCredentialTaskSrc = null;
        }
    }

    function getUserConfirmationAsync(pin) {
        hidePairingPanel();
        completeConfirmPinTask(false);

        showPairingPanel(
            "Does the following PIN match the one shown on the device you are pairing?: " + pin,
            DevEnum.DevicePairingKinds.confirmPinMatch);

        return new WinJS.Promise(function (c) {
            confirmPinTaskSrc = c;
        });
    }

    // If pin is not provided, then any pending pairing request is abandoned.
    function completeConfirmPinTask(accept) {
        if (confirmPinTaskSrc) {
            confirmPinTaskSrc(accept);
            confirmPinTaskSrc = null;
        }
    }
    
    function okButton() {
        // OK button is only used for the ProvidePin scenario
        completeProvidePinTask(pinEntryTextBox.value);
        hidePairingPanel();
    }

    function verifyButton(){
        // verify button is only used for the ProvidePin scenario
        completePasswordCredential(usernameEntryTextBox.value, passwordEntryTextBox.value);
        hidePairingPanel();
    }

    function yesButton() {
        completeConfirmPinTask(true);
        hidePairingPanel();
    }

    function noButton() {
        completeConfirmPinTask(false);
        hidePairingPanel();
    }

    function GetSelectedCeremonies() {
        var ceremonySelection = DevEnum.DevicePairingKinds.none;

        WinJS.Utilities.query(".win-checkbox").forEach(function (checkbox) {
            if (checkbox.checked) {
                switch (checkbox.id) {
                    case "confirmOnlyOption":
                        ceremonySelection |= DevEnum.DevicePairingKinds.confirmOnly;
                        break;
                    case "displayPinOption":
                        ceremonySelection |= DevEnum.DevicePairingKinds.displayPin;
                        break;
                    case "providePinOption":
                        ceremonySelection |= DevEnum.DevicePairingKinds.providePin;
                        break;
                    case "confirmPinMatchOption":
                        ceremonySelection |= DevEnum.DevicePairingKinds.confirmPinMatch;
                        break;
                    case "passwordCredentialOption":
                        ceremonySelection |= DevEnum.DevicePairingKinds.providePasswordCredential;
                        break;
                }
            }
        });

        return ceremonySelection;
    }

    function onSelectionChanged() {
        updatePairingButtons();
    }

    function updatePairingButtons() {

        var deviceDispInfo = getSelectedItem();

        if (deviceDispInfo &&
            deviceDispInfo.canPair &&
            !deviceDispInfo.isPaired) {
            pairButton.disabled = false;
        }
        else {
            pairButton.disabled = true;
        }

        if (deviceDispInfo &&
            deviceDispInfo.isPaired) {
            unpairButton.disabled = false;
        }
        else {
            unpairButton.disabled = true;
        }
    }
})();
