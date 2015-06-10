//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/10_AppWebviewComm.html", {
        ready: function (element, options) {
            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/appwebviewcomm_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            document.getElementById("reloadButton").addEventListener("click", reloadPage, false);

            document.getElementById("webview").addEventListener("MSWebViewNavigationStarting", navigationStarting);

            // create winrt object, pass it to the webview and load the page
            reloadPage();
        }
    });

    function navigationStarting() {
        if (document.getElementById("webview").addWebAllowedObject !== undefined) {
            // create the winrt object, pass it to webview. This winrt object can be used inside webview till the next navigation.
            var winRTSharedObject = new WinRTComponent.WinRTSharedObject();
            winRTSharedObject.onnotifyappofprimes = function (e) { appendOutput("Notification from webview for prime number: " + e.target.toString()); }
            winRTSharedObject.setStartNumber(document.getElementById("startnumField").value);
            winRTSharedObject.setEndNumber(document.getElementById("endnumField").value);

            document.getElementById("webview").addWebAllowedObject("winRTSharedObject", winRTSharedObject);
        } else {
            WinJS.log && WinJS.log("addWebAllowedObject method is not supported by WebView", "sdksample", "error");
        }
    }

    function reloadPage() {
        document.getElementById("outputArea").value = "";
        document.getElementById("webview").navigate("ms-appx-web:///html/appwebviewcomm_example.html");
    }

    // Appends a line of text to outputArea
    function appendOutput(message) {
        var outputArea = document.getElementById("outputArea");
        outputArea.value += message + "\n";
        outputArea.scrollTop = outputArea.scrollHeight;
    }
})();