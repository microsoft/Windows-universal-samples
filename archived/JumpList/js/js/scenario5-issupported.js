//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var StartScreen = Windows.UI.StartScreen;
    var JumpList = StartScreen.JumpList;

    var page = WinJS.UI.Pages.define("/html/scenario5-issupported.html", {
        ready: function (element, options) {
            Supported.innerText = JumpList.isSupported() ? "supports" : "does not support";
        }
    });
})();
