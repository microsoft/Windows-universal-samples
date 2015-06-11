//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/5_InvokeScript.html", {
        ready: function (element, options) {
            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/script_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            document.getElementById("webview").navigate("ms-appx-web:///html/script_example.html");
            document.getElementById("invokeScriptButton").addEventListener("click", invokeScript, false);
        }
    });

    function invokeScript() {
        // Call the changeText function in the page loaded into the WebView, passing in the
        // value of the textInput field as the argument
        document.getElementById("webview").invokeScriptAsync("changeText", document.getElementById("textInput").value).start();
    }

})();
