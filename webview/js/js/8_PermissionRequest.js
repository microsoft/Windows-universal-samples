//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/8_PermissionRequest.html", {
        ready: function (element, options) {
            // Load the contents of the sample HTML file to be displayed in the text area
            WinJS.xhr({ url: "ms-appx-web:///html/permissionrequest_example.html", responseType: "text" }).done(function (request) {
                document.getElementById("htmlString").value = request.responseText;
            });

            document.getElementById("goButton").addEventListener("click", goToUrl, false);

            document.getElementById("webview").navigate("ms-appx-web:///html/permissionrequest_example.html");

            // Subscribe to the permission requested event. When the web page loaded inside webview, tries to access user location
            // this event is sent to the app. 
            document.getElementById("webview").addEventListener("MSWebViewPermissionRequested", managePermissionRequest)
        }
    });

    function goToUrl() {
        var destinationUrl = document.getElementById("urlField").value;
        try {
            document.getElementById("webview").navigate(destinationUrl);
        } catch (error) {
            WinJS.log && WinJS.log("\"" + destinationUrl + "\" is not a valid absolute URL.\n", "sdksample", "error");
            return;
        }
    }

    var deferredReqId;
    function managePermissionRequest(e) {
        if (e.permissionRequest.type == "geolocation") {
            if (e.permissionRequest.uri.search("ms-appx-web") >= 0) {
                // the app can decide the if the webpage can access user location or not on the user behalf.
                e.permissionRequest.allow();
            } else {
                // or else mark the permission status as deferred and asynchronously show the prompt for user to respond.
                deferredReqId = e.permissionRequest.id;
                e.permissionRequest.defer();
                showUserGeolocationPromptUI(e.permissionRequest.uri);
            }
        }
    }
    
    function showUserGeolocationPromptUI(url) {
        // show a message dialog
        var messageDialog = new Windows.UI.Popups.MessageDialog(url + " wants to track your physical location");
        messageDialog.commands.append(
            new Windows.UI.Popups.UICommand("Allow", commandInvokedHandler));
        messageDialog.commands.append(
            new Windows.UI.Popups.UICommand("Deny", commandInvokedHandler));

        messageDialog.defaultCommandIndex = 0;
        messageDialog.cancelCommandIndex = 1;
        
        messageDialog.showAsync();
    }

    function commandInvokedHandler(command) {
        // get the permission context from the webview and set the user consent
        var permissionContext = document.getElementById("webview").getDeferredPermissionRequestById(deferredReqId); 
        if (command.label == "Allow") {
            permissionContext.allow();
        } else {
            permissionContext.deny();
        }
    }
})();
