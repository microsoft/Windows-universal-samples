//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario5.html", {
        ready: function (element, options) {
            document.getElementById("scenario5Show").addEventListener("click", show, false);
        }
    });

    function show() {
        var statusDiv = document.getElementById("statusMessage");
        var str = WinJS.Resources.getString('/errors/scenario5error');
        statusDiv.textContent = str.value;
        statusDiv.lang = str.lang;
    }
})();
