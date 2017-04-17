//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/9_NewWindow.html", {
        ready: function (element, options) {
            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/newwindow_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            document.getElementById("goButton").addEventListener("click", goToUrl, false);

            document.getElementById("webviewmain").navigate("ms-appx-web:///html/newwindow_example.html");

            // Subscribe to the new window event
            document.getElementById("webviewmain").addEventListener("MSWebViewNewWindowRequested", onNewWindowRequested)
        }
    });

    function goToUrl() {
        var destinationUrl = document.getElementById("urlField").value;
        try {
            document.getElementById("webviewmain").navigate(destinationUrl);
        } catch (error) {
            WinJS.log && WinJS.log("\"" + destinationUrl + "\" is not a valid absolute URL.\n", "sdksample", "error");
            return;
        }
    }

    function onNewWindowRequested(e) {
        if (e.referer.search("ms-appx-web") >= 0) {
            // create a new webview and open the page in that webview
            var webView = document.createElement("x-ms-webview");
            webView.navigate(e.uri);
            
            // append the new webview to the page
            document.getElementById("scenario9_right").appendChild(webView);
            WinJS.Utilities.addClass(webView, "small-window");

            var webviewMain = document.getElementById("webviewmain");
            WinJS.Utilities.addClass(webviewMain, "small-window");

            // Prevent the default behavior as we handled the new window event. 
            e.preventDefault();
        }
    }
})();
