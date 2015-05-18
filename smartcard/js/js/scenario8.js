//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario8.html", {
        ready: function (element, options) {
            document.getElementById("TransmitAPDU").addEventListener("click", Transmit_Click, false);
        }
    });

    function Transmit_Click() {

        if (null === SdkSample.reader || undefined === SdkSample.reader) {
            WinJS.log("Use Scenario One to create a TPM virtual smart card.", "sample", "error");
            return;
        }

        var button = document.getElementById("TransmitAPDU");
        button.disabled = true;

        WinJS.log("Transmitting APDU...", "sample", "status");

        // Read EF.ATR file
        // The command is meant specifically for GIDS cards (such as TPM VSCs), 
        // and will fail on other types.
        var readEfAtrBytes = [0x00, 0xCB, 0x2F, 0x01, 0x02, 0x5C, 0x00, 0xFF];

        var readEfAtr = Windows.Security.Cryptography.CryptographicBuffer.createFromByteArray(readEfAtrBytes);

        SdkSample.getSmartCard().then(
        function (smartCardInfo) {
            return Windows.Devices.SmartCards.SmartCardProvisioning.fromSmartCardAsync(smartCardInfo);
        }).then(
        function (smartCardProvisioning) {
            return smartCardProvisioning.smartCard;
        }).then(
        function (smartCardObject) {
            return smartCardObject.connectAsync();
        }).then(
        function (smartCardConnection) {
            return smartCardConnection.transmitAsync(readEfAtr);
        }).done(
        function (result) {
            WinJS.log("Response: " + Windows.Security.Cryptography.CryptographicBuffer.encodeToHexString(result), "sample", "status");

            button.disabled = false;
        },
        function (error) {
            WinJS.log && WinJS.log("Transmit failed with exception: " + error.toString(), "sample", "error");
            button.disabled = false;
        });
    }
})();