//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/defaultclosecommand.html", {
        ready: function (element, options) {
            document.getElementById("defaultCloseCommand").addEventListener("click", defaultCloseCommand_Click, false);
        }
    });

    // Click handler for the 'defaultCloseCommand' button.
    // Demonstrates showing a message dialog with a default close command and content.
    // A message will be displayed indicating that the dialog has been closed.
    // In this scenario, the only command is the default 'Close' command that is used if no other commands are specified.
    function defaultCloseCommand_Click() {
        // Create the message dialog and set its content; it will get a default "Close" command since there aren't any other commands being added
        var msg = new Windows.UI.Popups.MessageDialog("You've exceeded your trial period.");

        // Show the message dialog
        msg.showAsync().done(function () {
            WinJS.log && WinJS.log("The dialog has now been closed.", "samples", "status");
        });
    }
})();
