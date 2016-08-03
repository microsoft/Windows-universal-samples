//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var webview;
    var outputArea;
    var cancelCheckbox;

    var page = WinJS.UI.Pages.define("/html/10_AppWebviewComm.html", {
        ready: function (element, options) {
            outputArea = document.getElementById("outputArea");
            webview = document.getElementById("webview");
            cancelCheckbox = document.getElementById("cancelCheckbox");

            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/appwebviewcomm_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            document.getElementById("reloadButton").addEventListener("click", reloadPage, false);

            webview.addEventListener("MSWebViewNavigationStarting", navigationStarting);

            // create winrt object, pass it to the webview and load the page
            reloadPage();
        }
    });

    function navigationStarting() {
        if (webview.addWebAllowedObject) {
            // Create the winrt object, and pass it to webview. This winrt object can be used inside webview until the next navigation.
            var winRTSharedObject = new WinRTComponent.WinRTSharedObject();
            winRTSharedObject.addEventListener("primefound", function(e) {
                appendOutput("Notification from webview for prime number: " + e.prime);
                e.cancel = cancelCheckbox.checked;
            });
            winRTSharedObject.setStartNumber(document.getElementById("startnumField").value);
            winRTSharedObject.setEndNumber(document.getElementById("endnumField").value);

            webview.addWebAllowedObject("winRTSharedObject", winRTSharedObject);
        } else {
            WinJS.log && WinJS.log("addWebAllowedObject method is not supported by WebView", "sdksample", "error");
        }
    }

    function reloadPage() {
        outputArea.value = "";
        webview.navigate("ms-appx-web:///html/appwebviewcomm_example.html");
    }

    // Appends a line of text to outputArea
    function appendOutput(message) {
        outputArea.value += message + "\n";
        outputArea.scrollTop = outputArea.scrollHeight;
    }
})();