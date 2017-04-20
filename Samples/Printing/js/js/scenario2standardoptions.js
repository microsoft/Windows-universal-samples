//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";

    var printManager = Windows.Graphics.Printing.PrintManager.getForCurrentView();

    var page = WinJS.UI.Pages.define("/html/scenario2standardoptions.html", {
        ready: function (element, options) {
            var printButton = document.getElementById("printButton");
            if (!Windows.Graphics.Printing.PrintManager.isSupported()) {
                WinJS.log && WinJS.log("Printing is not supported.", "sample", "error");

                // Hide the Print button if printing is not supported.
                printButton.style.display = "none";

                // Printing-related event handlers will never be called if printing
                // is not supported, but it's okay to register for them anyway.
            }

            printButton.addEventListener("click", printButtonHandler, false);

            // Register for Print Contract
            printManager.addEventListener("printtaskrequested", onPrintTaskRequested);
        },
        unload: function () {
            printManager.removeEventListener("printtaskrequested", onPrintTaskRequested);
        }
    });

    /// <summary>
    /// Print event handler for printing via the PrintManager API.
    /// In order to ensure a good user experience, the system requires that the app handle the PrintTaskRequested event within the time specified by printEvent.request.deadline. 
    /// Therefore, we use this handler to only create the print task.
    /// The print settings customization can be done when the print document source is requested.
    /// </summary>
    /// <param name="printEvent" type="Windows.Graphics.Printing.PrintTaskRequest">
    /// The event containing the print task request object.
    /// </param>
    function onPrintTaskRequested(printEvent) {
        var printTask = printEvent.request.createPrintTask("Print Sample", function (args) {
            var deferral = args.getDeferral();

            // Choose the printer options to be shown.
            // The order in which the options are appended determines the order in which they appear in the UI
            printTask.options.displayedOptions.clear();
            printTask.options.displayedOptions.push(
                Windows.Graphics.Printing.StandardPrintTaskOptions.copies,
                Windows.Graphics.Printing.StandardPrintTaskOptions.mediaSize,
                Windows.Graphics.Printing.StandardPrintTaskOptions.orientation,
                Windows.Graphics.Printing.StandardPrintTaskOptions.duplex);

            // Preset the default value of the printer option
            printTask.options.mediaSize = Windows.Graphics.Printing.PrintMediaSize.northAmericaLegal;

            // Register the handler for print task completion event
            printTask.addEventListener("completed", onPrintTaskCompleted);

            // Get document source to print
            MSApp.getHtmlPrintDocumentSourceAsync(document).then(function (source) {
                args.setSource(source);
                deferral.complete();
            });
        });
    }

    /// <summary>
    /// Print Task event handler is invoked when the print job is completed.
    /// </summary>
    /// <param name="printTaskCompletionEvent" type="Windows.Graphics.Printing.PrintTaskCompleted">
    /// The event containing the print task completion object.
    /// </param>
    function onPrintTaskCompleted(printTaskCompletionEvent) {
        // Notify the user about the failure
        if (printTaskCompletionEvent.completion === Windows.Graphics.Printing.PrintTaskCompletion.failed) {
            WinJS.log && WinJS.log("Failed to print.", "sample", "error");
        }
    }

    function printButtonHandler() {
        // If the print contract is registered, the print experience is invoked.
        Windows.Graphics.Printing.PrintManager.showPrintUIAsync();
    }
})();
