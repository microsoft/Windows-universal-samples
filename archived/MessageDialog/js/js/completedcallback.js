//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/completedcallback.html", {
        ready: function (element, options) {
            document.getElementById("completedCallback").addEventListener("click", completedCallback_Click, false);
        }
    });

    // Click handler for the 'completedCallback' button.
    // Demonstrates the use of a message dialog with custom commands and using a completed callback instead of delegates.
    // A message will be displayed indicating which command was invoked on the dialog.
    // In this scenario, 'Install updates' is selected as the default choice.
    function completedCallback_Click() {
        // Create the message dialog and set its content
        var msg = new Windows.UI.Popups.MessageDialog("New updates have been found for this program. Would you like to install the new updates?", "Updates available");

        // Add commands and set their CommandIds
        msg.commands.append(new Windows.UI.Popups.UICommand("Install updates", null, 1));
        msg.commands.append(new Windows.UI.Popups.UICommand("Don't install", null, 2));

        // Set the command that will be invoked by default
        msg.defaultCommandIndex = 2;

        // Show the message dialog
        msg.showAsync().done(function (command) {
            if (command) {
                WinJS.log && WinJS.log("The '" + command.label + "' (" + command.id + ") command has been selected.", "sample", "status");
            }
        });
    }
})();
