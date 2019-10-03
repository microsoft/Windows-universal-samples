//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/6_ScriptNotify.html", {
        ready: function (element, options) {
            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/scriptnotify_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            // Set up an event listener for the ScriptNotify event.  This event is fired when javascript
            // code loaded in the WebView calls the window.external.notify method.
            document.getElementById("webview").addEventListener("MSWebViewScriptNotify", scriptNotify);

            document.getElementById("webview").navigate("ms-appx-web:///html/scriptnotify_example.html");
            document.getElementById("reloadButton").addEventListener("click", reloadHtml, false);
        }
    });

    function scriptNotify(e) {
        // When a ScriptNotify event is received, append a message to outputArea to indicate that fact
        var outputArea = document.getElementById("outputArea");
        outputArea.value += ("ScriptNotify event received with data:\n" + e.value + "\n\n");
        outputArea.scrollTop = outputArea.scrollHeight;
    }

    function reloadHtml() {
        document.getElementById("webview").refresh();
    }

})();
