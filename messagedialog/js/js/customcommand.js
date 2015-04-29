//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/customcommand.html", {
        ready: function (element, options) {
            document.getElementById("customCommand").addEventListener("click", customCommand_Click, false);
        }
    });

    // Click handler for the 'customCommand' button.
    // Demonstrates the use of a message dialog with custom commands, command callbacks, and a default command.
    // A message will be displayed indicating which command was invoked on the dialog.
    // In this scenario, 'Install updates' is selected as the default choice.
    function customCommand_Click() {
        // Create the message dialog and set its content
        var msg = new Windows.UI.Popups.MessageDialog("New updates have been found for this program. Would you like to install the new updates?", "Updates available");

        // Add commands and set their command handlers
        msg.commands.append(new Windows.UI.Popups.UICommand("Don't install", function (command) {
            WinJS.log && WinJS.log("The 'Don't install' command has been selected.", "sample", "status");
        }));

        msg.commands.append(new Windows.UI.Popups.UICommand("Install updates", function (command) {
            WinJS.log && WinJS.log("The 'Install updates' command has been selected.", "sample", "status");
        }));

        // Set the command that will be invoked by default
        msg.defaultCommandIndex = 1;

        // Show the message dialog
        msg.showAsync();
    }
})();
