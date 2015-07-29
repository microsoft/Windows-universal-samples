//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/navigationErrors.html", {
        ready: function (element, options) {
            document.getElementById("navigate").addEventListener("click", navigate, false);
        }
    });

    function navigate() {
        // Navigate to a page that does not exist to see msapp-error.html loaded in the iframe
        document.getElementById("iframe").src = "https://www.microsoft.com/nonexistent";
    }
})();
