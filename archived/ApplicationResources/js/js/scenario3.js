//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario3.html", {
        ready: function (element, options) {
            document.getElementById("scenario3Show").addEventListener("click", show, false);
        }
    });

    function show() {
        var statusDiv = document.getElementById("statusMessage");
        var str = WinJS.Resources.getString('scenario3Message');
        statusDiv.textContent = str.value;
        statusDiv.lang = str.lang; // Setting the lang attribute to ensure proper font choice 
    }
})();
