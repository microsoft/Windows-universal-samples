//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/cancelcommand.html", {
        ready: function (element, options) {
            document.getElementById("cancelCommand").addEventListener("click", cancelCommand_Click, false);
        }
    });

    // Click handler for the 'cancelCommand' button.
    // Demonstrates setting the command to be invoked when the 'escape' key is pressed.
    // Also demonstrates retrieval of the label of the chosen command and setting a callback to a function.
    // A message will be displayed indicating which command was invoked.
    // In this scenario, 'Try again' is selected as the default choice, and the 'escape' key will invoke the command named 'Close'
    function cancelCommand_Click() {
        // Create the message dialog and set its content
        var msg = new Windows.UI.Popups.MessageDialog("No internet connection has been found.");

        // Add commands and set their command handlers
        msg.commands.append(new Windows.UI.Popups.UICommand("Try again", commandInvokedHandler));
        msg.commands.append(new Windows.UI.Popups.UICommand("Close", commandInvokedHandler));

        // Set the command that will be invoked by default
        msg.defaultCommandIndex = 0;

        // Set the command to be invoked when escape is pressed
        msg.cancelCommandIndex = 1;

        // Show the message dialog
        msg.showAsync();
    }

    function commandInvokedHandler(command) {
        // Display message
        WinJS.log && WinJS.log("The '" + command.label + "' command has been selected.", "sample", "status");
    }
})();
