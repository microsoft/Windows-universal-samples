//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var page = WinJS.UI.Pages.define("/html/scenario2.html", {
        ready: function (element, options) {
            document.getElementById("sampleText").addEventListener("contextmenu", textHandler, false);
        }
    });

    // Converts from client to WinRT coordinates, which take scale factor into consideration.
    function clientToWinRTRect(rect) {
        var zoomFactor = document.documentElement.msContentZoomFactor;
        return {
            x: (rect.left + document.documentElement.scrollLeft - window.pageXOffset) * zoomFactor,
            y: (rect.top + document.documentElement.scrollTop - window.pageYOffset) * zoomFactor,
            width: rect.width * zoomFactor,
            height: rect.height * zoomFactor
        };
    }

    function textHandler(e) {
        e.preventDefault(); // Prevent the default context menu.

        // Only show a context menu if text is selected
        if (isTextSelected()) {
            // Creating a menu with each command specifying a unique command callaback.
            // Seach the command callbacks are unique, there is no need to specify command IDs.
            var menu = new Windows.UI.Popups.PopupMenu();
            menu.commands.append(new Windows.UI.Popups.UICommand("Copy", null, 1));
            menu.commands.append(new Windows.UI.Popups.UICommandSeparator);
            menu.commands.append(new Windows.UI.Popups.UICommand("Highlight", null, 2));
            menu.commands.append(new Windows.UI.Popups.UICommand("Look up", null, 3));

            // We don't want to obscure content, so pass in the position representing the selection area.
            menu.showForSelectionAsync(clientToWinRTRect(window.getSelection().getRangeAt(0).getBoundingClientRect())).done(function (invokedCommand) {
                if (invokedCommand !== null) {
                    switch (invokedCommand.id) {
                        case 1: // Copy
                            var selectedText = window.getSelection();
                            copyTextToClipboard(selectedText);
                            var message = "'Copy' button clicked and '" + /*@static_cast(String)*/selectedText + "' copied to clipboard";
                            WinJS.log && WinJS.log(message, "sample", "status");
                            break;
                        case 2: // Highlight
                            // Add command handler code here.
                            WinJS.log && WinJS.log("'Highlight' button clicked", "sample", "status");
                            break;
                        case 3: // Look up
                            // Add command handler code here.
                            WinJS.log && WinJS.log("'Look up' button clicked", "sample", "status");
                            break;
                        default:
                            break;
                    }
                } else {
                    // The command is null if no command was invoked.
                    WinJS.log && WinJS.log("Context menu dismissed", "sample", "status");
                }
            });
            WinJS.log && WinJS.log("Context menu shown", "sample", "status");
        } else {
            WinJS.log && WinJS.log("Context menu not shown because there is no text selected", "sample", "status");
        }
    };

    function isTextSelected() {
        return (document.getSelection().toString().length > 0);
    };

    function copyTextToClipboard(textToCopy) {
        var dataPackage = new Windows.ApplicationModel.DataTransfer.DataPackage();
        dataPackage.setText(textToCopy);
        Windows.ApplicationModel.DataTransfer.Clipboard.setContent(dataPackage);
    };
})();
