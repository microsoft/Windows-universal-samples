//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/2_NavToString.html", {
        ready: function (element, options) {
            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/simple_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            document.getElementById("loadButton").addEventListener("click", navigateToString, false);
        }
    });

    function navigateToString() {
        document.getElementById("webview").navigateToString(document.getElementById("htmlString").value);
    }

})();
