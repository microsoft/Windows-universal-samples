//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var output;

    var page = WinJS.UI.Pages.define("/html/scenario7.html", {
        ready: function (element, options) {
            output = document.getElementById('output');
            document.getElementById("scenario7GetUrl").addEventListener("click", getUrl, false);
        }
    });

    function getUrl() {
        var url = 'http://api.contoso.com/Service?lang=' + WinJS.Resources.getString('scenario7Param').value;

        WinJS.log && WinJS.log(url, "sample", "status");
    }
})();
