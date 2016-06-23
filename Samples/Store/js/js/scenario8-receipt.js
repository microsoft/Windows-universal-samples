//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var CurrentAppSimulator = Windows.ApplicationModel.Store.CurrentAppSimulator;

    var page = WinJS.UI.Pages.define("/html/scenario8-receipt.html", {
        ready: function (element, options) {
            document.getElementById("showReceipt").addEventListener("click", showReceipt);
            return SdkSample.configureSimulatorAsync("receipt.xml");
        }
    });

    function showReceipt() {
        CurrentAppSimulator.getAppReceiptAsync().done(function (receipt) {
            output.innerText = receipt;
        }, function () {
            WinJS.log && WinJS.log("Get Receipt failed.", "sample", "error");
        });
    }
})();
