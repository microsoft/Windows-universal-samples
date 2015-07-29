//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/other.html", {
        ready: function (element, options) {
            document.getElementById("scenario4Show").addEventListener("click", scenario4Show, false);
            document.getElementById("scenario4Empty").addEventListener("click", scenario4Empty, false);
            document.getElementById("scenario4Clear").addEventListener("click", scenario4Clear, false);
            document.getElementById("scenario4Register").checked = registeredUpdateNotification;
            document.getElementById("scenario4Register").addEventListener("click", scenario4RegisterUnregister, false);
        }
    });

    function displayStatus(statusString) {
        return WinJS.log && WinJS.log(statusString, "sample", "status");
    }

    function displayError(errorString) {
        return WinJS.log && WinJS.log(errorString, "sample", "error");
    }

    var unprocessedUpdateNotification = false;
    var applicationWindowActive = true;
    var registeredUpdateNotification = false;

    function onClipboardContentChanged(e) {
        var timestamp = new Date();

        if (applicationWindowActive) {
            var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.getContent();

            displayStatus("Received Clipboard update notification at " + timestamp.toTimeString() + ". Number of data formats on Clipboard: " + dataPackageView.availableFormats.size);
        } else {
            displayStatus("Received clipboard update notification at " + timestamp.toTimeString() + ". Clipboard can't be accessed due to the application being in the background.");

            // background applications can't access clipboard
            // deferring processing of update notification till later
            unprocessedUpdateNotification = true;
        }
    }

    function onWindowActivated(e) {
        // application's main window has been activated (received focus), and therefore the application can now access clipboard

        // track the activation status of the window
        applicationWindowActive = true;

        if (unprocessedUpdateNotification) {
            // There is an unprocessed update notification - process it now
            var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.getContent();

            displayStatus("Clipboard changed while application was in the background. Number of data formats on Clipboard: " + dataPackageView.availableFormats.size);

            unprocessedUpdateNotification = false;
        }
    }

    function onWindowDeactivated(e) {
        // application's main window has been deactivated (lost focus), and therefore the application can no longer access clipboard

        // track the activation status of the window
        applicationWindowActive = false;
    }

    function scenario4RegisterUnregister() {
        // "Register for notifications" checkbox is being toggled

        if (document.getElementById("scenario4Register").checked) {
            // User wants to check the checkbox (register)

            // register a listener on the ContentChanged event, this will run every time the clipboard changes
            Windows.ApplicationModel.DataTransfer.Clipboard.addEventListener("contentchanged", onClipboardContentChanged);

            // register for window activation events
            window.addEventListener("focus", onWindowActivated, false);
            window.addEventListener("blur", onWindowDeactivated, false);
            registeredUpdateNotification = true;

            displayStatus("Successfully registered for clipboard update notification");
        } else {
            // User wants to uncheck the checkbox (unregister)

            // unregister window activation events
            window.removeEventListener("focus", onWindowActivated, false);
            window.removeEventListener("blur", onWindowDeactivated, false);

            // unregister the ContentChanged event listener
            Windows.ApplicationModel.DataTransfer.Clipboard.removeEventListener("contentchanged", onClipboardContentChanged);
            registeredUpdateNotification = false;

            displayStatus("Successfully unregistered clipboard update notification");
        }
    }

    function scenario4Show() {
        // show the list of available formats
        scenario4DisplayFormats();
    }

    function scenario4Empty() {
        try {
            // empty clipboard
            Windows.ApplicationModel.DataTransfer.Clipboard.clear();

            displayStatus("Clipboard emptied");
            scenario4DisplayFormats();
        } catch (e) {
            // Emptying Clipboard can potentially fail - for example, if another application is holding Clipboard open
            displayError("Error emptying Clipboard: " + e + ". Try again.");
        }
    }

    function scenario4Clear() {
        displayStatus("");
        WinJS.Utilities.empty(document.getElementById("scenario4UpdateStatus"));
        WinJS.Utilities.empty(document.getElementById("scenario4FormatList"));
    }

    function scenario4DisplayFormats() {
        var dataPackageView = Windows.ApplicationModel.DataTransfer.Clipboard.getContent();

        document.getElementById("scenario4FormatList").innerHTML = "Clipboard contains following data formats: <br />";

        if (dataPackageView.availableFormats.size > 0) {
            // Print all format IDs in the datapackage
            dataPackageView.availableFormats.forEach(function (formatId) {
                document.getElementById("scenario4FormatList").innerHTML += formatId + "<br />";
            });
        } else {
            document.getElementById("scenario4FormatList").innerHTML += "Clipboard is empty";
        }
    }
})();
