//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario1.html", {
        ready: function (element, options) {
            document.getElementById("attachment").addEventListener("contextmenu", attachmentHandler, false);
        }
    });

    // Converts from page to WinRT coordinates, which take scale factor into consideration.
    function pageToWinRT(pageX, pageY) {
        var zoomFactor = document.documentElement.msContentZoomFactor;
        return { x: (pageX - window.pageXOffset) * zoomFactor,
                 y: (pageY - window.pageYOffset) * zoomFactor };
    }

    function attachmentHandler(e) {
        // Create a menu and add commands with callbacks.
        var menu = new Windows.UI.Popups.PopupMenu();
        menu.commands.append(new Windows.UI.Popups.UICommand("Open with", onOpenWith));
        menu.commands.append(new Windows.UI.Popups.UICommand("Save attachment", onSaveAttachment));

        // We don't want to obscure content, so pass in the position representing the selection area.
        // We registered command callbacks; no need to handle the menu completion event
        menu.showAsync(pageToWinRT(e.pageX, e.pageY)).then(function (invokedCommand) {
            if (invokedCommand === null) {
                // The command is null if no command was invoked.
                WinJS.log && WinJS.log("Context menu dismissed", "sample", "status");
            }
        });

        WinJS.log && WinJS.log("Context menu shown", "sample", "status");
    }

    function onOpenWith() {
        // Add command handler code here.
        WinJS.log && WinJS.log("'Open with' button clicked", "sample", "status");
    }

    function onSaveAttachment() {
        // Add command handler code here.
        WinJS.log && WinJS.log("'Save attachment' button clicked", "sample", "status");
    }
})();
