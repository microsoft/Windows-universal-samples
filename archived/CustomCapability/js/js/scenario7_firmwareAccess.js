//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var ERROR_SUCCESS = 0;
    var ERROR_INVALID_FUNCTION = 1;

    var page = WinJS.UI.Pages.define("/html/scenario7_firmwareAccess.html", {
        ready: function (element, options) {
            document.getElementById("manufacturer-name").addEventListener("click", onGetManufacturerName, false);
            document.getElementById("secure-boot-status").addEventListener("click", onGetSecureBootStatus, false);
        }
    });
    
    function onGetManufacturerName() {
        var result = RpcClientRt.Smbios.getManufacturerName();
        if (result.errorCode == ERROR_SUCCESS) {
            document.getElementById("manufacturer-name-output").textContent = result.value;
        } else {
            WinJS.log && WinJS.log(`Failed to get the manufacturer name from smbios, error ${result.errorCode}`, "sample", "error");
        }
    }

    function onGetSecureBootStatus() {
        var result = RpcClientRt.Smbios.getSecureBootEnabled();

        switch (result.errorCode) {
            case ERROR_SUCCESS:
                document.getElementById("secure-boot-status-output").textContent = result.value;
                break;

            case ERROR_INVALID_FUNCTION:
                WinJS.log && WinJS.log("Windows was installed using legacy BIOS", "sample", "error");
                break;

            default:
                WinJS.log && WinJS.log(`Failed to retrieve secure boot state from UEFI, error ${result.errorCode}`, "sample", "error");
                break;
        }
    }

})();
