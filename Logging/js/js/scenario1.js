//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("doWin81Mode").addEventListener("click", doWin81Mode, false);
            document.getElementById("doWin10Mode").addEventListener("click", doWin10Mode, false);
        }
    });

    function doWin81Mode()
    {
        new LoggingChannelScenario.LoggingChannelScenario().logWithWin81Constructor();
        WinJS.log && WinJS.log("Complete: Windows 8.1 mode", "sample", "status");
    }

    function doWin10Mode() {
        new LoggingChannelScenario.LoggingChannelScenario().logWithWin10Constructor();
        WinJS.log && WinJS.log("Complete: Windows 10 mode", "sample", "status");
    }
})();
