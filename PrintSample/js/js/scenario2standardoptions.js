//// Copyright (c) Microsoft Corporation. All rights reserved

(function () {
    "use strict";
    var page = WinJS.UI.Pages.define("/html/scenario2standardoptions.html", {
        ready: function (element, options) {
            document.getElementById("printButton").addEventListener("click", printButtonHandler, false);

            // Register for Print Contract
            registerForPrintContract();
        }
    });

    function registerForPrintContract() {
        var printManager = Windows.Graphics.Printing.PrintManager.getForCurrentView();
        printManager.onprinttaskrequested = onPrintTaskRequested;
        WinJS.log && WinJS.log("Print Contract registered. Use the Print button to print.", "sample", "status");
    }

    // Variable to hold the document source to print
    var gHtmlPrintDocumentSource = null;

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
            args.setSource(gHtmlPrintDocumentSource);

            // Choose the printer options to be shown.
            // The order in which the options are appended determines the order in which they appear in the UI
            printTask.options.displayedOptions.clear();
            printTask.options.displayedOptions.append(Windows.Graphics.Printing.StandardPrintTaskOptions.copies);
            printTask.options.displayedOptions.append(Windows.Graphics.Printing.StandardPrintTaskOptions.mediaSize);
            printTask.options.displayedOptions.append(Windows.Graphics.Printing.StandardPrintTaskOptions.orientation);
            printTask.options.displayedOptions.append(Windows.Graphics.Printing.StandardPrintTaskOptions.duplex);

            // Preset the default value of the printer option
            printTask.options.mediaSize = Windows.Graphics.Printing.PrintMediaSize.northAmericaLegal;

            // Register the handler for print task completion event
            printTask.oncompleted = onPrintTaskCompleted;
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
        // Get document source to print
        MSApp.getHtmlPrintDocumentSourceAsync(document).then(function (htmlPrintDocumentSource) {
            gHtmlPrintDocumentSource = htmlPrintDocumentSource;

            // If the print contract is registered, the print experience is invoked.
            Windows.Graphics.Printing.PrintManager.showPrintUIAsync();
        });
    }
})();
