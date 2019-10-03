//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/11_UseWinrt.html", {
        ready: function (element, options) {
            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/usewinrt_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            document.getElementById("webview").addEventListener("MSWebViewNavigationStarting", navigationStarting);

            document.getElementById("webview").navigate("ms-appx-web:///html/usewinrt_example.html");
        }
    });

    function navigationStarting() {
        if (document.getElementById("webview").addWebAllowedObject !== undefined) {
            // create the winrt object, pass it to webview and navigate. This winrt object can be used inside webview till the next navigation.
            document.getElementById("webview").addWebAllowedObject("winRTToastNotification", new WinRTComponent.WinRTToastNotificationWrapper());
        } else {
            WinJS.log && WinJS.log("addWebAllowedObject method is not supported by WebView", "sdksample", "error");
        }
    }
})();