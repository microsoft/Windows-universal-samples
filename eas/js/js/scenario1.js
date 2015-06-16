//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("DeviceInforLaunch").addEventListener("click", deviceInfor, false);
            document.getElementById("DeviceInforReset").addEventListener("click", deviceInforReset, false);
        }
    });

    function deviceInfor() {
        var currentDeviceInfor = new Windows.Security.ExchangeActiveSyncProvisioning.EasClientDeviceInformation();

        document.getElementById("DeviceID").value = currentDeviceInfor.id;
        document.getElementById("OperatingSystem").value = currentDeviceInfor.operatingSystem;
        document.getElementById("FriendlyName").value = currentDeviceInfor.friendlyName;
        document.getElementById("SystemManufacturer").value = currentDeviceInfor.systemManufacturer;
        document.getElementById("SystemProductName").value = currentDeviceInfor.systemProductName;
        document.getElementById("SystemSku").value = currentDeviceInfor.systemSku;
    }

    function deviceInforReset() {
        document.getElementById("DeviceID").value = "";
        document.getElementById("OperatingSystem").value = "";
        document.getElementById("FriendlyName").value = "";
        document.getElementById("SystemManufacturer").value = "";
        document.getElementById("SystemProductName").value = "";
        document.getElementById("SystemSku").value = "";
    }
})();
